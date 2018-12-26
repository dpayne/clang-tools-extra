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
      : ClangTidyCheck(Name, Context) {}
  void registerPPCallbacks(CompilerInstance &Compiler) override;

  static const char DiagWording[];
};

} // namespace applovin
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_APPLOVIN_ALENABLEDCHECK_H
