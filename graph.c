
#include "stdio.h"

int main(int argc, char** argv)
{
  int count = 0;
  char info[100];
  FILE *f, *graph;
  f = fopen(argv[1], "r");
  graph = fopen("results/graph.gv", "w");
  fprintf(graph, "digraph gr{\n");
  fprintf(graph, "  node [height = 1, width = 1.4];\n");
  while(1){
    count++;
    fscanf(f, "%s", info);

    if(feof(f))
      break;
    if(count % 10 == 1)
      fprintf(graph, "  loc%s -> ", info);
    if(count % 10 == 2)
      fprintf(graph, "loc%s ", info);
    if(count % 10 == 3)
      fprintf(graph, "[ label = \"%s\" ];\n", info);
  }
  fclose(f);
  fprintf(graph, "}\n");
  fclose(graph);
  return 0;
}
