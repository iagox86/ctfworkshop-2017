CC?=gcc
OBJS=readstack.o

readstack: ${OBJS}
	${CC} -m32 ${CFLAGS} -o readstack readstack.c

clean:
	rm -fv *.o readstack core core.*
