################################################
# Source Code for the Original Compiler for the
# Programming Language Wake
#
# %filename
#
# Licensed under the MIT license
# See LICENSE.TXT for details
#
# Author: Michael Fairhurst
# Revised By:
#
#################################################

SHELL=/bin/bash

#OPT=-O3
OPT=-O0 -g
FLAGS=-Iinclude -Igen
CC=cc $(FLAGS)
CPP=g++ $(FLAGS)
TEST=false

CPPNAMES= \
	VarRef.cpp \
	ClassVarRef.cpp \
	VarDecl.cpp \
	SpecializableVarDecl.cpp \
	SpecializablePureType.cpp \
	PureType.cpp \
	PureTypeArray.cpp \
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
	DerivedPropertySymbolTable.cpp \
	TempPropertySymbolTable.cpp \
	AnnotationTreeTraverser.cpp \
	UnifyingType.cpp \
	AstCreator.cpp \
	ast/Catch.cpp \
	ast/MethodSegment.cpp \
	ast/Declaration.cpp \
	ast/Exists.cpp \
	ast/For.cpp \
	ast/Foreach.cpp \
	ast/ForeachInAliased.cpp \
	ast/ForeachInExplicitType.cpp \
	ast/IfElseWhile.cpp \
	ast/OtherExpression.cpp \
	ast/OtherStatement.cpp \
	ast/Return.cpp \
	ast/ExpressionUsabilityCatcher.cpp \
	ast/ExpressionStatement.cpp \
	ast/Throw.cpp \
	ast/ExpressionErrorCatcher.cpp \
	ast/Invocation.cpp \
	ast/MethodInvocationBase.cpp \
	ast/MethodInvocation.cpp \
	ast/EarlyBailoutMethodInvocation.cpp \
	ast/StatementErrorCatcher.cpp \
	ast/Lambda.cpp \
	ast/LambdaReturn.cpp \
	ast/Retrieval.cpp \
	ast/UnsafeCast.cpp

CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))

WAKETABLENAMES=List.wk Text.wk Num.wk Bool.wk Exception.wk
WAKETABLEOBJS=$(addprefix bin/waketable/lang/, $(WAKETABLENAMES:.wk=.table))
WAKETABLEINCLUDES=$(addprefix gen/, $(WAKETABLENAMES:.wk=.table.h))

STDWAKETABLENAMES=io/Printer.wk lang/List.wk lang/Text.wk lang/Num.wk lang/Bool.wk io/File.wk io/FilePath.wk io/FileSystem.wk std/Argv.wk lang/Exception.wk std/System.wk std/Map.wk std/UndefinedIndexException.wk
STDWAKETABLEOBJS=$(addprefix bin/waketable/, $(STDWAKETABLENAMES:.wk=.table))

CNAMES=tree.c type.c parseUtil.c error.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))

GENNAMES=lex.wake.c wake.tab.c objectfile.tab.c lex.objectfile.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))

ifeq ( false , $(TEST) )
TESTNAMES=
else
TESTNAMES=CompilerTests.cpp \
	ClassSpaceSymbolTableTest.cpp \
	Integration.cpp \
	ScopeSymbolTableTest.cpp \
	TypeAnalyzerTest.cpp \
	PropertySymbolTableTest.cpp \
	AddressAllocatorTest.cpp \
	OptionsParserTest.cpp \
	ObjectFileHeaderDataTest.cpp \
	TableFileWriterTest.cpp \
	TableFileDataSets.cpp \
	TableFileReaderTest.cpp \
	DerivedPropertySymbolTableTest.cpp
endif

TESTOBJS=$(addprefix bin/tests/, $(TESTNAMES:.cpp=.o))

prefix=/usr/local

default: default-notests
	cd src/wake/stdlib && make WAKE=../../../bin/wake
	cd src/wake/test && make WAKE=../../../bin/wake

default-notests: bin/wake $(STDWAKETABLEOBJS)

install: bin/wake
	mkdir -p $(prefix)/bin
	#mkdir -p $(prefix)/share/wake/stdtable
	#mkdir -p $(prefix)/share/wake/stdobj
	install -m 0755 bin/wake $(prefix)/bin
	#install -m 0644 $(WAKETABLEOBJS) $(prefix)/share/wake/stdtable
	#install -m 0644 bin/wakeobj/std.o $(prefix)/share/wake/stdobj

.PHONY: install

src/cpp/test/Integration.cpp: $(wildcard src/cpp/test/integration/*)

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
	@echo remove TEST=true or set to false to disable tests
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

bin/wakeobj/std.o: src/wake/stdlib/src/extern/internals/std.js src/wake/stdlib/js_to_wakeobj.sh
	cd src/wake/stdlib && make bin/wakeobj/std.o
	cd ../../../
	cp src/wake/stdlib/bin/wakeobj/std.o bin/wakeobj

$(OBJECTFILES) : bin/wake
$(TABLEFILES) : bin/wake

bin/wakeobj/%.o: bin/wake
	cd src/wake/stdlib && make WAKE=../../../bin/wake $@
	cp src/wake/stdlib/$@ $@

bin/waketable/%.table: bin/wake
	cd src/wake/stdlib && make WAKE=../../../bin/wake $@
	cp src/wake/stdlib/$@ $@

bin/waketable/lang/%.table: bin/wake-nolib
	cd src/wake/stdlib && make WAKE=../../../bin/wake-nolib $@
	cp src/wake/stdlib/$@ $@

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
	bison --report=states --locations -p $* -dg -o $@ $<

gen/%.table.h: bin/waketable/lang/%.table bin/wake-nolib
	xxd -i $< $@

gen/lex.%.c: src/flex/%lexer.l gen/wake.tab.c gen/objectfile.tab.c
	flex -P $* -o $@ $<

loc:
	find src -type f -print0 | xargs -0 wc -l makefile include/* include/ast/* js_to_wakeobj.sh

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
	rm bin/wake-nolib || :
	rm bin/wakeobj/* || :
	rm bin/waketable/* || :
	rm bin/finaltest.js || :
	cd src/wake/stdlib && make clean
	cd src/wake/test && make clean
	@echo
	@echo -- CLEANED MY ARSE
	@echo
