# CodeSim
CodeSim Tool for checking the similarity of two C++ codes.

## Run CodeSim
1. cd "src" directory && run make
2. run CodeSim in terminal with "./codesim code1.cpp code2.cpp"
   1. code1 and code2 are source file which need to be compare.
   2. run "./codesim -h" for more information
3. or user can run "build.sh" with root to install codesim
   1. after installing codesim, user may run codesim with "codesim code1.cpp code2.cpp"

## Compile & Environment
The compiler used is g++-9.3，and Makefile is used to finish build job. Clang and LLVM must be installed, and the experiment environment is with clang-11.0.0.
