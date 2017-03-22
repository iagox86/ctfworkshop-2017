CC?=gcc
OBJS=readmem-nz.o

readmem-nz: ${OBJS}
	${CC} -m32 ${CFLAGS} -o readmem-nz readmem-nz.c

clean:
	rm -fv *.o readmem-nz core core.*
