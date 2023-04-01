#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bread.h"
#include "config.h"
#include "log.h"

int main(int argc, char *argv[])
{
  log_enter_context("main");
  setlocale(LC_ALL, "");

  log_debug("creating config");
  struct config conf = {
    .font_size = 24
  };

  struct bread bread = bread_create(&conf);

  log_debug("finished execution");
  return 0;
  log_leave_context();
}
