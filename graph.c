#include "stdio.h"
#include "stdlib.h"

struct op{
  int get;
  int put;
}oper;

int main(int argc, char** argv)
{
  int count = 0, numlocs, i, j;
  char info[100], to[100], comm_op[100];
  FILE *f, *graph, *nl;
  struct op **ops;

  f = fopen(argv[1], "r");
  graph = fopen("results/graph.gv", "w");
  nl = fopen("results/numloc", "r");
  fscanf(nl, "%s", info);
  fclose(nl);
  numlocs = atoi(info);

  ops = (struct op **)malloc(numlocs * sizeof(ops));
  for(i = 0; i < numlocs; ++i)
    ops[i] = (struct op *)malloc(numlocs * sizeof(ops));

  for(i = 0; i < numlocs; ++i)
    for(j = 0; j < numlocs; ++j){
      ops[i][j].put = 0;
      ops[i][j].get = 0;
    }

  fprintf(graph, "digraph gr{\n");
  fprintf(graph, "  node [height = 1, width = 1.4];\n");
  while(1){
    count++;
    fscanf(f, "%s", info);

    if(feof(f))
      break;
    if(count % 10 == 1){
      fprintf(graph, "  loc%s -> ", info); count++;
      fscanf(f, "%s", to);
      fprintf(graph, "loc%s ", to); count++;
      fscanf(f, "%s", comm_op);
      fprintf(graph, "[ label = \"%s\" ];\n", comm_op);
      if(!strcmp(comm_op, "get"))
        ops[ atoi(to) ][ atoi(info) ].get++;
      else
        ops[ atoi(to) ][ atoi(info) ].put++;
    }
  }
  fclose(f);
  fprintf(graph, "}\n");
  fclose(graph);

  for(i = 0; i < numlocs; ++i){
    for(j = 0; j < numlocs; ++j)
      printf("%d,%d ", ops[i][j].get, ops[i][j].put);
      printf("\n");
  }

  for(i = 0; i < numlocs; ++i)
    free(ops[i]);
  free(ops);
  return 0;
}
