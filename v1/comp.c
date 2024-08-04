#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif /* ifndef EXIT_SUCCESS */

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif /* ifndef EXIT_FAILURE */

int main(int argc, char *argv[]) {
  if (2 != argc) {
    printf("Improper amount of arguments\n");
    return EXIT_FAILURE;
  }

  const char comp[] = "gcc -Wall -Werror -Wpedantic -o p menu.c ";
  char command[sizeof(comp) + strlen(argv[1])];
  __builtin_memcpy(command, comp, sizeof(comp) - 1);
  __builtin_memcpy(command + sizeof(comp) - 1, argv[1],
                   sizeof(command) - sizeof(comp));

  command[sizeof(command) - 1] = '\0';
  system(command);

  return EXIT_SUCCESS;
}
