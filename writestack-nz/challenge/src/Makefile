CC?=gcc
OBJS=writestack-nz.o

writestack-nz: ${OBJS}
	${CC} -m32 ${CFLAGS} -o writestack-nz writestack-nz.c

clean:
	rm -fv *.o writestack-nz core core.*
