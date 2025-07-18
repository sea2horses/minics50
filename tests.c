#include <stdio.h>
#include <stdlib.h>
#include "minics50.h"

int main()
{
  string name = get_string("Tell me your name: ");
  string res = sconcat("Hola, ", "Mundo!");

  printf("Tu nombre es %s!\n", name);
  printf("%s\n", res);
  return 0;
}
