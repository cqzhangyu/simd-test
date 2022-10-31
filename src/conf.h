#ifndef __CONF_H__
#define __CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

void parse_conf(int argc, char **argv);

void print_conf();

struct conf_t {
	int n;
	int epoch;
};

extern struct conf_t conf;

#endif
