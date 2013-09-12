DEBUG=
main: wake

debug: DEBUG=-t
debug: main

wake: lex.yy.c y.tab.c tree.o
	cc lex.yy.c y.tab.c tree.o -o wake -lfl

y.tab.h: parser.y
	yacc -d $(DEBUG) parser.y

tree.o: tree.c tree.h
	cc -c tree.c -o tree.o

lex.yy.c: lexer.l y.tab.h
	flex lexer.l

clean:
	rm lex.yy.c y.tab.c y.tab.h wake tree.o
