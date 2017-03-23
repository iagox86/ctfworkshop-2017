CC?=gcc
OBJS=readfile-nz.o

readfile-nz: ${OBJS}
	${CC} -m32 ${CFLAGS} -o readfile-nz readfile-nz.c

clean:
	rm -fv *.o readfile-nz core core.*
