#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define cl 50
#define br 200

typedef struct op{
  int get;
  int put;
}oper;

typedef struct act{
  int from;
  int to;
  char module[20];
  char op[5];
  float exec;
  float time;
}action;

void head(FILE* f)
{
  fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(f, "<svg style=\"background: white\" version = \"1.1\"\n");
  fprintf(f, "baseProfile=\"full\"\n");
  fprintf(f, "xmlns = \"http://www.w3.org/2000/svg\" \n");
  fprintf(f, "xmlns:xlink = \"http://www.w3.org/1999/xlink\" \n");
  fprintf(f, "xmlns:ev = \"http://www.w3.org/2001/xml-events\" \n");
}

char* get_name(char* str)
{
  char* module;
  int pos;
  module = strchr(str, '/');
  if(module == NULL)
    return str;
  else
    module = strrchr(str, '/');
  return module + 1;
}

int main(int argc, char** argv)
{
  int numlocs, i, j, numgets = 0, rect_width,
      numputs = 0, col = 0, m_size = 0, found = 0;
  char info[100], to[100], comm_op[100],
       buf[100], **modules = NULL, **m_temp;
  float end_time, ops_time = 0.0;
  action *ac, *max_op;
  ac = (action *)malloc(sizeof(action));
  max_op = (action *)malloc(sizeof(action));
  FILE *f, *graph, *nl, *matrix, *timeline;
  oper **ops;
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

  head(timeline);
  fprintf(timeline, "height = \"%dpx\" width = \"765px\">\n", cl * (numlocs + 1) + 80);

  while(1){
    fscanf(f, "%s", info);
    if(feof(f))
      break;

    ac->from = atoi(info);
    fscanf(f, "%s", to);
    ac->to = atoi(to);
    fscanf(f, "%s", comm_op);
    fscanf(f, "%s", buf);
    ac->exec = atof(buf);
    ops_time += ac->exec;
    fscanf(f, "%s", buf);
    ac->time = atof(buf);
    fscanf(f, "%s", buf);
    strcpy(buf, get_name(buf));
    strcpy(ac->module, buf);

    if(ac->exec > max_op->exec){
      max_op->exec = ac->exec;
      max_op->time = ac->time;
      strcpy(max_op->module, buf);
      strcpy(max_op->op, comm_op);
      max_op->from = ac->from;
      max_op->to = ac->to;
    }

    if(modules == NULL){
      modules = (char **)malloc(sizeof(char*));
      modules[0] = (char *)malloc(sizeof(buf));
      strcpy(modules[0],buf);
      m_size++;
    }
    else{
      for(i = 0; i < m_size; ++i){
        if(!strcmp(modules[i], buf)){
          found = 1;
          break;
        }
      }
      if(found == 0){
        m_temp = (char **)realloc(modules, sizeof(modules) + sizeof(buf));
        modules = m_temp;
        modules[m_size] = (char *)malloc(sizeof(buf));
        strcpy(modules[m_size], buf);
        m_size++;
      }
    }
    found = 0;
    rect_width = ((ac->exec / end_time) * 750 < 1?
                 (int)((ac->exec / end_time) * 750) + 2:
                 (int)((ac->exec / end_time) * 750));
    // Drawing timeline elements
    if(!strcmp(comm_op, "get")){
      ops[ atoi(info) ][ atoi(to) ].get++;
      numgets++;
      fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"fill:rgb(%d, %d, %d)\"/>\n",
                         (int)((750 * ac->time) / end_time), cl * (ac->from + 1) - 20,
                         rect_width,
                         ac->module[0],
                         ac->module[strlen(ac->module) - 6],
                         ac->module[(strlen(ac->module) - 1) / 2]);

      fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                        style=\"stroke:rgb(180, 180, 180);\"/>\n",
                        (int)((750 * ac->time) / end_time) + rect_width / 2,
                        (ac->from < ac->to ? cl * (ac->from + 1): cl * (ac->from + 1) - 20 ),
                        (int)((750 * ac->time) / end_time) + rect_width / 2,
                        cl * (ac->to + 1));
    }
    else{
      ops[ atoi(info) ][ atoi(to) ].put++;
      numputs++;
      fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"fill:rgb(%d, %d, %d)\"/>\n",
                         (int)((750 * ac->time) / end_time), cl * (ac->from + 1),
                         rect_width,
                         ac->module[0],
                         ac->module[strlen(ac->module) - 6],
                         ac->module[(strlen(ac->module) - 1) / 2]);

      fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                        style=\"stroke:rgb(180, 180, 180);\"/>\n",
                        (int)((750 * ac->time) / end_time) + rect_width / 2,
                        (ac->from > ac->to? cl * (ac->from + 1):  cl * (ac->from + 1) + 20),
                        (int)((750 * ac->time) / end_time) + rect_width / 2,
                        cl * (ac->to + 1));
    }
  }
  rect_width = ((max_op->exec / end_time) * 750 < 1?
               (int)((max_op->exec / end_time) * 750) + 2:
               (int)((max_op->exec / end_time) * 750));

  if(!strcmp(max_op->op, "get")){
    fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:red;stroke-width:1\"/>\n",
                      (int)((750 * max_op->time) / end_time) + rect_width / 2,
                      (max_op->from < max_op->to ? cl * (max_op->from + 1):
                      cl * (max_op->from + 1) - 20 ),
                      (int)((750 * max_op->time) / end_time) + rect_width / 2,
                      cl * (max_op->to + 1));
  }
  else{
    fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:red;stroke-width:1\"/>\n",
                      (int)((750 * max_op->time) / end_time) + rect_width / 2,
                      (max_op->from > max_op->to? cl * (max_op->from + 1):
                      cl * (max_op->from + 1) + 20),
                      (int)((750 * max_op->time) / end_time) + rect_width / 2,
                      cl * (max_op->to + 1));
  }

  fclose(f);

  head(matrix);
  fprintf(matrix, "height = \"%dpx\" width = \"%dpx\">",
                  cl * (numlocs + 1) - 15, cl * numlocs + 40);

  for(i = 0; i < numlocs; ++i){
    // Senders tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\">%d</text>\n",
                    (i + 1) * cl, cl / 2, i);
    // Recievers tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    transform = \"rotate(270 %d, %d)\" \
                    fill = \"black\" font-family = \"arial\">%d</text>\n",
                    cl / 2, cl * (i + 1) + cl / 4,
                    cl / 2, cl * (i + 1) + cl / 4, i);

    // locale lines
    fprintf(timeline, "<line x1=\"%dpx\" y1 = \"%dpx\" \
                      x2 = \"%dpx\" y2 = \"%dpx\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      50, cl * (i + 1), 750, cl * (i + 1));

    // locale lines tags
    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"16px\"> %d </text>\n",
                    6, cl * (i + 1) + 6, i);
    // operation tags
    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"14px\"> get </text>\n",
                    30, cl * (i + 1) - 2);

    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"14px\"> put </text>\n",
                    30, cl * (i + 1) + 11);


    for(j = 0; j < numlocs; ++j){
      if(ops[i][j].get != 0)
        fprintf(graph, "  loc%d -> loc%d[ label = \"get,%d\" ];\n", i, j, ops[i][j].get);
      if(ops[i][j].put != 0)
        fprintf(graph, "  loc%d -> loc%d[ label = \"put,%d\" ];\n", i, j, ops[i][j].put);

      if((ops[i][j].get && ops[i][j].put) != 1) // CHECK later !!
        // 0 put and 0 get fields
        fprintf(matrix, "<rect x = \"%d\" y = \"%d\" \
                         width = \"%d\" height = \"%d\" \
                         style = \"fill:grey\"/>\n",
                         cl / 2 + cl * i + 5, cl / 2 + cl * j + 5, cl , cl);
      else{
        // Get ops field
        col = br - ((float)ops[i][j].get / numgets) * br;
        fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                        style = \"fill:rgb(%d,255,%d); \
                        stroke:black;stroke-width:1\"/>\n",
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
  // Timeline annotation
  fprintf(timeline, "<text x = \"15\" y = \"39\" \
                   transform = \"rotate(270 15, 39)\" \
                   fill = \"black\" style=\"font-family:arial\" \
                   font-size=\"12\">locale</text>\n",
                   cl * (i + 1) + cl / 3, i);

  fprintf(timeline, "<text x = \"42\" y = \"39\" \
                   transform = \"rotate(270 42, 39)\" \
                   fill = \"black\" style=\"font-family:arial\" \
                   font-size=\"12\">oper</text>\n",
                   cl * (i + 1) + cl / 3, i);

  fprintf(timeline, "<text x = \"250\" y = \"20\" \
                   fill = \"black\" font-size=\"12\" \
                   style=\"font-family:arial\" \
                   >Program execution(exchanges between locales are colored)</text>\n",
                   cl * (i + 1) + cl / 3, i);

  for(i = 0; i < m_size; ++i){
    fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" height = \"%d\" \
                    style = \"fill:rgb(%d, %d, %d)\"/>\n",
                    40, cl * numlocs + 40 + 30 * i, 20, 20,
                    modules[i][0],
                    modules[i][strlen(modules[i]) - 6],
                    modules[i][(strlen(modules[i]) - 1) / 2]);

    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                      style = \"fill:black;font-family:arial\"> %s </text>\n",
                      70, cl * numlocs + 55 + 30 * i , modules[i]);

  }
  // program execution info
  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    Program was running for %.3f s; </text>\n",
                    250, cl * numlocs + 55 , end_time);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    comm ops were running for %.3f s (%.2f%% of time); </text>\n",
                    250, cl * numlocs + 79 , ops_time, (ops_time / end_time) * 100);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    the longest op was initiated by %s module;</text>\n",
                    250, cl * numlocs + 103, max_op->module);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    it was running for %f s (%f%% of time).</text>\n",
                    250, cl * numlocs + 127,
                    max_op->exec, (max_op->exec / end_time) * 100);
  // Matrix annotation
  fprintf(matrix, "<text x = \"%d\" y = \"11\" \
                  fill = \"black\" font-family = \"arial\"> \
                  Locales-senders </text>\n", (cl * numlocs) / 2 - 30, i);

  fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                  transform = \"rotate(270 %d, %d)\" \
                  style=\"writing-mode: tb\" \
                  fill = \"black\" font-family = \"arial\" \
                  >Locales-receivers</text>\n",
                  cl / 3 - 5, (cl * numlocs) / 2 + 80,
                  cl / 3 - 5, (cl * numlocs) / 2 + 80, i);

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
  fclose(timeline);
  fprintf(matrix, "</svg>");
  fclose(matrix);
  fprintf(graph, "}\n");
  fclose(graph);

  for(i = 0; i < numlocs; ++i)
    free(ops[i]);
  free(ops);

  for(i = 0; i < m_size; ++i)
    free(modules[i]);
  free(modules);

  free(ac);
  free(max_op);
  return 0;
}
