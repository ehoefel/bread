#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include "log.h"

#define SECOND 1000000000ul

struct log_context {
  char *name;
  struct log_context *parent;
  int level;
};

static struct timespec time_diff(
    struct timespec cur,
    struct timespec old);

static struct log_context *current;

static void print_indent(FILE *file)
{
  if (current == NULL)
    return;

  for (int i = 0; i < current->level; i++) {
    fprintf(file, "  ");
  }
}

void log_enter_context(char *name)
{
  struct log_context *parent = current;
  current = (struct log_context*)malloc(sizeof(struct log_context));
  current->name = name;
  current->parent = parent;
  if (parent == NULL) {
    current->level = 0;
  } else {
    current-> level = parent->level + 1;
  }
  log_debug("enter context");
}

void log_leave_context(void)
{
  if (current == NULL)
    return;

  log_debug("leave context");
  struct log_context *parent = current->parent;
  free(current);
  current = parent;
}

void log_error(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[ERROR]: ");
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void log_warning(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[WARNING]: ");
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void log_debug(const char *const fmt, ...)
{
#ifndef DEBUG
  return;
#endif

  if (current == NULL) {
    return;
  }

  static struct timespec start_time;
  if (start_time.tv_nsec == 0) {
    clock_gettime(CLOCK_REALTIME, &start_time);
  }
  struct timespec real_time;
  struct timespec cpu_time;
  clock_gettime(CLOCK_REALTIME, &real_time);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_time);
  real_time = time_diff(real_time, start_time);

  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);

  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[DEBUG] [%25s]: ", current->name);
  print_indent(stderr);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

void log_info(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[INFO]: ");
  print_indent(stderr);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void log_append_error(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void log_append_warning(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void log_append_debug(const char *const fmt, ...)
{
#ifndef DEBUG
  return;
#endif
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void log_append_info(const char *const fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

struct timespec time_diff(struct timespec cur,
    struct timespec old)
{
  struct timespec diff;
  diff.tv_sec = cur.tv_sec - old.tv_sec;
  if (cur.tv_nsec > old.tv_nsec) {
    diff.tv_nsec = cur.tv_nsec - old.tv_nsec;
  } else {
    diff.tv_nsec = SECOND + cur.tv_nsec - old.tv_nsec;
    diff.tv_sec -= 1;
  }
  return diff;
}
