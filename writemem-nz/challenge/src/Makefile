CC?=gcc
OBJS=writemem-nz.o

writemem-nz: ${OBJS}
	${CC} -m32 ${CFLAGS} -o writemem-nz writemem-nz.c

clean:
	rm -fv *.o writemem-nz core core.*
