//===--- AlEnabledCheck.cpp - clang-tidy ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AlEnabledCheck.h"
#include "../../clangd/Protocol.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/MacroArgs.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace applovin {

const char AlEnabledCheck::EnabledFeatureDiagWording[] =
    "Feature %0 is fully enabled, consider scaling";
const char AlEnabledCheck::DisabledFeatureDiagWording[] =
    "Feature %0 is disabled, consider removing";

namespace {

class AlEnabledPPCallbacks : public PPCallbacks {
public:
  explicit AlEnabledPPCallbacks(AlEnabledCheck &Check, Preprocessor &PP,
                                SourceManager &SM)
      : Check(Check), PP(PP), SM(SM) {}

  void MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                    SourceRange Range, const MacroArgs *Args) override;

private:
  AlEnabledCheck &Check;
  Preprocessor &PP;
  SourceManager &SM;
};
} // namespace

void AlEnabledPPCallbacks::MacroExpands(const Token &MacroNameTok,
                                        const MacroDefinition &MD,
                                        SourceRange Range,
                                        const MacroArgs *Args) {
  const auto *II = MacroNameTok.getIdentifierInfo();
  if (!II)
    return;

  if (II->getName() == "AL_ENABLED") {
    if (Args->getNumMacroArguments() > 0) {
      const auto *ResultArgToks = Args->getUnexpArgument(0);
      if (ResultArgToks->isNot(tok::eof)) {
        const auto FirstArg = *ResultArgToks;
        if (tok::isStringLiteral(FirstArg.getKind())) {
          bool Invalid = false;
          std::string TokStr = PP.getSpelling(FirstArg, &Invalid);
          if (!Invalid) {
            std::string Str = Lexer::Stringify(TokStr);
            if (Str.size() >= 4) {
              if (Str.substr(0, 2) == "\\\"") {
                Str = Str.substr(2);
              }

              if (Str.substr(Str.size() - 2) == "\\\"") {
                Str = Str.substr(0, Str.size() - 2);
              }
            }

            if (Check.EnabledFeatures.find(Str) !=
                Check.EnabledFeatures.end()) {
              Check.diag(Range.getBegin(), Check.EnabledFeatureDiagWording)
                  << Str;
            } else if (Check.DisabledFeatures.find(Str) !=
                       Check.DisabledFeatures.end()) {
              Check.diag(Range.getBegin(), Check.DisabledFeatureDiagWording)
                  << Str;
            }
          }
        }
      }
    }
  }
}

void AlEnabledCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "EnabledFeatures", RawStringEnabledFeatures);
  Options.store(Opts, "DisabledFeatures", RawStringDisabledFeatures);
}

void AlEnabledCheck::registerPPCallbacks(CompilerInstance &Compiler) {
  if (!getLangOpts().CPlusPlus)
    return;

  Compiler.getPreprocessor().addPPCallbacks(
      llvm::make_unique<AlEnabledPPCallbacks>(*this, Compiler.getPreprocessor(),
                                              Compiler.getSourceManager()));
}
} // namespace applovin
} // namespace tidy
} // namespace clang
