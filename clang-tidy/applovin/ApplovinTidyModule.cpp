//===--- ApplovinTidyModule.cpp - clang-tidy ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "AlEnabledCheck.h"

namespace clang {
namespace tidy {
namespace applovin {

class ApplovinModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<AlEnabledCheck>(
        "applovin-al-enabled");
  }
};

} // namespace applovin

// Register the ApplovinTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<applovin::ApplovinModule>
    X("applovin-module", "Adds applovin lint checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the ApplovinModule.
volatile int ApplovinModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
