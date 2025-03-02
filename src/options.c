#include "options.h"

#include <getopt.h>
#include <stdlib.h>

options_t options;

void
parse_options (int argc, char **argv)
{
    int                 long_idx   = 0, opt;

    const struct option longopts[] = {
        { "log_path", required_argument, NULL, 'l' },
        { "ip",       required_argument, NULL, 'i' },
    };

    options.log_path = "./arpsnif.log";

    while ((opt = getopt_long (argc, argv, "l:i:", longopts, &long_idx)) !=
           -1) {
        switch (opt) {
        case 'l':
            options.log_path = optarg;
            break;
#if 0
        case 'i':
            options.ip = optarg;
            break;
#endif
        }
    }
}
