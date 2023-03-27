#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bread.h"
#include "config.h"

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");
  printf("Hello World\n");

  struct config conf = {
    .font_size = 24
  };

  struct bread bread = bread_create(&conf);

  printf("Finished Execution\n");
  return 0;
}
