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

void AlEnabledCheck::registerMatchers(ast_matchers::MatchFinder *Finder) {
  using namespace ast_matchers;
  Finder->addMatcher(
      callExpr(allOf(callee(cxxMethodDecl(hasName("__isFeatureEnabled"))),
                     hasParent(ifStmt().bind("if_stmt")),
                     hasArgument(0, stringLiteral().bind("feature_name"))))
          .bind("al_enabled"),
      this);
}

void AlEnabledCheck::check(
    const ast_matchers::MatchFinder::MatchResult &Result) {
  auto *If = Result.Nodes.getNodeAs<IfStmt>("if_stmt");
  auto *Feature = Result.Nodes.getNodeAs<StringLiteral>("feature_name");
  auto FeatureStr = Feature->getString();
  auto Then = If->getThen();
  auto Else = If->getElse();

  std::string replacement;
  if (EnabledFeatures.find(FeatureStr) != EnabledFeatures.end()) {
    auto Diag = diag(If->getBeginLoc(), EnabledFeatureDiagWording)
                << FeatureStr;
    SourceRange AlEnabledRange{If->getBeginLoc(), Then->getBeginLoc()};
    Diag << clang::FixItHint::CreateRemoval(AlEnabledRange);

    auto EndLoc = If->getEndLoc();
    if (Else != nullptr) {
      EndLoc = Else->getEndLoc();
    }

    SourceRange RemovalRange{Then->getEndLoc(), EndLoc};
    Diag << clang::FixItHint::CreateRemoval(RemovalRange);
  } else if (DisabledFeatures.find(FeatureStr) != DisabledFeatures.end()) {

    auto Diag = diag(If->getBeginLoc(), DisabledFeatureDiagWording)
                << FeatureStr;

    auto EndLoc = If->getEndLoc();
    if (Else != nullptr) {
      EndLoc = Else->getBeginLoc();
    }
    SourceRange Range{If->getBeginLoc(), EndLoc};
    Diag << clang::FixItHint::CreateRemoval(Range);

    if (Else != nullptr) {
      // Remove the trailing '}'
      SourceRange EndRange{Else->getEndLoc(), If->getEndLoc()};
      Diag << clang::FixItHint::CreateRemoval(EndRange);
    }
  }
}

void AlEnabledCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "EnabledFeatures", RawStringEnabledFeatures);
  Options.store(Opts, "DisabledFeatures", RawStringDisabledFeatures);
}

} // namespace applovin
} // namespace tidy
} // namespace clang
