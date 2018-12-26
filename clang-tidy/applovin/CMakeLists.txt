set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyApplovinModule
  ApplovinTidyModule.cpp
  AlEnabledCheck.cpp

  LINK_LIBS
  clangAnalysis
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangSerialization
  clangTidy
  clangTidyUtils
  clangTooling
  clangDaemon
  )
