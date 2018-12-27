//===--- AlEnabledCheck.h - clang-tidy --------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_APPLOVIN_ALENABLEDCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_APPLOVIN_ALENABLEDCHECK_H

#include "../ClangTidy.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallSet.h"
#include <iostream>

namespace clang {
namespace tidy {
namespace applovin {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/applovin-al-enabled.html
class AlEnabledCheck : public ClangTidyCheck {
public:
  AlEnabledCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context),
        RawStringEnabledFeatures(
            Options.getLocalOrGlobal("EnabledFeatures", "")),
        RawStringDisabledFeatures(Options.getLocalOrGlobal(
            "DisabledFeatures", "")) {
            SmallVector<llvm::StringRef, 10> EnabledFeaturesVec;
            SmallVector<llvm::StringRef, 10> DisabledFeaturesVec;

            StringRef(RawStringEnabledFeatures).split(EnabledFeaturesVec, ',');
            StringRef(RawStringDisabledFeatures).split(DisabledFeaturesVec, ',');

            for ( const auto & feature : EnabledFeaturesVec )
            {
                std::cerr << feature.str() << std::endl;
                EnabledFeatures.insert( feature );
            }

            for ( const auto & feature : DisabledFeaturesVec )
            {
                std::cerr << feature.str() << std::endl;
                DisabledFeatures.insert( feature );
            }
        }

  void registerPPCallbacks(CompilerInstance &Compiler) override;
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;

  static const char EnabledFeatureDiagWording[];
  static const char DisabledFeatureDiagWording[];

  std::set<std::string> EnabledFeatures;
  std::set<std::string> DisabledFeatures;

private:
  std::string RawStringEnabledFeatures;
  std::string RawStringDisabledFeatures;
};

} // namespace applovin
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_APPLOVIN_ALENABLEDCHECK_H
