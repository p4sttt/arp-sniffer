#ifndef OPTIONS_H_
#define OPTIONS_H_

typedef struct {
    char *log_path;
} options_t;

extern options_t options;

/* */

void parse_options (int argc, char **argv);

#endif
