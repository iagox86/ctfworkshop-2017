CC?=gcc
OBJS=dontcrash.o

dontcrash: ${OBJS}
	${CC} -m32 ${CFLAGS} -o dontcrash dontcrash.c

clean:
	rm -fv *.o dontcrash core core.*
