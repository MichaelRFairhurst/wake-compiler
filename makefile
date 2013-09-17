DEBUG=
OPT=-O0 -g -Iinclude -Igen
CC=cc $(OPT)
CPP=g++ $(OPT)
main: bin/wake

debug: DEBUG=-t
debug: main

bin/wake: bin/lex.yy.o bin/y.tab.o bin/tree.o bin/type.o bin/wake.o bin/ObjectSymTable.o bin/SemanticError.o
	$(CPP) bin/lex.yy.o bin/y.tab.o bin/tree.o bin/type.o bin/wake.o bin/ObjectSymTable.o bin/SemanticError.o -o bin/wake -lfl

bin/SemanticError.o: src/SemanticError.cpp
	$(CPP) -c src/SemanticError.cpp -o bin/SemanticError.o

bin/ObjectSymTable.o: src/ObjectSymTable.cpp
	$(CPP) -c src/ObjectSymTable.cpp -o bin/ObjectSymTable.o

bin/wake.o: src/wake.cpp
	$(CPP) -c src/wake.cpp -o bin/wake.o

bin/lex.yy.o: gen/y.tab.h gen/lex.yy.c
	$(CC) -c gen/lex.yy.c -o bin/lex.yy.o

bin/y.tab.o: gen/y.tab.c gen/y.tab.h
	$(CC) -c gen/y.tab.c -o bin/y.tab.o

gen/y.tab.h: src/parser.y
	yacc -dv $(DEBUG) -o gen/y.tab.c src/parser.y

bin/tree.o: src/tree.c include/tree.h
	$(CC) -c src/tree.c -o bin/tree.o

bin/type.o: src/type.c include/type.h
	$(CC) -c src/type.c -o bin/type.o

gen/lex.yy.c: src/lexer.l gen/y.tab.h
	flex -o gen/lex.yy.c src/lexer.l

clean:
	rm gen/y.tab.c gen/y.tab.h gen/y.output gen/lex.yy.c bin/wake bin/tree.o bin/type.o bin/lex.yy.o bin/y.tab.o
