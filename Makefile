all: main.o lex.yy.o register.o scope.o string_list.o list.o hash_table.o error.o token.o parser.tab.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o gfx.o library.o 
	gcc gfx.o main.o register.o scope.o string_list.o list.o hash_table.o token.o error.o lex.yy.o parser.tab.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o -ll -o cminor -lX11 -lm

%.o: %.c *.h
	gcc -c $< -o $@

lex.yy.c: scanner.l
	flex scanner.l

gfx.o: gfx.c
	gcc -c gfx.c -lX11

library.o: library.c
	gcc -c library.c

parser.tab.c parser.tab.h: parser.bison
	yacc -d -bparser -v parser.bison

clean:
	rm -f *.s *.o cminor lex.yy.c parser.output parser.tab.*
