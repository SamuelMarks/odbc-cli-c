#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odbc_cli_c.h"

struct Command {
  const char *name;
  bool value;
};

struct Argument {
  const char *name;
  const char *value;
  const char *array[ARG_MAX];
};

struct Option {
  const char *oshort;
  const char *olong;
  bool argcount;
  bool value;
  const char *argument;
};

struct Elements {
  int n_commands;
  int n_arguments;
  int n_options;
  struct Command *commands;
  struct Argument *arguments;
  struct Option *options;
};

/*
 * Tokens object
 */

struct Tokens {
  int argc;
  char **argv;
  int i;
  char *current;
};

const char usage_pattern[] = "Usage:\n"
                             "  odbc-cli-c <DSN>\n"
                             "  odbc-cli-c --output=<o> <DSN>\n"
                             "  odbc-cli-c --command=<c> <DSN>\n"
                             "  odbc-cli-c --command-file=<f> <DSN>\n"
                             "  odbc-cli-c -c=<c> --params=<PARAMS> <DSN>\n"
                             "  odbc-cli-c --help\n"
                             "  odbc-cli-c --version";

struct Tokens tokens_new(int argc, char **argv) {
  struct Tokens ts;
  ts.argc = argc;
  ts.argv = argv;
  ts.i = 0;
  ts.current = argv[0];
  return ts;
}

struct Tokens *tokens_move(struct Tokens *ts) {
  if (ts->i < ts->argc) {
    ts->current = ts->argv[++ts->i];
  }
  if (ts->i == ts->argc) {
    ts->current = NULL;
  }
  return ts;
}

/*
 * ARGV parsing functions
 */

int parse_doubledash(struct Tokens *ts, struct Elements *elements) {
  /*
  int n_commands = elements->n_commands;
  int n_arguments = elements->n_arguments;
  Command *commands = elements->commands;
  Argument *arguments = elements->arguments;

  not implemented yet
  return parsed + [Argument(None, v) for v in tokens]
  */
  return 0;
}

int parse_long(struct Tokens *ts, struct Elements *elements) {
  int i;
  int len_prefix;
  int n_options = elements->n_options;
  char *eq = strchr(ts->current, '=');
  struct Option *option;
  struct Option *options = elements->options;

  len_prefix = (eq - (ts->current)) / sizeof(char);
  for (i = 0; i < n_options; i++) {
    option = &options[i];
    if (!strncmp(ts->current, option->olong, len_prefix))
      break;
  }
  if (i == n_options) {
    /* TODO: %s is not a unique prefix */
    fprintf(stderr, "%s is not recognized\n", ts->current);
    return 1;
  }
  tokens_move(ts);
  if (option->argcount) {
    if (eq == NULL) {
      if (ts->current == NULL) {
        fprintf(stderr, "%s requires argument\n", option->olong);
        return 1;
      }
      option->argument = ts->current;
      tokens_move(ts);
    } else {
      option->argument = eq + 1;
    }
  } else {
    if (eq != NULL) {
      fprintf(stderr, "%s must not have an argument\n", option->olong);
      return 1;
    }
    option->value = true;
  }
  return 0;
}

int parse_shorts(struct Tokens *ts, struct Elements *elements) {
  char *raw;
  int i;
  int n_options = elements->n_options;
  struct Option *option;
  struct Option *options = elements->options;

  raw = &ts->current[1];
  tokens_move(ts);
  while (raw[0] != '\0') {
    for (i = 0; i < n_options; i++) {
      option = &options[i];
      if (option->oshort != NULL && option->oshort[1] == raw[0])
        break;
    }
    if (i == n_options) {
      /* TODO -%s is specified ambiguously %d times */
      fprintf(stderr, "-%c is not recognized\n", raw[0]);
      return EXIT_FAILURE;
    }
    raw++;
    if (!option->argcount) {
      option->value = true;
    } else {
      if (raw[0] == '\0') {
        if (ts->current == NULL) {
          fprintf(stderr, "%s requires argument\n", option->oshort);
          return EXIT_FAILURE;
        }
        raw = ts->current;
        tokens_move(ts);
      }
      option->argument = raw;
      break;
    }
  }
  return EXIT_SUCCESS;
}

int parse_argcmd(struct Tokens *ts, struct Elements *elements) {
  int i;
  int n_commands = elements->n_commands;
  /* int n_arguments = elements->n_arguments; */
  struct Command *command;
  struct Command *commands = elements->commands;
  /* Argument *arguments = elements->arguments; */

  for (i = 0; i < n_commands; i++) {
    command = &commands[i];
    if (strcmp(command->name, ts->current) == 0) {
      command->value = true;
      tokens_move(ts);
      return EXIT_SUCCESS;
    }
  }
  /* not implemented yet, just skip for now
     parsed.append(Argument(None, tokens.move())) */
  /*
  fprintf(stderr, "! argument '%s' has been ignored\n", ts->current);
  fprintf(stderr, "  '");
  for (i=0; i<ts->argc ; i++)
      fprintf(stderr, "%s ", ts->argv[i]);
  fprintf(stderr, "'\n");
  */
  tokens_move(ts);
  return EXIT_SUCCESS;
}

