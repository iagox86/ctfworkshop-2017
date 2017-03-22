CC?=gcc
OBJS=writemem.o

writemem: ${OBJS}
	${CC} -m32 ${CFLAGS} -o writemem writemem.c

clean:
	rm -fv *.o writemem core core.*
