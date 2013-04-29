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
  struct timeval st, fin, time;
  FILE* f;
  f = fopen("results/chpl_comm_get", "a");
  gettimeofday(&st, NULL);
  __real_chpl_comm_get(addr, locale, raddr, elemSize,
		       typeIndex, len, ln, fn);
  gettimeofday(&fin, NULL);
  time.tv_sec = fin.tv_sec - st.tv_sec;
  time.tv_usec = fin.tv_usec - st.tv_usec;
  fprintf(f, "chpl_comm_get from %d locale, %ld.%ld s\n",locale, time.tv_sec, time.tv_usec);
  fclose(f);
  return;
}