int parse_args(struct Tokens *ts, struct Elements *elements) {
  int ret = EXIT_FAILURE;

  while (ts->current != NULL) {
    if (strcmp(ts->current, "--") == 0) {
      ret = parse_doubledash(ts, elements);
      if (ret == EXIT_FAILURE)
        break;
    } else if (ts->current[0] == '-' && ts->current[1] == '-') {
      ret = parse_long(ts, elements);
    } else if (ts->current[0] == '-' && ts->current[1] != '\0') {
      ret = parse_shorts(ts, elements);
    } else
      ret = parse_argcmd(ts, elements);
    if (ret)
      return ret;
  }
  return ret;
}

int elems_to_args(struct Elements *elements, struct DocoptArgs *args,
                  const bool help, const char *version) {
  struct Command *command;
  struct Argument *argument;
  struct Option *option;
  int i, j;

  /* fix gcc-related compiler warnings (unused) */
  (void)command;
  (void)argument;

  /* options */
  for (i = 0; i < elements->n_options; i++) {
    option = &elements->options[i];
    if (help && option->value && strcmp(option->olong, "--help") == 0) {
      for (j = 0; j < 18; j++)
        puts(args->help_message[j]);
      return EXIT_FAILURE;
    } else if (version && option->value &&
               strcmp(option->olong, "--version") == 0) {
      puts(version);
      return EXIT_FAILURE;
    } else if (strcmp(option->olong, "--help") == 0) {
      args->help = option->value;
    } else if (strcmp(option->olong, "--version") == 0) {
      args->version = option->value;
    } else if (strcmp(option->olong, "--command") == 0) {
      if (option->argument) {
        args->command = (char *)option->argument;
      }
    } else if (strcmp(option->olong, "--command-file") == 0) {
      if (option->argument) {
        args->command_file = (char *)option->argument;
      }
    } else if (strcmp(option->olong, "--output") == 0) {
      if (option->argument) {
        args->output = (char *)option->argument;
      }
    } else if (strcmp(option->olong, "--params") == 0) {
      if (option->argument) {
        args->params = (char *)option->argument;
      }
    }
  }
  /* commands */
  for (i = 0; i < elements->n_commands; i++) {
    command = &elements->commands[i];
  }
  /* arguments */
  for (i = 0; i < elements->n_arguments; i++) {
    argument = &elements->arguments[i];

    if (strcmp(argument->name, "<DSN>") == 0) {
      args->DSN = (char *)argument->value;
    }
  }
  return EXIT_SUCCESS;
}

/*
 * Main docopt function
 */

int docopt(struct DocoptArgs *args, int argc, char *argv[], const bool help,
           const char *version) {
  *args = (struct DocoptArgs){
      NULL,
      0,
      0,
      NULL,
      NULL,
      NULL,
      NULL,
      usage_pattern,
      {"odbc-cli-c: ODBC CLI", "", "Usage:", "  odbc-cli-c <DSN>",
       "  odbc-cli-c --output=<o> <DSN>", "  odbc-cli-c --command=<c> <DSN>",
       "  odbc-cli-c --command-file=<f> <DSN>",
       "  odbc-cli-c -c=<c> --params=<PARAMS> <DSN>", "  odbc-cli-c --help",
       "  odbc-cli-c --version", "",
       "Options:", "  -h --help                     Show this screen.",
       "  --version                     Show version.",
       "  -c=<c>, --command=<c>         Query to execute",
       "  -f=<c>, --command-file=<c>    Alternative query to execute from file "
       "or stdin",
       "  -p=<p>, --params=<p>          Parameters to provide sanitarily to "
       "SQL query",
       "  -o=<o>, --output=<o>          Output file. If not specified: will "
       "use `stdout`. If specified: will overwrite file."}};
  struct Command commands[] = {NULL};
  struct Argument arguments[] = {{"<DSN>", NULL, NULL}};
  struct Option options[] = {
      {"-h", "--help", 0, 0, NULL},    {NULL, "--version", 0, 0, NULL},
      {"-c", "--command", 1, 0, NULL}, {"-f", "--command-file", 1, 0, NULL},
      {"-o", "--output", 1, 0, NULL},  {"-p", "--params", 1, 0, NULL}};
  struct Elements elements;
  int return_code = EXIT_SUCCESS;

  elements.n_commands = 0;
  elements.n_arguments = 1;
  elements.n_options = 6;
  elements.commands = commands;
  elements.arguments = arguments;
  elements.options = options;

  if (argc == 1) {
    argv[argc++] = "--help";
    argv[argc++] = NULL;
    return_code = EXIT_FAILURE;
  }

  {
    struct Tokens ts = tokens_new(argc, argv);
    if (parse_args(&ts, &elements))
      return EXIT_FAILURE;
  }
  elems_to_args(&elements, args, help, version);
  return return_code;
}
