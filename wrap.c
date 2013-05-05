#include "stdio.h"
#include "sys/time.h"
#include "time.h"

typedef unsigned long int int32_t;
typedef const char* chpl_string;

void __real_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                       	  int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  struct timeval st, fin, tim;
  time_t rawtime;
  struct tm* info;
  FILE* f;
  f = fopen("results/chpl_comm_get", "a");
  time(&rawtime);
  gettimeofday(&st, NULL);
  __real_chpl_comm_get(addr, locale, raddr, elemSize,
		       typeIndex, len, ln, fn);
  gettimeofday(&fin, NULL);
  tim.tv_sec = fin.tv_sec - st.tv_sec;
  tim.tv_usec = fin.tv_usec - st.tv_usec;
  info = localtime(&rawtime);
  fprintf(f, "chpl_comm_get from loc %d, %ld.%ld s, start at %d:%d:%d\n",
	  locale, tim.tv_sec, tim.tv_usec,
          info->tm_hour, info->tm_min, info->tm_sec);
  fclose(f);
  return;
}

void __real_chpl_comm_put(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_put(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  struct timeval st, fin, tim;
  time_t rawtime;
  struct tm* info;
  FILE* f;
  f = fopen("results/chpl_comm_put", "a");
  time(&rawtime);
  gettimeofday(&st, NULL);
  __real_chpl_comm_put(addr, locale, raddr, elemSize,
                       typeIndex, len, ln, fn);
  gettimeofday(&fin, NULL);
  tim.tv_sec = fin.tv_sec - st.tv_sec;
  tim.tv_usec = fin.tv_usec - st.tv_usec;
  info = localtime(&rawtime);
  fprintf(f, "chpl_comm_put to loc %d, %ld.%ld s, start at %d:%d:%d\n",
          locale, tim.tv_sec, tim.tv_usec,
          info->tm_hour, info->tm_min, info->tm_sec);

  fclose(f);
  return;
}

