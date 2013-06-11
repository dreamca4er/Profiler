#include "stdio.h"
#include "sys/time.h"
#include "time.h"

typedef unsigned long int int32_t;
typedef const char* chpl_string;

extern int chpl_nodeID, chpl_numNodes;

struct timeval prog_end, main_start,
       st_get, fin_get, st_put, fin_put;

double time_from_st_get, op_time_get,
       time_from_st_put, op_time_put;

FILE *comm_get, *comm_put, *f;

void __real_chpl_comm_init(int *argc_p, char ***argv_p);

void __wrap_chpl_comm_init(int *argc_p, char ***argv_p)
{
  __real_chpl_comm_init(argc_p, argv_p);
  char f_name[20];
  f = fopen("results/info", "w");
  fprintf(f, "%d\n", chpl_numNodes);
  sprintf(f_name, "results/chpl_comm_get%d", chpl_nodeID);
  comm_get = fopen(f_name, "a");
  sprintf(f_name, "results/chpl_comm_put%d", chpl_nodeID);
  comm_put = fopen(f_name, "a");
}

void __real_chpl_exit_all(int status);

void __wrap_chpl_exit_all(int status)
{
  gettimeofday(&prog_end, NULL);
  if(chpl_nodeID == 0)
    fprintf(f, "%.3f\n", prog_end.tv_sec - main_start.tv_sec +
           (prog_end.tv_usec - main_start.tv_usec) * 0.000001);
  fclose(f);
  __real_chpl_exit_all(status);
}


int __real_main(int argc, char* argv[]);

void __wrap_main(int argc, char* argv[])
{
  gettimeofday(&main_start, NULL);
  __real_main(argc, argv);
}

void __real_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                     	  int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_get(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  gettimeofday(&st_get, NULL);
  __real_chpl_comm_get(addr, locale, raddr, elemSize,
		       typeIndex, len, ln, fn);
  gettimeofday(&fin_get, NULL);
  op_time_get = fin_get.tv_sec - st_get.tv_sec;
  op_time_get += (fin_get.tv_usec - st_get.tv_usec) * 0.000001;
  time_from_st_get = st_get.tv_sec - main_start.tv_sec;
  time_from_st_get += (st_get.tv_usec - main_start.tv_usec) * 0.000001;
  fprintf(comm_get, "%d %d get %lf %.3lf %s\n",
	  chpl_nodeID, locale, op_time_get, time_from_st_get, fn);
  return;
}

void __real_chpl_comm_put(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn);

void __wrap_chpl_comm_put(void *addr, int32_t locale, void* raddr,
                          int32_t elemSize, int32_t typeIndex, int32_t len,
                          int ln, chpl_string fn)
{
  gettimeofday(&st_put, NULL);
  __real_chpl_comm_put(addr, locale, raddr, elemSize,
                       typeIndex, len, ln, fn);
  gettimeofday(&fin_put, NULL);
  op_time_put = fin_put.tv_sec - st_put.tv_sec;
  op_time_put += (fin_put.tv_usec - st_put.tv_usec) * 0.000001;
  time_from_st_put = st_put.tv_sec - main_start.tv_sec;
  time_from_st_put += (st_put.tv_usec - main_start.tv_usec) * 0.000001;
  fprintf(comm_put, "%d %d put %lf %.3lf %s\n",
          chpl_nodeID, locale, op_time_put, time_from_st_put, fn);
  return;
}

