#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define CL 50
#define BR 180

typedef struct{
  int get;
  int put;
  char show;
}oper;

typedef struct{
  int from;
  int to;
  char module[20];
  char op[5];
  float exec;
  float time;
}action;

int comp (const void * elem1, const void * elem2) {
  int f = *((int*)elem1);
  int s = *((int*)elem2);
  return (f > s) - (f < s);
}

void head(FILE* f)
{
  fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(f, "<svg style=\"background: white\" version = \"1.1\"\n");
  fprintf(f, "baseProfile=\"full\"\n");
  fprintf(f, "xmlns = \"http://www.w3.org/2000/svg\" \n");
  fprintf(f, "xmlns:xlink = \"http://www.w3.org/1999/xlink\" \n");
  fprintf(f, "xmlns:ev = \"http://www.w3.org/2001/xml-events\" \n");
}

void create_color(char* name, char* color)
{
  int red, green, blue;
  char result[25];
  red = ((name[5 % strlen(name)] + name[8 % strlen(name)] +
         name[9 % strlen(name)]) * strlen(name)) % 255;
  green = ((name[(2 + strlen(name) / 2) % strlen(name)]) * strlen(name) * 3) % 255;
  blue = (name[(strlen(name) - 9) % strlen(name)] * strlen(name)) % 255;
  sprintf(result, "rgb(%d, %d, %d)", red, green, blue);
  strcpy(color, result);
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
  int numlocs, numgets = 0, rect_width, numputs = 0, pos,
      w_found = 0, i, j, col = 0, us_loc_count = 0,
      m_size = 0, found = 0, WIDTH = 1300, *us_loc, k = 0,
      transfered_size = 0;
  char info[100], to[100], comm_op[100], color[25], prog[100],
       buf[100], **modules = NULL, **m_temp, tmp[10];
  float end_time, ops_time = 0.0;

  FILE *f, *graph, *nl, *matrix, *timeline, *collect_info;
  fpos_t position_height, position_last;
  f = fopen("results/m", "r");
  graph = fopen("results/graph.gv", "w");
  nl = fopen("results/info", "r");
  matrix = fopen("results/matrix.html", "w");
  timeline = fopen("results/timeline.html", "w");
  collect_info = fopen("results/gathered_info", "w");

  action *ac, *max_op;
  ac = (action *)malloc(sizeof(action));
  max_op = (action *)malloc(sizeof(action));
  oper **ops;

  fscanf(nl, "%s", prog);
  fscanf(nl, "%s", info);
  numlocs = atoi(info);
  fscanf(nl, "%s", info);
  end_time = atof(info);
  fclose(nl);

  us_loc = (int *)malloc(numlocs * sizeof(int));
  ops = (oper **)malloc(numlocs * sizeof(oper *));

  for(i = 0; i < numlocs; ++i){
    us_loc[i] = numlocs + 1;
    ops[i] = (oper *)malloc(numlocs * sizeof(oper));

    for(j = 0; j < numlocs; j++){
      ops[i][j].put = 0;
      ops[i][j].get = 0;
      ops[i][j].show = 0;
    }
  }

  //parsing command line options
  if(argc > 1){
    for(i = 1; i < argc; ++i){
      if(!strcmp(argv[i], "-w")){
        WIDTH = atoi(argv[i + 1]);
      }
      if(!strcmp(argv[i], "-l")){
        while(i < argc - 1){
          if(!strcmp(argv[i + 1], "-w")){
            w_found = 1;
            break;
          }
          us_loc[k] = atoi(argv[i + 1]);
          us_loc_count++;
          k++; i++;
        }
        if(w_found == 1){
          WIDTH = atoi(argv[i + 1]);
        }
      }
    }
  }
  qsort(us_loc, numlocs, sizeof(int), comp);

  if(us_loc_count > 0){
    for(i = 0; i < numlocs; ++i)
      for(j = 0; j < numlocs; ++j)
        if((us_loc[j] < numlocs) && (us_loc[i] < numlocs))
          ops[us_loc[i]][us_loc[j]].show = 1;
  }
  else{
    for(i = 0; i < numlocs; ++i)
      for(j = 0; j < numlocs; ++j)
        ops[i][j].show = 1;
  }

  fprintf(graph, "digraph gr{\n");
  fprintf(graph, "  node [height = 1, width = 1, fontsize=28];\n");
  head(timeline);
  pos = ftell(timeline);
  fprintf(timeline, "                                                      ");
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
    fscanf(f, "%s", tmp);
    transfered_size += atoi(tmp);

    if(ac->exec > max_op->exec){
      max_op->exec = ac->exec;
      max_op->time = ac->time;
      strcpy(max_op->module, buf);
      strcpy(max_op->op, comm_op);
      max_op->from = ac->from;
      max_op->to = ac->to;
    }
    // Creating array of modules names
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
    rect_width = ((ac->exec / end_time) * (WIDTH - 55) < 1?
                 (int)((ac->exec / end_time) * (WIDTH - 55)) + 2:
                 (int)((ac->exec / end_time) * (WIDTH - 55)));
    create_color(ac->module, color);
    // Drawing timeline elements
    if(!strcmp(comm_op, "get") && ops[atoi(info)][atoi(to)].show == 1){
      ops[ atoi(info) ][ atoi(to) ].get++;
      numgets++;
      fprintf(timeline, "\n<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"fill:%s\"/>",
                         (int)(((WIDTH - 55) * ac->time) / end_time) + 50,
                         CL * (ac->from + 1) - 20, rect_width, color);

      fprintf(timeline, "\n<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                        style=\"stroke:rgb(180, 180, 180);\"/>",
                        (int)(((WIDTH - 55) * ac->time) / end_time) + rect_width / 2 + 50,
                        (ac->from < ac->to ? CL * (ac->from + 1): CL * (ac->from + 1) - 20 ),
                        (int)(((WIDTH - 55) * ac->time) / end_time) + rect_width / 2 + 50,
                        CL * (ac->to + 1));
    }
    if(!strcmp(comm_op, "put") && ops[atoi(info)][atoi(to)].show == 1){
      ops[ atoi(info) ][ atoi(to) ].put++;
      numputs++;
      fprintf(timeline, "\n<rect x = \"%d\" y = \"%d\" width = \"%d\" \
                         height = \"20\" style = \"fill:%s\"/>",
                         (int)(((WIDTH - 55) * ac->time) / end_time) + 50,
                         CL * (ac->from + 1), rect_width, color);

      fprintf(timeline, "\n<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                        style=\"stroke:rgb(180, 180, 180);\"/>",
                        (int)(((WIDTH - 55) * ac->time) / end_time) + rect_width / 2 + 50,
                        (ac->from > ac->to? CL * (ac->from + 1):  CL * (ac->from + 1) + 20),
                        (int)(((WIDTH - 55) * ac->time) / end_time) + rect_width / 2 + 50,
                        CL * (ac->to + 1));
    }
  }
  rect_width = ((max_op->exec / end_time) * (WIDTH - 55) < 1?
               (int)((max_op->exec / end_time) * (WIDTH - 55)) + 2:
               (int)((max_op->exec / end_time) * (WIDTH - 55)));

  //longest operation
  if(!strcmp(max_op->op, "get") && (ops[max_op->from][max_op->to].show == 1)){
    fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:red;stroke-width:1\"/>\n",
                      (int)(((WIDTH - 55) * max_op->time) / end_time) + rect_width / 2 + 50,
                      (max_op->from < max_op->to ? CL * (max_op->from + 1):
                      CL * (max_op->from + 1) - 20 ),
                      (int)(((WIDTH - 55) * max_op->time) / end_time) + rect_width / 2 + 50,
                      CL * (max_op->to + 1));
  }
  if(!strcmp(max_op->op, "put") && (ops[max_op->from][max_op->to].show == 1)){
    fprintf(timeline, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:red;stroke-width:1\"/>\n",
                      (int)(((WIDTH - 55) * max_op->time) / end_time) + rect_width / 2 + 50,
                      (max_op->from > max_op->to? CL * (max_op->from + 1):
                      CL * (max_op->from + 1) + 20),
                      (int)(((WIDTH - 55) * max_op->time) / end_time) + rect_width / 2 + 50,
                      CL * (max_op->to + 1));
  }

  fclose(f);

  head(matrix);
  fprintf(matrix, "height = \"%dpx\" width = \"%dpx\">",
                  CL * (numlocs + 1) - 15, CL * numlocs + 40);

  for(i = 0; i < numlocs; ++i){
    // Senders tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\">%d</text>\n",
                    (i + 1) * CL, CL / 2, i);
    // Recievers tags
    fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                    transform = \"rotate(270 %d, %d)\" \
                    fill = \"black\" font-family = \"arial\">%d</text>\n",
                    CL / 2, CL * (i + 1) + CL / 4,
                    CL / 2, CL * (i + 1) + CL / 4, i);

    // locale lines
    fprintf(timeline, "<line x1=\"%dpx\" y1 = \"%dpx\" \
                      x2 = \"%dpx\" y2 = \"%dpx\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      50, CL * (i + 1), (WIDTH - 5), CL * (i + 1));

    // locale lines tags
    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"16px\"> %d </text>\n",
                    6, CL * (i + 1) + 6, i);
    // operation tags
    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"14px\"> get </text>\n",
                    30, CL * (i + 1) - 2);

    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    fill = \"black\" font-family = \"arial\" \
                    font-size = \"14px\"> put </text>\n",
                    30, CL * (i + 1) + 11);


    for(j = 0; j < numlocs; ++j){

      if(((ops[i][j].get == 0) && (ops[i][j].put == 0))|| ops[i][j].show == 0)
        // 0 put and 0 get fields
        fprintf(matrix, "<rect x = \"%d\" y = \"%d\" \
                         width = \"%d\" height = \"%d\" \
                         style = \"fill:grey\"/>\n",
                         CL / 2 + CL * i + 5, CL / 2 + CL * j + 5, CL , CL);
      else{
        // Get ops field
        col = BR - ((float)ops[i][j].get / numgets) * BR;
        fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                        style = \"fill:rgb(%d,255,%d); \
                        stroke:black;stroke-width:1\"/>\n",
                        CL / 2 + CL * i + 5, CL / 2 + CL * j + 5,
                        CL / 2 + CL * i + 5, CL / 2 + CL * (j + 1) + 5,
                        CL / 2 + CL * (i + 1) + 5, CL / 2 + CL * j + 5,
                        col, col);
        // Get ops text
        fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                        style=\"fill:black;font-family:arial\"> %d </text>\n",
                        CL / 2 + CL * i + CL / 4 ,
                        CL / 2 + CL * j + CL / 4 + CL / 6, ops[i][j].get);
        // Put ops field
        col = BR - ((float)ops[i][j].put / numputs) * BR;
	fprintf(matrix, "<polygon points=\"%d,%d %d,%d, %d,%d\" \
                       	style = \"fill:rgb(255,%d,%d);stroke:black;stroke-width:1\"/>\n",
                        CL / 2 + CL * (i + 1) + 5, CL / 2 + CL * (j + 1) + 5,
                        CL / 2 + CL * i + 5, CL / 2 + CL * (j + 1) + 5,
                        CL / 2 + CL * (i + 1) + 5, CL / 2 + CL * j + 5,
                        col, col);
        // Put ops text
        fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                        style = \"fill:black;font-family:arial\"> %d </text>\n",
                        CL / 2 + CL * i + CL / 2 + CL / 5,
                       	CL + CL * j + CL / 4 + CL / 6, ops[i][j].put);
      }
      // Graph generation
      if(ops[i][j].show == 1){
        if((ops[i][j].get == 0) &&
          (ops[i][j].put == 0))
          fprintf(graph, "  loc%d;\n  loc%d;", i, j);

        if(ops[i][j].get != 0)
           fprintf(graph, "  loc%d -> loc%d[ label = \"get,%d\", \
                       fontsize = 20];\n", i, j, ops[i][j].get);

        if(ops[i][j].put != 0)
          fprintf(graph, "  loc%d -> loc%d[ label = \"put,%d\", \
                       fontsize = 20];\n",i, j, ops[i][j].put);
      }
    }
  }
  // Timeline annotation
  fprintf(timeline, "<text x = \"15\" y = \"39\" \
                   transform = \"rotate(270 15, 39)\" \
                   fill = \"black\" style=\"font-family:arial\" \
                   font-size=\"12\">locale</text>\n",
                   CL * (i + 1) + CL / 3, i);

  fprintf(timeline, "<text x = \"42\" y = \"39\" \
                   transform = \"rotate(270 42, 39)\" \
                   fill = \"black\" style=\"font-family:arial\" \
                   font-size=\"12\">oper</text>\n",
                   CL * (i + 1) + CL / 3, i);

  fprintf(timeline, "<text x = \"250\" y = \"20\" \
                   fill = \"black\" font-size=\"12\" \
                   style=\"font-family:arial\" \
                   >%s execution(exchanges between locales are colored)</text>\n",
                   prog, CL * (i + 1) + CL / 3, i);

  for(i = 0; i < m_size; ++i){
    create_color(modules[i], color);
    fprintf(timeline, "<rect x = \"%d\" y = \"%d\" width = \"%d\" height = \"%d\" \
                      style = \"fill:%s\"/>\n",
                      40, CL * numlocs + 40 + 25 * i, 20, 20, color);

    fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                      style = \"fill:black;font-family:arial\"> %s </text>\n",
                      70, CL * numlocs + 55 + 25 * i , modules[i]);

  }
  // program execution info
  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    Program was running for %.3f s; </text>\n",
                    250, CL * numlocs + 55 , end_time);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    comm ops were running for %.3f s (%.2f%% of time); </text>\n",
                    250, CL * numlocs + 79 , ops_time, (ops_time / end_time) * 100);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    the longest op was initiated by %s module;</text>\n",
                    250, CL * numlocs + 103, max_op->module);

  fprintf(timeline, "<text x = \"%d\" y = \"%d\" \
                    style = \"fill:black;font-family:arial\"> \
                    it was running for %f s (%f%% of time).</text>\n",
                    250, CL * numlocs + 127,
                    max_op->exec, (max_op->exec / end_time) * 100);
  // Matrix annotation
  fprintf(matrix, "<text x = \"%d\" y = \"11\" \
                  fill = \"black\" font-family = \"arial\"> \
                  Locales-senders at %s</text>\n",
                 (CL * numlocs) / 2 - 30, prog);

  fprintf(matrix, "<text x = \"%d\" y = \"%d\" \
                  transform = \"rotate(270 %d, %d)\" \
                  fill = \"black\" font-family = \"arial\" \
                  >Locales-receivers at %s</text>\n",
                  CL / 3 - 5, (CL * numlocs) / 2 + 80,
                  CL / 3 - 5, (CL * numlocs) / 2 + 80, prog);

  // Net
  for(i = 0; i < numlocs + 1; ++i)
    for(j = 0; j < numlocs + 1; ++j){
      fprintf(matrix, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      CL / 2 + 5, CL / 2 + CL * i + 5,
                      CL / 2 + 5 + CL * numlocs, CL / 2+ CL* i + 5);
      fprintf(matrix, "<line x1=\"%d\" y1 = \"%d\" x2 = \"%d\" y2 = \"%d\" \
                      style=\"stroke:black;stroke-width:1\"/>\n",
                      CL / 2 + CL * i + 5, CL / 2 + 5,
                      CL / 2+ CL* i + 5 ,CL / 2 + 5 + CL * numlocs);
    }

  // Info file generation
  fprintf(collect_info, "Program %s was executed on %d locales; \
                         \r%d gets and %d puts were initiated;\
                         \r%d bytes of data were transfered;\
                         \rprogram was running for %.3f seconds;\
                         \rexchanges were running for %.3f seconds (%.2f%% of time);\
                         \rthe longest exchange was initiated by %s module;\
                         \rit was running for %f seconds (%f%% of time).",
                         prog, numlocs, numgets, numputs, transfered_size, end_time,
                         ops_time, (ops_time / end_time) * 100, max_op->module,
                         max_op->exec, (max_op->exec / end_time) * 100);
  fclose(collect_info);
  fseek(timeline, pos, SEEK_SET);
  fprintf(timeline, "height = \"%dpx\" width = \"%dpx\">",
                    CL * numlocs + 40 + 25 * m_size, WIDTH);
  fseek(timeline, 0, SEEK_END);
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
