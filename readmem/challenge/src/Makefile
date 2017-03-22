CC?=gcc
OBJS=readmem.o

readmem: ${OBJS}
	${CC} -m32 ${CFLAGS} -o readmem readmem.c

clean:
	rm -fv *.o readmem core core.*
