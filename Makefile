CC=clang
LEX=flex
YACC=bison

CFLAGS=-g

fermat: parser.tab.c lexer.c
	${CC} ${CFLAGS} -o $@ $^

parser.tab.c: parser.y
	${YACC} -d -v $<

lexer.c: lexer.l
	${LEX} -o $@ $<

clean:
	rm -f fermat *.o lexer.c parser.tab.* parser.output
