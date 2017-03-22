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
  register int eax asm ("eax");
  int eax_bak;
  int i;

  alarm(10);

  disable_buffering(stdout);
  disable_buffering(stderr);

  printf("Send me stuff!! Your goal: cleanly return with eax set to 0x00010203!\n");
  printf("Restriction: you aren't allowed to have any NUL (\\x00) bytes!\n");
  len = read(0, buffer, LENGTH);

  if(len < 0) {
    printf("Error reading!\n");
    exit(1);
  }
  for(i = 0; i < len; i++) {
    if(buffer[i] == '\0') {
      printf("No NUL bytes!!\n");
      return 0;
    }
  }

  asm("call *%0\n" : :"r"(buffer));
  eax_bak = eax;

  printf("After returning, eax => 0x%08x\n", eax_bak);
  if(eax_bak == 0x00010203)
    printf("Well done! The flag is: %s\n", FLAG);

  return 0;
}
