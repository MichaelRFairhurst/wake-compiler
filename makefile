DEBUG=
main: wake

debug: DEBUG=-t
debug: main

wake: lex.yy.c y.tab.c tree.o type.o
	cc -O3 lex.yy.c y.tab.c tree.o type.o -o wake -lfl

y.tab.h: parser.y
	yacc -dv $(DEBUG) parser.y

tree.o: tree.c tree.h
	cc -O3 -c tree.c -o tree.o

type.o: type.c type.h
	cc -O3 -c type.c -o type.o

lex.yy.c: lexer.l y.tab.h
	flex lexer.l

clean:
	rm lex.yy.c y.tab.c y.tab.h wake tree.o type.o
