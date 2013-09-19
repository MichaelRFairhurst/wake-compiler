DEBUG=
OPT=-O3 -Iinclude -Igen
CC=cc $(OPT)
CPP=g++ $(OPT)

CPPNAMES=ObjectSymbolTable.cpp ParseTreeTraverser.cpp SemanticError.cpp PropertySymbolTable.cpp Parser.cpp SemanticErrorPrinter.cpp MockSemanticErrorPrinter.cpp
CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))
CNAMES=tree.c type.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))
GENNAMES=lex.yy.c y.tab.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))
TESTNAMES=CompilerTests.cpp ObjectSymbolTableTest.cpp ParseTreeTraverserTest.cpp
TESTOBJS=$(addprefix bin/tests/, $(TESTNAMES:.cpp=.o))

wake: bin/wake
buggered: loo wake

chivvy: bin/test
	@echo
	@echo -- FANNY AROUND
	@echo
	./bin/test
	@echo
	@echo -- NOW CHIVVY ALONG
	@echo

debug: DEBUG=-t
debug: main

bin/test: $(CPPOBJS) $(GENOBJS) $(COBJS) $(TESTOBJS)
	$(CPP) $(TESTOBJS) $(CPPOBJS) $(GENOBJS) $(COBJS) -o bin/test -lfl -lboost_unit_test_framework

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS) bin/cpp/wake.o chivvy
	$(CPP) $(CPPOBJS) bin/cpp/wake.o $(GENOBJS) $(COBJS) -o bin/wake -lfl
	@echo
	@echo -- CHEERIO, MATE
	@echo

bin/gen/%.o: gen/%.c
	$(CC) -c $< -o $@

bin/tests/%.o: src/test/%.cpp
	$(CPP) -c $< -o $@

bin/cpp/%.o: src/%.cpp gen/y.tab.h
	$(CPP) -c $< -o $@

bin/c/%.o: src/%.c gen/y.tab.h
	$(CC) -c $< -o $@

gen/y.tab.h: src/parser.y
	yacc -dv $(DEBUG) -o gen/y.tab.c src/parser.y

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
