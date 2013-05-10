#include "stdio.h"
#include "stdlib.h"

#define cl 60
#define br 200

typedef struct op{
  int get;
  int put;
}oper;

typedef struct act{
  int from;
  int to;
  char *op;
  float exec;
  float time;
  char *module;
}action;

int main(int argc, char** argv)
{
  int numlocs, i, j, numgets = 0,
      numputs = 0, col = 0;
  char info[100], to[100], comm_op[100], buf[100];
  float end_time, max_time = 0.0;
  FILE *f, *graph, *nl, *matrix, *timeline;
  oper **ops;
  action ac; //structure for drawing ops
  f = fopen("results/m", "r");
  graph = fopen("results/graph.gv", "w");
  nl = fopen("results/info", "r");
  matrix = fopen("results/matrix.html", "w");
  timeline = fopen("results/timeline.html", "w");
  fscanf(nl, "%s", info);
  numlocs = atoi(info);
  fscanf(nl, "%s", info);
  end_time = atof(info);
  fclose(nl);
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

  fprintf(timeline, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(timeline, "<svg style=\"background: white\" version = \"1.1\"\n");
  fprintf(timeline, "baseProfile=\"full\"\n");
  fprintf(timeline, "xmlns = \"http://www.w3.org/2000/svg\" \n");
  fprintf(timeline, "xmlns:xlink = \"http://www.w3.org/1999/xlink\" \n");
  fprintf(timeline, "xmlns:ev = \"http://www.w3.org/2001/xml-events\" \n");
  fprintf(timeline, "height = \"%dpx\" width = \"800px\">\n", cl * (numlocs + 1) + 150);

  while(1){
    fscanf(f, "%s", info);
    if(feof(f))
      break;

    ac.from = atoi(info);
    fprintf(graph, "  loc%s -> ", info);
    fscanf(f, "%s", to);
    ac.to = atoi(to);
    fprintf(graph, "loc%s ", to);
    fscanf(f, "%s", comm_op);
    ac.op = comm_op;
    fprintf(graph, "[ label = \"%s\" ];\n", comm_op);
    fscanf(f, "%s", buf);
    ac.exec = atof(buf);

    if(ac.exec > max_time)
      max_time = ac.exec;

    fscanf(f, "%s", buf);
    ac.time = atof(buf);
    fscanf(f, "%s", buf);
    ac.module = buf;
    if(!strcmp(comm_op, "get")){
      ops[ atoi(info) ][ atoi(to) ].get++;
      numgets++;
      fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"stroke-width:2;fill:black\"/>\n",
                         (int)((750 * ac.time) / end_time), cl * (ac.from + 1) - 20,
                         (int)((ac.exec / end_time) * 750) + 1);

      if(ac.from < ac.to)
        fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                          style=\"stroke:grey;stroke-width:1\"/>\n",
                          (int)((750 * ac.time) / end_time), cl * (ac.from + 1),
                          (int)((750 * ac.time) / end_time), cl * (ac.to + 1));
      else
        fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                          style=\"stroke:grey;stroke-width:1\"/>\n",
                          (int)((750 * ac.time) / end_time), cl * (ac.from + 1) - 20,
                          (int)((750 * ac.time) / end_time), cl * (ac.to + 1));


    }
    else{
      ops[ atoi(info) ][ atoi(to) ].put++;
      numputs++;
      fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"fill:black\"/>\n",
                         (int)((750 * ac.time) / end_time), cl * (ac.from + 1),
                         (int)((ac.exec / end_time) * 750) + 1);
      if(ac.from > ac.to)
        fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                          style=\"stroke:grey;stroke-width:1\"/>\n",
                          (int)((750 * ac.time) / end_time), cl * (ac.from + 1),
                          (int)((750 * ac.time) / end_time), cl * (ac.to + 1));
      else
        fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                          style=\"stroke:grey;stroke-width:1\"/>\n",
                          (int)((750 * ac.time) / end_time), cl * (ac.from + 1) + 20,
                          (int)((750 * ac.time) / end_time), cl * (ac.to + 1));

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
    // locale lines
    fprintf(timeline, "<line x1=\"%dpx\" y1 = \"%dpx\" x2 = \"%dpx\" y2 = \"%dpx\" \
                    style=\"stroke:black;stroke-width:1\"/>\n",
                    50, cl * (i + 1), 750, cl * (i + 1));
    // locale lines tags
    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\"> loc%d </text>\n",
                    20, cl * (i + 1) + 6, i);


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

  fprintf(timeline, "</svg>");
  fprintf(matrix, "</svg>");
  fclose(matrix);
  for(i = 0; i < numlocs; ++i)
    free(ops[i]);
  free(ops);
  return 0;
}
