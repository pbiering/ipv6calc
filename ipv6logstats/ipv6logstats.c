/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstats.c
 * Version    : $Id: ipv6logstats.c,v 1.13 2011/09/16 18:05:13 peter Exp $
 * Copyright  : 2003-2005 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile statistics
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "ipv6logstats.h"
#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "ipv6calccommands.h"
#include "ipv6calctypes.h"
#include "ipv6logstatsoptions.h"
#include "ipv6calchelp.h"
#include "ipv6logstatshelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libifinet6.h"

#define LINEBUFFER	16384

long int ipv6calc_debug = 0;
int flag_quiet = 0;

static int opt_unknown = 0;
static int opt_noheader = 0;
static int opt_onlyheader = 0;
static int opt_printdirection = 0; /* rows */
static char opt_token[NI_MAXHOST] = "";

/* prototypes */
static void lineparser(void);

/**************************************************/
/* main */
#define DEBUG_function_name "ipv6logstats/main"
int main(int argc,char *argv[]) {
	int i, lop;
	unsigned long int command = 0;

	/* check for UID */
	if (getuid() == 0) {
		printversion();
		fprintf(stderr, " DON'T RUN THIS PROGRAM AS root USER!\n");
		fprintf(stderr, " This program uses insecure C string handling functions and is not full audited\n");
		fprintf(stderr, "  therefore parsing insecure and unchecked input like logfiles isn't a good choice\n");
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6logstats_shortopts, ipv6logstats_longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;

			case 'v':
				command |= CMD_printversion;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;

			case 'q':
				flag_quiet = 1;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case 'p':
				strncpy(opt_token, optarg, NI_MAXHOST - 1);
				opt_token[NI_MAXHOST - 1] = '\0';
				break;

			case 'u':
				opt_unknown = 1;
				break;

			case 'n':
				opt_noheader = 1;
				opt_printdirection = 1;
				break;

			case 'o':
				opt_onlyheader = 1;
				opt_printdirection = 1;
				break;

			case 'c':
				opt_printdirection = 1;
				break;

			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(EXIT_FAILURE);
	};

	if (command == CMD_printhelp) {
		ipv6logstats_printhelp();
		exit(EXIT_FAILURE);
	};
	

	/* call lineparser */
	lineparser();

	exit(EXIT_SUCCESS);
};
#undef DEBUG_function_name

/*
 * Statistics structure handling
 */
#define DEBUG_function_name "ipv6logstats/stat_inc"
static void stat_inc(int number) {
	int i;
	
	for (i = 0; i < (int) (sizeof(ipv6logstats_statentries) / sizeof(ipv6logstats_statentries[0])); i++) {
		if (number == ipv6logstats_statentries[i].number) {
			ipv6logstats_statentries[i].counter++;
			break;
		};
	};
};
#undef DEBUG_function_name


/*
 * Line parser
 */
