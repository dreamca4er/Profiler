#include "stdio.h"

typedef unsigned long int int32_t;
typedef const char* chpl_string;

void __real_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                       	  int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  FILE* f;
  f = fopen("results/chpl_comm_get", "a");
  fprintf(f, "chpl_comm_get from %d\n",locale);
  __real_chpl_comm_get(addr, locale, raddr, elemSize,
		       typeIndex, len, ln, fn);
  fclose(f);
  return;

}

