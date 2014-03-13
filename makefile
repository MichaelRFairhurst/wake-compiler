SHELL=/bin/bash

#OPT=-O3
OPT=-O0 -g
FLAGS=-Iinclude -Igen
CC=cc $(FLAGS)
CPP=g++ $(FLAGS) -std=c++11
TEST=true

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
	ObjectFileGenerator.cpp \
	ObjectFileHeaderData.cpp \
	ObjectFileHeaderRenderer.cpp \
	AddressAllocator.cpp \
	OptionsParser.cpp \
	EntryPointAnalyzer.cpp \
	Linker.cpp \
	SimpleAddressTable.cpp \
	TableFileWriter.cpp \
	TableFileReader.cpp \
	ImportParseTreeTraverser.cpp

CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))

WAKENAMES=ArrayTest.wk \
			InheritanceTestFallthrough.wk \
			InheritanceTest.wk \
			OptionalTypeTest.wk \
			AssertsTest.wk \
			InheritanceTestInterface.wk \
			Main.wk \
			Asserts.wk \
			InheritanceTestOverride.wk \
			MathTest.wk \
			BooleanLogicTest.wk \
			InheritanceTestParent.wk \
			MockPrinter.wk \
			AssignmentsTest.wk \
			PropertyTest.wk

WAKEOBJS=$(addprefix bin/wakeobj/, $(WAKENAMES:.wk=.o))

WAKETABLENAMES=Printer.wk System.wk
WAKETABLEOBJS=$(addprefix bin/waketable/, $(WAKETABLENAMES:.wk=.table))

CNAMES=tree.c type.c parseUtil.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))

GENNAMES=lex.wake.c wake.tab.c objectfile.tab.c lex.objectfile.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))

TESTNAMES=CompilerTests.cpp \
	ObjectSymbolTableTest.cpp \
	ParseTreeTraverserTest.cpp \
	ScopeSymbolTableTest.cpp \
	TypeAnalyzerTest.cpp \
	PropertySymbolTableTest.cpp \
	AddressAllocatorTest.cpp \
	OptionsParserTest.cpp \
	ObjectFileHeaderDataTest.cpp \
	TableFileWriterTest.cpp \
	TableFileReaderTest.cpp

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
	if $(TEST); then $(CPP) $(TESTOBJS) $(CPPOBJS) $(GENOBJS) $(COBJS) -o bin/test -lfl -lboost_unit_test_framework -lboost_filesystem -lboost_system ; fi

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o chivvy
	$(CPP) $(OPT) $(CPPOBJS) bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake -lfl -lboost_filesystem -lboost_system
	@echo
	@echo -- CHEERIO
	@echo

bin/finaltest.js: $(WAKEOBJS) src/wake/stdlib/myobj/std.o
	time ./bin/wake -d bin/waketable -l src/wake/stdlib/myobj/std.o $(WAKEOBJS) -o bin/finaltest.js

bin/wakeobj/Main.o: src/wake/test/Main.wk bin/wake bin/wakeobj/PropertyTest.o bin/wakeobj/Asserts.o bin/wakeobj/ArrayTest.o bin/wakeobj/MathTest.o bin/wakeobj/AssertsTest.o bin/wakeobj/BooleanLogicTest.o bin/wakeobj/OptionalTypeTest.o bin/wakeobj/InheritanceTest.o bin/wakeobj/AssignmentsTest.o $(WAKETABLEOBJS)
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/MockPrinter.o: src/wake/test/MockPrinter.wk bin/wake bin/waketable/Printer.table
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/PropertyTest.o: src/wake/test/PropertyTest.wk bin/wake bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/AssignmentsTest.o: src/wake/test/AssignmentsTest.wk bin/wake bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/ArrayTest.o: src/wake/test/ArrayTest.wk bin/wake bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/MathTest.o: src/wake/test/MathTest.wk bin/wake bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/BooleanLogicTest.o: src/wake/test/BooleanLogicTest.wk bin/wake bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/AssertsTest.o: src/wake/test/AssertsTest.wk bin/wake bin/wakeobj/Asserts.o bin/wakeobj/MockPrinter.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/InheritanceTestOverride.o: src/wake/test/InheritanceTestOverride.wk bin/wake bin/wakeobj/InheritanceTestParent.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/InheritanceTestFallthrough.o: src/wake/test/InheritanceTestFallthrough.wk bin/wake bin/wakeobj/InheritanceTestParent.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/InheritanceTestInterface.o: src/wake/test/InheritanceTestInterface.wk bin/wake bin/wakeobj/InheritanceTestParent.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/InheritanceTest.o: src/wake/test/InheritanceTest.wk bin/wake bin/wakeobj/InheritanceTestInterface.o bin/wakeobj/InheritanceTestOverride.o bin/wakeobj/InheritanceTestFallthrough.o bin/wakeobj/Asserts.o
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/Asserts.o: src/wake/stdlib/Asserts.wk bin/wake $(WAKETABLEOBJS)
	time ./bin/wake -d bin/waketable $< -o $@

bin/wakeobj/%.o: src/wake/test/%.wk bin/wake $(WAKETABLEOBJS)
	time ./bin/wake -d bin/waketable $< -o $@

bin/gen/%.o: gen/%.c gen/wake.tab.c
	$(CC) $(OPT) -c $< -o $@

bin/tests/%.o: src/cpp/test/%.cpp
	$(CPP) -g -c $< -o $@

bin/waketable/%.table: src/wake/stdlib/tables/%.wk bin/wake
	./bin/wake -t $< -d bin/waketable

bin/cpp/%.o: src/cpp/%.cpp gen/wake.tab.c gen/objectfile.tab.c
	$(CPP) $(OPT) -c $< -o $@

bin/c/%.o: src/c/%.c gen/wake.tab.c gen/objectfile.tab.c
	$(CC) $(OPT) -c $< -o $@

gen/%.tab.c: src/bison/%parser.y
	bison -p $* -dg -o $@ $<

gen/lex.%.c: src/flex/%lexer.l gen/wake.tab.c gen/objectfile.tab.c
	flex -P $* -o $@ $<

loc:
	find src -type f -print0 | xargs -0 wc -l makefile include/*

loo:
	@echo
	@echo -- IN THE LOO
	@echo
	rm gen/* || :
	rm bin/tests/* || :
	rm bin/cpp/* || :
	rm bin/gen/* || :
	rm bin/c/* || :
	rm bin/wake || :
	rm bin/wakeobj/* || :
	rm bin/waketable/* || :
	rm bin/finaltest.js || :
	@echo
	@echo -- CLEANED MY ARSE
	@echo
