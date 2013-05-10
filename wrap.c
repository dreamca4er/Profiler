#include "stdio.h"
#include "sys/time.h"
#include "time.h"

typedef unsigned long int int32_t;
typedef const char* chpl_string;

struct timeval prog_start, prog_end;
extern int chpl_localeID, chpl_numLocales;

void __real_chpl_comm_init(int *argc_p, char ***argv_p);

void __wrap_chpl_comm_init(int *argc_p, char ***argv_p)
{
  gettimeofday(&prog_start, NULL);
  __real_chpl_comm_init(argc_p, argv_p);
  FILE *f;
  f = fopen("results/info", "w");
  fprintf(f, "%d\n", chpl_numLocales);
  fclose(f);
}

void __real_chpl_exit_all(int status);

void __wrap_chpl_exit_all(int status)
{
  FILE *t;
  t = fopen("results/info", "a");
  gettimeofday(&prog_end, NULL);
  if(chpl_localeID == 0)
    fprintf(t, "%.3f\n", prog_end.tv_sec - prog_start.tv_sec +
           (prog_end.tv_usec - prog_start.tv_usec) * 0.000001);
  fclose(t);
  __real_chpl_exit_all(status);
}

/*
int __real_main(int argc, char* argv[]);

void __wrap_main(int argc, char* argv[])
{
  gettimeofday(&main_start, NULL);
  __real_main(argc, argv);
}
*/
void __real_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                     	  int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  struct timeval st, fin;
  double time_from_st, op_time;
  FILE *f;
  f = fopen("results/chpl_comm_get", "a");
  gettimeofday(&st, NULL);
  __real_chpl_comm_get(addr, locale, raddr, elemSize,
		       typeIndex, len, ln, fn);
  gettimeofday(&fin, NULL);
  op_time = fin.tv_sec - st.tv_sec;
  op_time += (fin.tv_usec - st.tv_usec) * 0.000001;
  time_from_st = st.tv_sec - prog_start.tv_sec;
  time_from_st += (st.tv_usec - prog_start.tv_usec) * 0.000001;
  fprintf(f, "%d %d get %lf s, %.3lf s from init, %s\n",
	  chpl_localeID, locale, op_time, time_from_st, fn);
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
  struct timeval st, fin;
  double time_from_st, op_time;
  FILE* f;
  f = fopen("results/chpl_comm_put", "a");
  gettimeofday(&st, NULL);
  __real_chpl_comm_put(addr, locale, raddr, elemSize,
                       typeIndex, len, ln, fn);
  gettimeofday(&fin, NULL);
  op_time = fin.tv_sec - st.tv_sec;
  op_time += (fin.tv_usec - st.tv_usec) * 0.000001;
  time_from_st = st.tv_sec - prog_start.tv_sec;
  time_from_st += (st.tv_usec - prog_start.tv_usec) * 0.000001;
  fprintf(f, "%d %d put %lf s, %.3lf s from init, %s\n",
          chpl_localeID, locale, op_time, time_from_st, fn);

  fclose(f);
  return;
}

