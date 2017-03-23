#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  struct stat statbuf;
  if (stat(argv[1], &statbuf) == -1) {
    printf("Usage: %s <code.bin>\n", argv[0]);
    exit(0);
  }

  void * a = mmap(0, statbuf.st_size, PROT_EXEC |PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  printf("allocated %zd bytes of executable memory at: %p\n", (size_t)statbuf.st_size, a);

  FILE *file = fopen(argv[1], "rb");
  if(read(fileno(file), a, statbuf.st_size) != statbuf.st_size)
  {
    fprintf(stderr, "Failed to read() the entire file!\n");
    exit(1);
  }

  /* Give it 10 seconds to run before killing the process with SIGALRM */
  alarm(10);
  asm("jmp *%0\n" : :"r"(a));
}
