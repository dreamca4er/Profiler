#include "stdio.h"
#include "stdlib.h"

int main(int argc, char** argv)
{
  char info[100];
  float result = 0.0;
  int count = 0;
  FILE* f;
  f = fopen(argv[1], "r");
  while(1){
    count++;
    fscanf(f, "%s", info);

    if(count % 7 == 5)
			result += atof(info);

    if(feof(f))
			break;

  }
  fclose(f);
  f = fopen(argv[2], "w");
  fprintf(f, "%f\n", result);
  fclose(f);
  return 0;

}
