#include <stdio.h>
#include <stdlib.h>

#if defined(HAS_STDBOOL) && !defined(bool)
#include <stdbool.h>
#else
#include "odbc_cli_c_stdbool.h"
#endif /* defined(HAS_STDBOOL) && !defined(bool) */

#include <c89stringutils_string_extras.h>
#include <errno.h>

#include "odbc_cli_c.h"
#include "odbc_cli_c_config.h"
#include "odbc_runner.h"

int main(int argc, char *argv[]) {
  int exit_code = EXIT_SUCCESS;
  struct DocoptArgs *args = malloc(sizeof *args);
  if (args == NULL) {
    fputs("Out of memory\n", stderr);
    return ENOMEM;
  } else {
    exit_code = docopt(args, argc, argv, /* help */ true,
                       /* version */ ODBC_CLI_C_VERSION);
    if (exit_code != EXIT_SUCCESS) {
      free(args);
      return exit_code;
    }
  }

  // assert(args != NULL);
  run_tests(args->DSN);

  if (args->command != NULL)
    printf("args->command: \"%s\"", args->command);

cleanup:
  free(args);

  return exit_code;
}
