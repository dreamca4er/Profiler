#include "stdio.h"
#include "stdlib.h"

#define cl 60
#define br 200

typedef struct op{
  int get;
  int put;
}oper;

int main(int argc, char** argv)
{
  int count = 0, numlocs, i, j, numgets = 0, numputs = 0, col = 0;
  char info[100], to[100], comm_op[100];
  FILE *f, *graph, *nl, *matrix;
  oper **ops;
  f = fopen("results/m", "r");
  graph = fopen("results/graph.gv", "w");
  nl = fopen("results/numloc", "r");
  matrix = fopen("results/matrix.html", "w");
  fscanf(nl, "%s", info);
  fclose(nl);
  numlocs = atoi(info);
  ops = (oper **)malloc(numlocs * sizeof(oper *));
  for(i = 0; i < numlocs; i++)
    ops[i] = (oper *)malloc(numlocs * sizeof(oper));
  for(i = 0; i < numlocs; i++)
    for(j = 0; j < numlocs; j++){
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
      if(!strcmp(comm_op, "get")){
        ops[ atoi(info) ][ atoi(to) ].get++;
        numgets++;
      }
      else{
        ops[ atoi(info) ][ atoi(to) ].put++;
        numputs++;
        }
    }
  }

  fclose(f);
  fprintf(graph, "}\n");
  fclose(graph);
  fprintf(matrix, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(matrix, "<svg style=\"background: white\" version = \"1.1\"\n");
  fprintf(matrix, "baseProfile=\"full\"\n");
  fprintf(matrix, "xmlns = \"http://www.w3.org/2000/svg\" \n");
  fprintf(matrix, "xmlns:xlink = \"http://www.w3.org/1999/xlink\" \n");
  fprintf(matrix, "xmlns:ev = \"http://www.w3.org/2001/xml-events\" \n");
  fprintf(matrix, "height = \"%dpx\" width = \"%dpx\">", cl * (numlocs + 1), cl * numlocs * 2);
  for(i = 0; i < numlocs; ++i){
    // Senders tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\"> loc%d </text>\n",
                    cl / 2 + cl / 5 + i * cl, cl / 2, i);
    // Recievers tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    transform = \"rotate(270 %d, %d)\" \
                    fill = \"black\" font-family = \"arial\"> loc%d </text>\n",
                    cl / 2, cl * (i + 1) + cl / 3,
                    cl / 2, cl * (i + 1) + cl / 3, i);

    for(j = 0; j < numlocs; ++j){

      if((ops[i][j].get && ops[i][j].put) != 1) // CHECK later !!
        // 0 put and 0 get fields
        fprintf(matrix, "<rect x = \"%d\" y = \"%d\" width = \"%d\" height = \"%d\" \
                         style = \"fill:grey\"/>\n",
                         cl / 2 + cl * i + 5, cl / 2 + cl * j + 5, cl , cl);
      else{
        // Get ops field
        col = br - ((float)ops[i][j].get / numgets) * br;
        fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                        style = \"fill:rgb(%d,255,%d);stroke:black;stroke-width:1\"/>\n",
                        cl / 2 + cl * i + 5, cl / 2 + cl * j + 5,
                        cl / 2 + cl * i + 5, cl / 2 + cl * (j + 1) + 5,
                        cl / 2 + cl * (i + 1) + 5, cl / 2 + cl * j + 5,
                        col, col);
        // Get ops text
        fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                        style=\"fill:black;font-family:arial\"> %d </text>\n",
                        cl / 2 + cl * i + cl / 4 ,
                        cl / 2 + cl * j + cl / 4 + cl / 6, ops[i][j].get);
        // Put ops field
        col = br - ((float)ops[i][j].put / numputs) * br;
	fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                       	style = \"fill:rgb(255,%d,%d);stroke:black;stroke-width:1\"/>\n",
                        cl / 2 + cl * (i + 1) + 5, cl / 2 + cl * (j + 1) + 5,
                        cl / 2 + cl * i + 5, cl / 2 + cl * (j + 1) + 5,
                        cl / 2 + cl * (i + 1) + 5, cl / 2 + cl * j + 5,
                        col, col);
        // Put ops text
        fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                        style = \"fill:black;font-family:arial\"> %d </text>\n",
                        cl / 2 + cl * i + cl / 4 + cl / 2,
                       	cl + cl * j + cl / 4 + cl / 6, ops[i][j].put);
      }
    }
  }
  // Net
  for(i = 0; i < numlocs + 1; ++i)
    for(j = 0; j < numlocs + 1; ++j){
      fprintf(matrix, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      cl / 2 + 5, cl / 2 + cl * i + 5,
                      cl / 2 + 5 + cl * numlocs, cl / 2+ cl* i + 5);
      fprintf(matrix, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      cl / 2 + cl * i + 5, cl / 2 + 5,
                      cl / 2+ cl* i + 5 ,cl / 2 + 5 + cl * numlocs);

    }
/*
  fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                  style = \"fill:rgb(100,255,100);stroke:black;stroke-width:1\"/>\n",
                  cl + cl * numlocs + 5, cl / 2 + cl * (numlocs - (numlocs - 1)) + 5,
                  cl + cl * numlocs + 5, cl / 2 + cl * (2)+ 5,
                  cl + cl * (numlocs + 1) + 5, cl / 2 + cl * (numlocs - (numlocs - 1)) + 5,
                  col, col);

  fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                  style = \"fill:rgb(255,40,40);stroke:black;stroke-width:1\"/>\n",
                  2 * cl + cl * numlocs + 5, cl / 2 + cl * 2 + 5,
                  cl + cl * numlocs + 5, cl / 2 + cl * 2 + 5,
                  cl + cl * (numlocs + 1) + 5, cl / 2 + cl * (numlocs - (numlocs - 1)) + 5,
                  col, col);

  fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                  style=\"fill:black;font-family:arial\"> %d </text>\n",
                  cl + cl * numlocs + 5, cl / 2 + cl * (numlocs - (numlocs - 1)) + 5,
                  20);

  fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                  style = \"fill:black;font-family:arial\"> %d </text>\n",
                  cl / 2 + cl * i + cl / 4 + cl / 2,
                  cl + cl * j + cl / 4 + cl / 6, 5);
*/
  fprintf(matrix, "</svg>");
  fclose(matrix);
  for(i = 0; i < numlocs; ++i)
    free(ops[i]);
  free(ops);
  return 0;
}
