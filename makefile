SHELL=/bin/bash

#OPT=-O3
OPT=-O3
FLAGS=-Iinclude -Igen
CC=cc $(FLAGS)
CPP=g++ $(FLAGS)

CPPNAMES= \
	ObjectSymbolTable.cpp \
	ParseTreeTraverser.cpp \
	SemanticError.cpp \
	PropertySymbolTable.cpp \
	Parser.cpp \
	SemanticErrorPrinter.cpp \
	MockSemanticErrorPrinter.cpp \
	ScopeSymbolTable.cpp \
	TypeAnalyzer.cpp \
	ErrorTracker.cpp \
	TypeChecker.cpp \
	ClassParseTreeTraverser.cpp \
	MethodSignatureParseTreeTraverser.cpp \
	LibraryLoader.cpp \
	CodeGenerator.cpp \
	AddressAllocator.cpp \
	OptionsParser.cpp \
	EntryPointAnalyzer.cpp \
	Linker.cpp

CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))
CNAMES=tree.c type.c parseUtil.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))
GENNAMES=lex.wake.c wake.tab.c objectfile.tab.c lex.objectfile.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))
TEST=true
TESTNAMES=CompilerTests.cpp ObjectSymbolTableTest.cpp ParseTreeTraverserTest.cpp ScopeSymbolTableTest.cpp TypeAnalyzerTest.cpp PropertySymbolTableTest.cpp AddressAllocatorTest.cpp OptionsParserTest.cpp
TESTOBJS=$(addprefix bin/tests/, $(TESTNAMES:.cpp=.o))

chatup: bin/finaltest.js
	@echo
	@echo -- CHAT UP THE BIN
	@echo -- add TEST=false to skip
	@echo
	if $(TEST) ; then time node bin/finaltest.js ; fi
	@echo
	@echo -- BLINDING BUILD, MATE
	@echo

chivvy: bin/test
	@echo
	@echo -- FANNY AROUND
	@echo -- add TEST=false to skip
	@echo
	if $(TEST) ; then ./bin/test -p yes ; fi
	touch chivvy
	@echo
	@echo -- NOW CHIVVY ALONG
	@echo

bin/test: $(CPPOBJS) $(GENOBJS) $(COBJS) $(TESTOBJS)
	@echo add TEST=false to skip
	if $(TEST); then $(CPP) $(TESTOBJS) $(CPPOBJS) $(GENOBJS) $(COBJS) -o bin/test -lfl -lboost_unit_test_framework ; fi

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o chivvy
	$(CPP) $(OPT) $(CPPOBJS) bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake -lfl
	@echo
	@echo -- CHEERIO
	@echo

bin/finaltest.js: bin/wake finaltest.wk
	time ./bin/wake finaltest.wk -o bin/finaltest.js

bin/gen/%.o: gen/%.c gen/wake.tab.c
	$(CC) $(OPT) -c $< -o $@

bin/tests/%.o: src/test/%.cpp
	$(CPP) -g -c $< -o $@

bin/cpp/%.o: src/%.cpp gen/wake.tab.c gen/objectfile.tab.c
	$(CPP) $(OPT) -c $< -o $@

bin/c/%.o: src/%.c gen/wake.tab.c gen/objectfile.tab.c
	$(CC) $(OPT) -c $< -o $@

gen/wake.tab.c: src/wakeparser.y
	bison -p wake -dgv -o gen/wake.tab.c src/wakeparser.y

gen/lex.wake.c: src/wakelexer.l gen/wake.tab.c
	flex -P wake -o gen/lex.wake.c src/wakelexer.l

gen/objectfile.tab.c: src/objectfileparser.y
	bison -p objectfile -dgv -o gen/objectfile.tab.c src/objectfileparser.y

gen/lex.objectfile.c: src/objectfilelexer.l gen/objectfile.tab.c
	flex -P objectfile -o gen/lex.objectfile.c src/objectfilelexer.l

loo:
	@echo
	@echo -- IN THE LOO
	@echo
	rm gen/* || :
	rm bin/tests/* || :
	rm bin/cpp/* || :
	rm bin/gen/* || :
	rm bin/c/* || :
	rm bin/* || :
	@echo
	@echo -- CLEANED MY ARSE
	@echo
