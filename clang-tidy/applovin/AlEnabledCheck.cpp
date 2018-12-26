//===--- AlEnabledCheck.cpp - clang-tidy ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AlEnabledCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/MacroArgs.h"
#include "../../clangd/Protocol.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace applovin {

const char AlEnabledCheck::DiagWording[] =
    "do not call %0; consider using exception handling instead";

namespace {

class AlEnabledPPCallbacks : public PPCallbacks {
public:
  explicit AlEnabledPPCallbacks(AlEnabledCheck &Check, Preprocessor &PP, SourceManager &SM)
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
      if (ResultArgToks->isNot(tok::eof))
      {
          const auto FirstArg = *ResultArgToks;
        if (tok::isStringLiteral(FirstArg.getKind())) {
          bool Invalid = false;
          std::string TokStr = PP.getSpelling(FirstArg, &Invalid);
          if (!Invalid) {
            std::string Str = Lexer::Stringify(TokStr);
            Check.diag(Range.getBegin(), Check.DiagWording) << Str;
          }
        }
      }
    }
  }
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