#define DEBUG_function_name "ipv6logstats/lineparser"
static void lineparser(void) {
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char resultstring[LINEBUFFER];
	char *charptr, *cptr, **ptrptr;
	int linecounter = 0, retval, i;

	uint32_t inputtype  = FORMAT_undefined;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;
	int registry;
	uint32_t typeinfo;

	ptrptr = &cptr;
	
	if (opt_onlyheader == 0) {
		if (flag_quiet == 0) {
			fprintf(stderr, "Expecting log lines on stdin\n");
		};
	};

	while (opt_onlyheader == 0) {
		/* read line from stdin */
		charptr = fgets(linebuffer, LINEBUFFER, stdin);
		
		if (charptr == NULL) {
			/* end of input */
			break;
		};

		linecounter++;

		if (linecounter == 1) {
			if (flag_quiet == 0) {
				fprintf(stderr, "Ok, proceeding stdin...\n");
			};
		};
		
		if (ipv6calc_debug == 1) {
			fprintf(stderr, "Line: %d\r", linecounter);
		};

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
		};
		
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Got line: '%s'\n", DEBUG_function_name, linebuffer);
		};

		/* look for first token (should be IP address) */
		charptr = strtok_r(linebuffer, " \t\n", ptrptr);
		
		if ( charptr == NULL ) {
			fprintf(stderr, "Line contains no token: %d\n", linecounter);
			continue;
		};

		if ( strlen(charptr) >=  LINEBUFFER) {
			fprintf(stderr, "Line too strange: %d\n", linecounter);
			continue;
		};

		snprintf(token, sizeof(token) - 1, "%s", charptr);
		
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Token 1: '%s'\n", DEBUG_function_name, token);
		};

		stat_inc(STATS_ALL);

		/* get input type now */
		inputtype = libipv6calc_autodetectinput(token);

		/* check for proper type */
		if ((inputtype != FORMAT_ipv4addr) && (inputtype != FORMAT_ipv6addr)) {
			/* fprintf(stderr, "Token 1 (address) is not an IP address in line: %d\n", linecounter); */
			stat_inc(STATS_UNKNOWN);
			continue;
		};

		/* fill related structure */
		switch (inputtype) {
			case FORMAT_ipv6addr:
				retval = addr_to_ipv6addrstruct(token, resultstring, &ipv6addr);
				break;

			case FORMAT_ipv4addr:
				retval = addr_to_ipv4addrstruct(token, resultstring, &ipv4addr);
				break;

			default:
				retval = 0;
				break;
		};

		if (retval != 0 ) {
			fprintf(stderr, "Problem during address parsing on line %d (skipped): %s\n", linecounter, resultstring);
			continue;
		};

		/* get information and fill statistics */
		switch (inputtype) {
			case FORMAT_ipv6addr:
				/* is IPv6 address */
				stat_inc(STATS_IPV6);

				/* get type */
				typeinfo = ipv6addr_gettype(&ipv6addr);

				if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
					/* 6to4 address */
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(&ipv6addr, (unsigned int) 2 + i));
					};
					
					/* get registry */
					registry = ipv4addr_getregistry(&ipv4addr);
					switch (registry) {
						case IPV4_ADDR_REGISTRY_IANA:
							stat_inc(STATS_IPV6_6TO4_IANA);
							break;
						case IPV4_ADDR_REGISTRY_APNIC:
							stat_inc(STATS_IPV6_6TO4_APNIC);
							break;
						case IPV4_ADDR_REGISTRY_ARIN:
							stat_inc(STATS_IPV6_6TO4_ARIN);
							break;
						case IPV4_ADDR_REGISTRY_RIPE:
							stat_inc(STATS_IPV6_6TO4_RIPE);
							break;
						case IPV4_ADDR_REGISTRY_LACNIC:
							stat_inc(STATS_IPV6_6TO4_LACNIC);
							break;
						case IPV4_ADDR_REGISTRY_RESERVED:
							stat_inc(STATS_IPV6_6TO4_RESERVED);
							break;
						default:
							stat_inc(STATS_IPV6_6TO4_UNKNOWN);
							if (opt_unknown == 1) {
								fprintf(stderr, "Unknown address: %s\n", token);
							};
							break;
					};
				} else if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
					/* Teredo address */
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(&ipv6addr, (unsigned int) 12 + i) ^ 0xff);
					};
					
					/* get registry */
					registry = ipv4addr_getregistry(&ipv4addr);
					switch (registry) {
						case IPV4_ADDR_REGISTRY_IANA:
							stat_inc(STATS_IPV6_TEREDO_IANA);
							break;
						case IPV4_ADDR_REGISTRY_APNIC:
							stat_inc(STATS_IPV6_TEREDO_APNIC);
							break;
						case IPV4_ADDR_REGISTRY_ARIN:
							stat_inc(STATS_IPV6_TEREDO_ARIN);
							break;
						case IPV4_ADDR_REGISTRY_RIPE:
							stat_inc(STATS_IPV6_TEREDO_RIPE);
							break;
						case IPV4_ADDR_REGISTRY_LACNIC:
							stat_inc(STATS_IPV6_TEREDO_LACNIC);
							break;
						case IPV4_ADDR_REGISTRY_RESERVED:
							stat_inc(STATS_IPV6_TEREDO_RESERVED);
							break;
						default:
							stat_inc(STATS_IPV6_TEREDO_UNKNOWN);
							if (opt_unknown == 1) {
								fprintf(stderr, "Unknown address: %s\n", token);
							};
							break;
					};
				} else {
					/* Native IPv6 address */

					/* get registry */
					registry = ipv6addr_getregistry(&ipv6addr);
					switch (registry) {
						case IPV6_ADDR_REGISTRY_6BONE:
							stat_inc(STATS_IPV6_6BONE);
							break;
						case IPV6_ADDR_REGISTRY_IANA:
							stat_inc(STATS_IPV6_IANA);
							break;
						case IPV6_ADDR_REGISTRY_APNIC:
							stat_inc(STATS_IPV6_APNIC);
							break;
						case IPV6_ADDR_REGISTRY_ARIN:
							stat_inc(STATS_IPV6_ARIN);
							break;
						case IPV6_ADDR_REGISTRY_RIPE:
							stat_inc(STATS_IPV6_RIPE);
							break;
						case IPV6_ADDR_REGISTRY_LACNIC:
							stat_inc(STATS_IPV6_LACNIC);
							break;
						case IPV6_ADDR_REGISTRY_RESERVED:
							stat_inc(STATS_IPV6_RESERVED);
							break;
						default:
							stat_inc(STATS_IPV6_UNKNOWN);
							if (opt_unknown == 1) {
								fprintf(stderr, "Unknown address: %s\n", token);
							};
							break;
					};
				};
				
				break;

			case FORMAT_ipv4addr:
				/* is IPv4 address */
				stat_inc(STATS_IPV4);

				/* get registry */
				registry = ipv4addr_getregistry(&ipv4addr);
				switch (registry) {
					case IPV4_ADDR_REGISTRY_IANA:
						stat_inc(STATS_IPV4_IANA);
						break;
					case IPV4_ADDR_REGISTRY_APNIC:
						stat_inc(STATS_IPV4_APNIC);
						break;
					case IPV4_ADDR_REGISTRY_ARIN:
						stat_inc(STATS_IPV4_ARIN);
						break;
					case IPV4_ADDR_REGISTRY_RIPE:
						stat_inc(STATS_IPV4_RIPE);
						break;
					case IPV4_ADDR_REGISTRY_LACNIC:
						stat_inc(STATS_IPV4_LACNIC);
						break;
					case IPV4_ADDR_REGISTRY_RESERVED:
						stat_inc(STATS_IPV4_RESERVED);
						break;
					default:
						stat_inc(STATS_IPV4_UNKNOWN);
						if (opt_unknown == 1) {
							fprintf(stderr, "Unknown address: %s\n", token);
						};
						break;
				};
				
				break;
		};
	};

	if (opt_onlyheader == 0) {
		if (flag_quiet == 0) {
			fprintf(stderr, "...finished\n");
		};
	};

	/* print result */
	if (opt_printdirection == 0) {
		/* print in rows */
		printf("%-20s %d.%d\n", "*Version", STATS_VERSION_MAJOR, STATS_VERSION_MINOR);

		/* print version number */
		if (strlen(opt_token) > 0) {
			printf("%-20s %s\n", "*Token", opt_token);
		};
		for (i = 0; i < (int) (sizeof(ipv6logstats_statentries) / sizeof(ipv6logstats_statentries[0])); i++) {
			printf("%-20s %lu\n", ipv6logstats_statentries[i].token, ipv6logstats_statentries[i].counter);
		};
	} else {
		/* print in columns */
		if (opt_noheader == 0) {
			if (strlen(opt_token) > 0) {
				if (opt_onlyheader == 0) {
					printf("Token ");
				} else {
					printf("%s ", opt_token);
				};
			};
			for (i = 0; i < (int) (sizeof(ipv6logstats_statentries) / sizeof(ipv6logstats_statentries[0])); i++) {
				if (i > 0) {
					printf(" ");
				};
				printf("%s", ipv6logstats_statentries[i].token);
			};
			printf("\n");
		};
		if (opt_onlyheader == 0) {
			if (strlen(opt_token) > 0) {
				printf("%s ", opt_token);
			};
			for (i = 0; i < (int) (sizeof(ipv6logstats_statentries) / sizeof(ipv6logstats_statentries[0])); i++) {
				if (i > 0) {
					printf(" ");
				};
				printf("%lu", ipv6logstats_statentries[i].counter);
			};
			printf("\n");
		};
	};

	return;
};
#undef DEBUG_function_name



