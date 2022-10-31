#include "conf.h"
#include "util.h"

struct conf_t conf = {0};

static uint64_t
convert_to_bytes(char* str) {
    uint64_t temp = atoi(str);
    if (has_suffix(str, "B") == true) {
    } else if (has_suffix(str, "K") == true) {
        temp = temp * 1024;
    } else if (has_suffix(str, "M") == true) {
        temp = temp * 1024 * 1024;
    } else if (has_suffix(str, "G") == true) {
        temp = temp * 1024 * 1024 * 1024;
    } else if (has_suffix(str, "T") == true) {
        temp = temp * 1024 * 1024 * 1024 * 1024;
    } else {
        LOG_WARN("Unrecognized suffix in %s. Supported suffix is B/K/M/G/T, default to Byte\n", str);
    }
    return temp;
}

static inline void
show_usage(char* app) {
    LOG_INFO("Usage: %s [options]\n", app);
    LOG_INFO("       %s [-h|--help]\n", app);
    LOG_INFO("\n");
    LOG_INFO("Options:\n");
    LOG_INFO("      -n\n");
    LOG_INFO("      -e\n");
    

    exit(0);
}

void
print_conf() {
    LOG_INFO("  size of matrix :    n       = %d\n", conf.n);
    LOG_INFO("  num of epochs :     epoch   = %d\n", conf.epoch);
}

void
parse_conf(int argc, char **argv) {
    char* app = argv[0];

    if (argc == 1)
        show_usage(app);
    
    static int lopt = 0;
    static struct option opts[] = {
        // {"size", required_argument, &lopt, 1},
        {0, 0, 0, 0}
    };
    
    int c, opt_index = 0;
    optind = 1; // reset getopt_long
    while ((c = getopt_long(argc, argv, "n:e:", opts, &opt_index)) != -1) {
        switch(c) {
        case 0:
            switch(lopt) {
            default:
                show_usage(app);
                break;
            }
            break;
        case 'n':
            conf.n = atoi(optarg);
            break;
        case 'e':
            conf.epoch = atoi(optarg);
            break;
        default:
            show_usage(app);
            break;
        }
    }

    if (!conf.n)
        conf.n = 1000;
    if (!conf.epoch)
        conf.epoch = 1;
}

