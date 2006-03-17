


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "librfc1884.h"

#include "ipv6calctypes.h"

#include "libipv6calcdebug.h"

#include "../databases/ipv4-assignment/dbipv4addr_assignment.h"
#include "../databases/ipv6-assignment/dbipv6addr_assignment.h"

//long int ipv6calc_debug = DEBUG_libipv4addr;
long int ipv6calc_debug = 0;

/* main */
#define DEBUG_function_name "registry-assignment-to-list/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	int retval;

        /* used structures */
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	int type = 0; /* default */
	char registry[NI_MAXHOST] = "";
	int i;

	/* Fetch the command-line arguments. */
	while ((i = getopt(argc, argv, "d46r:")) != EOF) {
		switch (i) {
			case -1:
				break;

			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case '4':
				type = 4;
				break;

			case '6':
				type = 6;
				break;

			case 'r':
				snprintf(registry, sizeof(registry) - 1, "%s", optarg);
				break;

			default:
				fprintf(stderr, "Usage: -4|6 -r <registry> [-d <debug value>]\n");
				break;
		};
	};

	if (registry == NULL || strlen(registry) == 0) {
		fprintf(stderr, "missing registry (option -r)\n");
		exit(EXIT_FAILURE);
	};

	if (type != 4 && type != 6) {
		fprintf(stderr, "missing address selection (option -4|6)\n");
		exit(EXIT_FAILURE);
	};

	/* clear address structures */
	ipv4addr_clearall(&ipv4addr);
	ipv6addr_clearall(&ipv6addr);

	printf ("# Selected registry: %s\n", registry);
	printf ("# Selected IP address version: IPv%d\n", type);


	if (type == 4) {
		for (i = 0; i < (int) ( sizeof(dbipv4addr_assignment) / sizeof(dbipv4addr_assignment[0])); i++) {
			if (strcmp(registry, dbipv4addr_assignment[i].string_registry) != 0) {
				continue;
			};

			ipv4addr_setdword(&ipv4addr, dbipv4addr_assignment[i].ipv4addr);

			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, 0);
			//printf("%s/%d %08x %08x\ %08x\n", resultstring, j, dbipv4addr_assignment[i].ipv4addr, dbipv4addr_assignment[i].ipv4mask, ipv4addr.in_addr.s_addr);
			printf("%s/%d\n", resultstring, dbipv4addr_assignment[i].prefixlength);
			//	break;
		};
	} else if (type == 6) {
		for (i = 0; i < (int) ( sizeof(dbipv6addr_assignment) / sizeof(dbipv6addr_assignment[0])); i++) {
			if (strcmp(registry, dbipv6addr_assignment[i].string_registry) != 0) {
				continue;
			};

			ipv6addr_setdword(&ipv6addr, 0, dbipv6addr_assignment[i].ipv6addr_00_31);
			ipv6addr_setdword(&ipv6addr, 1, dbipv6addr_assignment[i].ipv6addr_32_63);

			retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, FORMATOPTION_printcompressed);
			//printf("%s/%d %08x %08x\ %08x\n", resultstring, j, dbipv4addr_assignment[i].ipv4addr, dbipv4addr_assignment[i].ipv4mask, ipv4addr.in_addr.s_addr);
			printf("%s/%d\n", resultstring, dbipv6addr_assignment[i].prefixlength);
			//	break;
		};
	};
};
