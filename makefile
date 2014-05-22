SHELL=/bin/bash

#OPT=-O3
OPT=-O0 -g
FLAGS=-Iinclude -Igen
CC=cc $(FLAGS)
CPP=g++ $(FLAGS) -std=c++11
TEST=true

CPPNAMES= \
	ClassSpaceSymbolTable.cpp \
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
	ImportParseTreeTraverser.cpp \
	TypeParameterizer.cpp \
	DerivedPropertySymbolTable.cpp

CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))

WAKETABLENAMES=Printer.wk System.wk List.wk Text.wk Num.wk Bool.wk File.wk FilePath.wk FileSystem.wk
WAKETABLEOBJS=$(addprefix bin/waketable/, $(WAKETABLENAMES:.wk=.table))
WAKETABLEINCLUDES=$(addprefix gen/, $(WAKETABLENAMES:.wk=.table.h))

CNAMES=tree.c type.c parseUtil.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))

GENNAMES=lex.wake.c wake.tab.c objectfile.tab.c lex.objectfile.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))

TESTNAMES=CompilerTests.cpp \
	ClassSpaceSymbolTableTest.cpp \
	ParseTreeTraverserTest.cpp \
	ScopeSymbolTableTest.cpp \
	TypeAnalyzerTest.cpp \
	PropertySymbolTableTest.cpp \
	AddressAllocatorTest.cpp \
	OptionsParserTest.cpp \
	ObjectFileHeaderDataTest.cpp \
	TableFileWriterTest.cpp \
	TableFileReaderTest.cpp \
	DerivedPropertySymbolTableTest.cpp

TESTOBJS=$(addprefix bin/tests/, $(TESTNAMES:.cpp=.o))

include wmake.mk

windowsbuildready: $(GENOBJS) $(WAKETABLEINCLUDES) bin/wakeobj/std.o
	zip windowsbuildready -r .

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

bin/test: $(CPPOBJS) $(GENOBJS) $(COBJS) $(TESTOBJS) bin/cpp/LibraryLoader-nolib.o
	@echo add TEST=false to skip
	if $(TEST); then $(CPP) $(TESTOBJS) $(CPPOBJS) bin/cpp/LibraryLoader-nolib.o $(GENOBJS) $(COBJS) -o bin/test -lfl -lboost_unit_test_framework -lboost_filesystem -lboost_system ; fi

bin/wake-nolib: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o chivvy bin/cpp/LibraryLoader-nolib.o
	$(CPP) $(OPT) $(CPPOBJS) bin/cpp/LibraryLoader-nolib.o bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake-nolib -lfl -lboost_filesystem -lboost_system -DSTDLIB_ONLY_DEFINE_PRIMITIVES
	@echo
	@echo -- CHEERIO
	@echo

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o bin/cpp/LibraryLoader-withlib.o chivvy bin/wake-nolib $(WAKETABLEOBJS)
	$(CPP) $(OPT) $(CPPOBJS) bin/cpp/LibraryLoader-withlib.o bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake -lfl -lboost_filesystem -lboost_system
	@echo
	@echo -- CHEERIO
	@echo

bin/wakeobj/std.o: src/wake/stdlib/myobj/std.o js_to_wakeobj.sh
	cat $< | ./js_to_wakeobj.sh > $@

$(OBJECTFILES) : bin/wake
$(TABLEFILES) : bin/wake

bin/waketable/FilePath.table: bin/waketable/File.table bin/waketable/Text.table bin/waketable/Num.table bin/waketable/Bool.table
	echo this overrides the wildcard that doesnt work

bin/waketable/FileSystem.table: bin/waketable/File.table bin/waketable/Text.table bin/waketable/Num.table

bin/waketable/%.table: src/wake/stdlib/tables/%.wk bin/wake-nolib
	./bin/wake-nolib -d bin/waketable -t $< -d bin/waketable

bin/waketable/Printer.table: bin/waketable/Num.table bin/waketable/Text.table bin/wake-nolib src/wake/stdlib/tables/Printer.wk
bin/waketable/System.table: bin/waketable/Num.table bin/wake-nolib src/wake/stdlib/tables/System.wk
bin/waketable/File.table: src/wake/stdlib/tables/File.wk bin/waketable/Text.table bin/waketable/Num.table bin/waketable/Bool.table
bin/waketable/List.table: src/wake/stdlib/tables/List.wk bin/waketable/Num.table bin/waketable/Bool.table

bin/waketable/Text.table bin/waketable/Num.table bin/waketable/Bool.table: src/wake/stdlib/tables/Primitives.wk bin/wake-nolib
	./bin/wake-nolib -d bin/waketable -t $< -d bin/waketable

bin/gen/%.o: gen/%.c gen/wake.tab.c
	$(CC) $(OPT) -c $< -o $@

bin/tests/%.o: src/cpp/test/%.cpp
	$(CPP) -g -c $< -o $@

bin/cpp/%.o: src/cpp/%.cpp gen/wake.tab.c gen/objectfile.tab.c
	$(CPP) $(OPT) -c $< -o $@

bin/cpp/LibraryLoader-withlib.o: src/cpp/LibraryLoader.cpp $(WAKETABLEINCLUDES)
	$(CPP) $(OPT) -c $< -o $@ -DCOMPILE_IN_PRIMITIVE_TYPES

bin/cpp/LibraryLoader-nolib.o: src/cpp/LibraryLoader.cpp
	$(CPP) $(OPT) -c $< -o $@

bin/c/%.o: src/c/%.c gen/wake.tab.c gen/objectfile.tab.c
	$(CC) $(OPT) -c $< -o $@

gen/%.tab.c: src/bison/%parser.y
	bison --report=states -p $* -dg -o $@ $<

gen/%.table.h: bin/waketable/%.table bin/wake-nolib
	xxd -i $< $@

gen/lex.%.c: src/flex/%lexer.l gen/wake.tab.c gen/objectfile.tab.c
	flex -P $* -o $@ $<

loc:
	find src -type f -print0 | xargs -0 wc -l makefile include/* js_to_wakeobj.sh

loo: clean
	@echo
	@echo -- IN THE LOO
	@echo
	rm gen/* || :
	rm bin/tests/* || :
	rm bin/cpp/* || :
	rm bin/gen/* || :
	rm bin/c/* || :
	rm bin/wake || :
	rm bin/wake-nolib || :
	rm bin/wakeobj/* || :
	rm bin/waketable/* || :
	rm bin/finaltest.js || :
	rm callwmake || :
	make -f wmake.mk clean
	@echo
	@echo -- CLEANED MY ARSE
	@echo
