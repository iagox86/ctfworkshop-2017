CC?=gcc
OBJS=readstack-nz.o

readstack-nz: ${OBJS}
	${CC} -m32 ${CFLAGS} -o readstack-nz readstack-nz.c

clean:
	rm -fv *.o readstack-nz core core.*
