CC?=gcc
OBJS=writestack.o

writestack: ${OBJS}
	${CC} -m32 ${CFLAGS} -o writestack writestack.c

clean:
	rm -fv *.o writestack core core.*
