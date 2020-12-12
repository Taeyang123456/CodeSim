CXX := g++
LLVMCOMPONENTS := all
RTTIFLAG := -fno-rtti
LLVMCONFIG := llvm-config
CXXFLAGS := $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))
SOURCES = codesim.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS = \
                -lclang\
                -lclangTooling\
                -lclangFrontendTool\
                -lclangFrontend\
                -lclangDriver\
                -lclangSerialization\
                -lclangCodeGen\
                -lclangParse\
                -lclangSema\
                -lclangStaticAnalyzerFrontend\
                -lclangStaticAnalyzerCheckers\
                -lclangStaticAnalyzerCore\
                -lclangAnalysis\
                -lclangARCMigrate\
                -lclangEdit\
                -lclangAST\
                -lclangLex\
                -lclangBasic\
                $(shell $(LLVMCONFIG) --libs)

all: $(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@ $< $(CLANGLIBS) $(LLVMLDFLAGS)

.PHONY : clean
clean:
	rm $(EXES) $(OBJECTS)
