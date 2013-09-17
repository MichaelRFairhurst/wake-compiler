DEBUG=
OPT=-O0 -g -Iinclude -Igen
CC=cc $(OPT)
CPP=g++ $(OPT)

CPPNAMES=ObjectSymTable.cpp SemanticError.cpp wake.cpp
CPPOBJS=$(addprefix bin/cpp/, $(CPPNAMES:.cpp=.o))
CNAMES=tree.c type.c
COBJS=$(addprefix bin/c/, $(CNAMES:.c=.o))
GENNAMES=lex.yy.c y.tab.c
GENOBJS=$(addprefix bin/gen/, $(GENNAMES:.c=.o))

main: bin/wake

debug: DEBUG=-t
debug: main

bin/wake: $(CPPOBJS) $(GENOBJS) $(COBJS)
	$(CPP) $(CPPOBJS) $(GENOBJS) $(COBJS) -o bin/wake -lfl

bin/gen/%.o: gen/%.c
	$(CC) -c $< -o $@

bin/cpp/%.o: src/%.cpp gen/y.tab.h
	$(CPP) -c $< -o $@

bin/c/%.o: src/%.c gen/y.tab.h
	$(CC) -c $< -o $@

gen/y.tab.h: src/parser.y
	yacc -dv $(DEBUG) -o gen/y.tab.c src/parser.y

gen/lex.yy.c: src/lexer.l gen/y.tab.h
	flex -o gen/lex.yy.c src/lexer.l

clean:
	rm gen/* || :
	rm bin/cpp/* || :
	rm bin/gen/* || :
	rm bin/c/* || :
	rm bin/* || :
