#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char buffer[256];

  printf("What is the password?\n");
  gets(buffer);

  if(!strcmp(buffer, "the password"))
  {
    printf("FLAG:a4d813bd601c5aa11a7f467e1e430d29\n");
  }
  else
  {
    printf("Wrong!!\n");
  }

  return 0;
}
