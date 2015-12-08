all: main.o scope.o list.o hash_table.o error.o token.o parser.tab.o lex.yy.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o
	gcc main.o scope.o list.o hash_table.o token.o error.o lex.yy.o parser.tab.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o -ll -o cminor

debug: main.o scope.o list.o hash_table.o error.o token.o parser.tab.o lex.yy.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o
	gcc -g main.o scope.o list.o hash_table.o token.o error.o lex.yy.o parser.tab.o scannerUtil.o expr.o decl.o type.o stmt.o param_list.o symbol.o -ll -o cminor

%.o: %.c *.h
	gcc -c $< -o $@

lex.yy.c: scanner.l
	flex scanner.l

parser.tab.c parser.tab.h: parser.bison
	yacc -d -bparser -v parser.bison

clean:
	rm -f *.o cminor lex.yy.c parser.output parser.tab.*
