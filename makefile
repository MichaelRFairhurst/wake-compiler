SHELL=/bin/bash

OPT=-O3
#OPT=-O0 -g
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
	EntryPointAnalyzer.cpp

CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))
CNAMES=tree.c type.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))
GENNAMES=lex.yy.c y.tab.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))
TESTNAMES=CompilerTests.cpp ObjectSymbolTableTest.cpp ParseTreeTraverserTest.cpp ScopeSymbolTableTest.cpp TypeAnalyzerTest.cpp PropertySymbolTableTest.cpp AddressAllocatorTest.cpp OptionsParserTest.cpp
TESTOBJS=$(addprefix bin/tests/, $(TESTNAMES:.cpp=.o))

chatup: bin/finaltest.js
	@echo
	@echo -- CHAT UP THE BIN
	@echo
	time node bin/finaltest.js
	@echo
	@echo -- BLINDING BUILD
	@echo

buggered: loo chatup

chivvy: bin/test
	@echo
	@echo -- FANNY AROUND
	@echo
	./bin/test -p yes
	touch chivvy
	@echo
	@echo -- NOW CHIVVY ALONG
	@echo

bin/test: $(CPPOBJS) $(GENOBJS) $(COBJS) $(TESTOBJS)
	$(CPP) $(TESTOBJS) $(CPPOBJS) $(GENOBJS) $(COBJS) -o bin/test -lfl -lboost_unit_test_framework

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o chivvy
	$(CPP) $(OPT) $(CPPOBJS) bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake -lfl
	@echo
	@echo -- CHEERIO
	@echo

bin/finaltest.js: bin/wake finaltest.wk
	time wake finaltest.wk -o bin/finaltest.js

bin/gen/%.o: gen/%.c
	$(CC) $(OPT) -c $< -o $@

bin/tests/%.o: src/test/%.cpp
	$(CPP) -g -c $< -o $@

bin/cpp/%.o: src/%.cpp gen/y.tab.h
	$(CPP) $(OPT) -c $< -o $@

bin/c/%.o: src/%.c gen/y.tab.h
	$(CC) $(OPT) -c $< -o $@

gen/y.tab.h: src/parser.y
	yacc -dv -o gen/y.tab.c src/parser.y

gen/lex.yy.c: src/lexer.l gen/y.tab.h
	flex -o gen/lex.yy.c src/lexer.l

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
