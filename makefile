CC = gcc
CFLAGS = -Wall 
LDFLAGS = 

xfse : utils.o xfse.o
	${CC} ${CFLAGS} $^ ${LDFLAGS} -o $@

xfse.o : xfse.c xfse.h
	${CC} ${CFLAGS} -c $^

utils.o : utils.c utils.h 
	${CC} ${CFLAGS} -c $^

run: xfse
	./$^

clean:
	rm -f *.o *.gch xfse