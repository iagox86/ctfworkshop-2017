#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "flag.h"

#define LENGTH 1024
#define disable_buffering(_fd) setvbuf(_fd, NULL, _IONBF, 0)

int main(int argc, char *argv[])
{
  uint8_t *buffer = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  ssize_t len;
  int guard1 = 0x41414141;
  int target = 0;
  int guard2 = 0x42424242;

  alarm(10);

  disable_buffering(stdout);
  disable_buffering(stderr);

  printf("Send me stuff!! Your goal: change target (on the stack) to 1 without damaging guard1 or guard2\n");
  len = read(0, buffer, LENGTH);

  if(len < 0) {
    printf("Error reading!\n");
    exit(1);
  }

  asm("call *%0\n" : :"r"(buffer));

  if(guard1 != 0x41414141) {
    printf("Damaged guard1!\n");
    exit(0);
  }

  if(guard2 != 0x42424242) {
    printf("Damaged guard2!\n");
    exit(0);
  }

  if(target != 1) {
    printf("Change target to 1!\n");
    exit(0);
  }

  printf("FLAG: %s\n", FLAG);

  return 0;
}
