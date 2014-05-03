/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstats.c
 * Version    : $Id: ipv6logstats.c,v 1.55 2014/05/03 07:28:31 ds6peter Exp $
 * Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile statistics
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "config.h"

#include "ipv6logstats.h"
#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "ipv6calccommands.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6logstatsoptions.h"
#include "ipv6calchelp.h"
#include "ipv6logstatshelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libifinet6.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

#define LINEBUFFER	16384

long int ipv6calc_debug = 0;	// ipv6calc_debug usage ok
int ipv6calc_quiet = 0;

static int opt_unknown = 0;
static int opt_simple = 0;
static int opt_noheader = 0;
static int opt_onlyheader = 0;
static int opt_printdirection = 0; /* rows */
static char opt_token[NI_MAXHOST] = "";

char    file_out[NI_MAXHOST] = "";
int     file_out_flag = 0;
FILE    *FILE_OUT;

int feature_cc  = 0;
int feature_as  = 0;
int feature_reg = 0;

static stat_entries ipv6logstats_statentries[] = {
	{ STATS_ALL		, 0, "ALL" },
	{ STATS_IPV4		, 0, "IPv4" },
	{ STATS_IPV6		, 0, "IPv6" },
	{ STATS_UNKNOWN		, 0, "UNKNOWN" },
	{ STATS_IPV4_APNIC	, 0, "IPv4/APNIC" },
	{ STATS_IPV4_ARIN	, 0, "IPv4/ARIN" },
	{ STATS_IPV4_RIPE	, 0, "IPv4/RIPE" },
	{ STATS_IPV4_LACNIC	, 0, "IPv4/LACNIC" },
	{ STATS_IPV4_AFRINIC	, 0, "IPv4/AFRINIC" },
	{ STATS_IPV4_UNKNOWN	, 0, "IPv4/UNKNOWN" },
	{ STATS_IPV6_6BONE	, 0, "IPv6/6bone" },
	{ STATS_IPV6_IANA	, 0, "IPv6/IANA" },
	{ STATS_IPV6_APNIC	, 0, "IPv6/APNIC" },
	{ STATS_IPV6_ARIN	, 0, "IPv6/ARIN" },
	{ STATS_IPV6_RIPE	, 0, "IPv6/RIPE" },
	{ STATS_IPV6_LACNIC	, 0, "IPv6/LACNIC" },
	{ STATS_IPV6_AFRINIC	, 0, "IPv6/AFRINIC" },
	{ STATS_IPV6_RESERVED	, 0, "IPv6/RESERVED" },
	{ STATS_IPV6_UNKNOWN	, 0, "IPv6/UNKNOWN" },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_IANA    , 0, "IPv6/6to4/IANA"     },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_APNIC   , 0, "IPv6/6to4/APNIC"    },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_ARIN    , 0, "IPv6/6to4/ARIN"     },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_RIPE    , 0, "IPv6/6to4/RIPE"     },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_LACNIC  , 0, "IPv6/6to4/LACNIC"   },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_AFRINIC , 0, "IPv6/6to4/AFRINIC"  },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_RESERVED, 0, "IPv6/6to4/RESERVED" },
	{ STATS_IPV6_6TO4_BASE + REGISTRY_UNKNOWN , 0, "IPv6/6to4/UNKNOWN"  },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_IANA    , 0, "IPv6/Teredo/IANA"     },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_APNIC   , 0, "IPv6/Teredo/APNIC"    },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_ARIN    , 0, "IPv6/Teredo/ARIN"     },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_RIPE    , 0, "IPv6/Teredo/RIPE"     },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_LACNIC  , 0, "IPv6/Teredo/LACNIC"   },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_AFRINIC , 0, "IPv6/Teredo/AFRINIC"  },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_RESERVED, 0, "IPv6/Teredo/RESERVED" },
	{ STATS_IPV6_TEREDO_BASE + REGISTRY_UNKNOWN , 0, "IPv6/Teredo/UNKNOWN"  },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_IANA    , 0, "IPv6/NAT64/IANA"     },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_APNIC   , 0, "IPv6/NAT64/APNIC"    },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_ARIN    , 0, "IPv6/NAT64/ARIN"     },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_RIPE    , 0, "IPv6/NAT64/RIPE"     },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_LACNIC  , 0, "IPv6/NAT64/LACNIC"   },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_AFRINIC , 0, "IPv6/NAT64/AFRINIC"  },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_RESERVED, 0, "IPv6/NAT64/RESERVED" },
	{ STATS_IPV6_NAT64_BASE + REGISTRY_UNKNOWN , 0, "IPv6/NAT64/UNKNOWN"  },
	{ STATS_IPV6_IID_GLOBAL, 0, "IPv6/IID/Global" },
	{ STATS_IPV6_IID_RANDOM, 0, "IPv6/IID/Random" },
	{ STATS_IPV6_IID_MANUAL, 0, "IPv6/IID/Manual" },
	{ STATS_IPV6_IID_ISATAP, 0, "IPv6/IID/ISATAP" },
	{ STATS_IPV6_IID_MANUAL, 0, "IPv6/IID/Unknown" },
};

/* stat by Country Code */
static long unsigned int counter_country[COUNTRYCODE_INDEX_MAX];
static long unsigned int counter_country_ipv4[COUNTRYCODE_INDEX_MAX];
static long unsigned int counter_country_ipv6[COUNTRYCODE_INDEX_MAX];

static long unsigned int counter_country_A46, counter_country_IPV4, counter_country_IPV6;

/* stat by ASN (only 16-bit ASN supported, 32-bit ASNs are mapped to 23456 "AS_TRANS" */
#define ASNUM_MAX     65536
static long unsigned int counter_asn[ASNUM_MAX];
static long unsigned int counter_asn_ipv4[ASNUM_MAX];
static long unsigned int counter_asn_ipv6[ASNUM_MAX];

/* prototypes */
static void lineparser(void);


/**************************************************/
/* main */
int main(int argc,char *argv[]) {
	int i, lop, result;
	unsigned long int command = 0;

	/* options */
	struct option longopts[MAXLONGOPTIONS];
	char   shortopts[NI_MAXHOST] = "";
	int    longopts_maxentries = 0;

	/* check for UID */
	if (getuid() == 0) {
		printversion();
		fprintf(stderr, " DON'T RUN THIS PROGRAM AS root USER!\n");
		fprintf(stderr, " This program uses insecure C string handling functions and is not full audited\n");
		fprintf(stderr, "  therefore parsing insecure and unchecked input like logfiles isn't a good choice\n");
		exit(EXIT_FAILURE);
	};

	/* initialize debug value from environment for bootstrap debugging */
	ipv6calc_debug_from_env(); // ipv6calc_debug usage ok

	/* add options */
	ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add(shortopts, sizeof(shortopts), longopts, &longopts_maxentries, ipv6logstats_shortopts, ipv6logstats_longopts, MAXENTRIES_ARRAY(ipv6logstats_longopts));

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {

		/* catch common options */
		result = ipv6calcoptions_common_basic(i, optarg, longopts);
		if (result == 0) {
			// found
			continue;
		};

		switch (i) {
			case -1:
				break;

			case 'v':
				if ((command & CMD_printversion_verbose) != 0) {
					// third time '-v'
					command |= CMD_printversion_verbose2;
				} else if ((command & CMD_printversion) != 0) {
					// second time '-v'
					command |= CMD_printversion_verbose;
				} else {
					command |= CMD_printversion;
				};
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;

			case 'p':
				DEBUGPRINT_WA(DEBUG_ipv6logstats_general, "Given prefix token: %s (%d)", optarg, (int) strlen(optarg));
				if (strlen(optarg) < sizeof(opt_token)) {
					snprintf(opt_token, sizeof(opt_token), "%s", optarg);
				} else {
					fprintf(stderr, " Prefix token too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;

			case 'u':
				opt_unknown = 1;
				break;

			case 's':
				opt_simple = 1;
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
				opt_simple = 1; // force simple mode in addition
				break;

			case 'w':
				if (strlen(optarg) < sizeof(file_out)) {
					snprintf(file_out, sizeof(file_out), "%s", optarg);
					file_out_flag = 1;
				} else {
					fprintf(stderr, " Output file name too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;

			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				break;
		};
	};

	argv += optind;
	argc -= optind;

        /* initialise database wrapper */
	result = libipv6calc_db_wrapper_init();
	if (result != 0) {
		exit(EXIT_FAILURE);
	};

	/* check for basic database support */
	if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY| IPV6CALC_DB_IPV6_TO_REGISTRY | IPV6CALC_DB_CC_TO_REGISTRY) == 1) {
		feature_reg = 1;
	};

	if (opt_simple != 1) {
		/* check for Country Code support */
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV6_TO_CC) == 1) {
			feature_cc = 1;
		};

		/* check for ASN support */
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_IPV6_TO_AS) == 1) {
			feature_as = 1;
		};
	};

	/* do work depending on selection */
	if ((command & CMD_printversion) != 0) {
		if ((command & CMD_printversion_verbose) != 0) {
			printversion_verbose(((command & CMD_printversion_verbose2) !=0) ? LEVEL_VERBOSE2 : LEVEL_VERBOSE);
		} else {
			printversion();
		};
		exit(EXIT_SUCCESS);
	};

	if (command == CMD_printhelp) {
		ipv6logstats_printhelp();
		exit(EXIT_FAILURE);
	};

	if (feature_reg == 0) {
		fprintf(stderr, "Basic databases are missing for creating statistic\n");
		exit(EXIT_FAILURE);
	};

	/* call lineparser */
	lineparser();

	exit(EXIT_SUCCESS);
};


/*
 * Statistics structure handling
 */
static void stat_inc(int number) {
	int i;
	
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6logstats_statentries); i++) {
		if (number == ipv6logstats_statentries[i].number) {
			ipv6logstats_statentries[i].counter++;
			break;
		};
	};
};


/*
 * Country code statistics
 */
static void stat_inc_country_code(uint16_t country_code, const int proto) {
	int index = COUNTRYCODE_INDEX_UNKNOWN;

	if (country_code < COUNTRYCODE_INDEX_MAX) {
		index = country_code;
	} else {
		fprintf(stderr, "%s/%s: unexpected index (too high): %d\n", __FILE__, __func__, index);
		exit(1);
	};

	DEBUGPRINT_WA(DEBUG_ipv6logstats_general, "Increment CountryCode index: %d (%d)", index, country_code);

	counter_country[index]++;
	counter_country_A46++;

	if (proto == 4) {
		counter_country_ipv4[index]++;
		counter_country_IPV4++;
	} else if (proto == 6) {
		counter_country_ipv6[index]++;
		counter_country_IPV6++;
	} else {
		fprintf(stderr, "%s/%s: unexpected unsupported proto: %d\n", __FILE__, __func__, proto);
		exit(1);
	};
};


/*
 * AS Number statistics
 */
static void stat_inc_asnum(const uint32_t as_num32, const int proto) {
	unsigned int index = ASNUM_AS_UNKNOWN;

	if (as_num32 < ASNUM_MAX) {
		// everything is fine
		index = as_num32;
	} else {
		// map to AS_TRANS for now
		index = ASNUM_AS_TRANS;
	};

	DEBUGPRINT_WA(DEBUG_ipv6logstats_general, "Increment ASN index: %d (%d)", index, as_num32);

	counter_asn[index]++;

	if (proto == 4) {
		counter_asn_ipv4[index]++;
	} else if (proto == 6) {
		counter_asn_ipv6[index]++;
	};
};


/*
 * Line parser
 */
static void lineparser(void) {
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char resultstring[LINEBUFFER];
	char *charptr, *cptr, **ptrptr;
	int linecounter = 0, retval, i, r;

	uint32_t inputtype  = FORMAT_undefined;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;
	int registry, stat_registry_base;

	time_t timer;
	struct tm* tm_info;

	int index;
	uint16_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	uint32_t as_num32 = ASNUM_AS_UNKNOWN;
	long unsigned int c_all, c_ipv4, c_ipv6;

	// clear counters
	for (i = 0; i < COUNTRYCODE_INDEX_MAX; i++) {
		counter_country[i] = 0;
		counter_country_ipv4[i] = 0;
		counter_country_ipv6[i] = 0;
	};

	ptrptr = &cptr;
	
	if (opt_onlyheader == 0) {
		if (ipv6calc_quiet == 0) {
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

		stat_registry_base = 0;

		if (linecounter == 1) {
			if (ipv6calc_quiet == 0) {
				fprintf(stderr, "Ok, proceeding stdin...\n");
			};
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6logstats_processing, "Line counter: %d", linecounter);

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};

		/* remove trailing \n */
		if (linebuffer[strlen(linebuffer) - 1] == '\n') {
			linebuffer[strlen(linebuffer) - 1] = '\0';
		};

		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6logstats_processing, "Got line: '%s'", linebuffer);

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
		
		DEBUGPRINT_WA(DEBUG_ipv6logstats_processing, "Token 1: '%s'", token);

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

		/* catch compat/mapped */
		switch (inputtype) {
			case FORMAT_ipv6addr:
				if ((ipv6addr.scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0) {
					/* extract IPv4 address */
					r = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, 1);
					if (r != 0) {
						continue;
					};

					// remap
					inputtype = FORMAT_ipv4addr;

					// create text represenation
					r = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, token, 0);
				};
				break;

			default:
				// nothing to do
				break;
		};

		/* get information and fill statistics */
		switch (inputtype) {
			case FORMAT_ipv6addr:
				/* is IPv6 address */
				stat_inc(STATS_IPV6);

				if ((ipv6addr.scope & IPV6_ADDR_HAS_PUBLIC_IPV4) != 0) {
					/* has public IPv4 address included */

					// get IPv4 address
					r = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, 1);
					if (r != 0) {
						continue;
					};

					cc_index = libipv4addr_cc_index_by_addr(&ipv4addr);
					as_num32 = libipv4addr_as_num32_by_addr(&ipv4addr);
					registry = libipv4addr_registry_num_by_addr(&ipv4addr);

					if (feature_cc == 1) {
						stat_inc_country_code(cc_index, 4);
					};

					if (feature_as == 1) {
						stat_inc_asnum(as_num32, 4);
					};

					if ((ipv6addr.scope & IPV6_NEW_ADDR_6TO4) != 0) {
						stat_registry_base = STATS_IPV6_6TO4_BASE;

					} else if ((ipv6addr.scope & IPV6_NEW_ADDR_TEREDO) != 0) {
						stat_registry_base = STATS_IPV6_TEREDO_BASE;

					} else if ((ipv6addr.scope & IPV6_NEW_ADDR_NAT64) != 0) {
						stat_registry_base = STATS_IPV6_NAT64_BASE;
					};

					if (stat_registry_base > 0) {
						switch (registry) {
							case IPV4_ADDR_REGISTRY_IANA:
								stat_inc(stat_registry_base + REGISTRY_IANA);
								break;
							case IPV4_ADDR_REGISTRY_APNIC:
								stat_inc(stat_registry_base + REGISTRY_APNIC);
								break;
							case IPV4_ADDR_REGISTRY_ARIN:
								stat_inc(stat_registry_base + REGISTRY_ARIN);
								break;
							case IPV4_ADDR_REGISTRY_RIPE:
								stat_inc(stat_registry_base + REGISTRY_RIPE);
								break;
							case IPV4_ADDR_REGISTRY_LACNIC:
								stat_inc(stat_registry_base + REGISTRY_LACNIC);
								break;
							case IPV4_ADDR_REGISTRY_AFRINIC:
								stat_inc(stat_registry_base + REGISTRY_AFRINIC);
								break;
							case IPV4_ADDR_REGISTRY_RESERVED:
								stat_inc(stat_registry_base + REGISTRY_RESERVED);
								break;
							default:
								stat_inc(stat_registry_base + REGISTRY_UNKNOWN);
								if (opt_unknown == 1) {
									fprintf(stderr, "Unknown address: %s\n", token);
								};
								break;
						};
					} else {
						if (opt_unknown == 1) {
							fprintf(stderr, "Unknown address: %s\n", token);
						};
					};
				} else {
					cc_index = libipv6addr_cc_index_by_addr(&ipv6addr);
					as_num32 = libipv6addr_as_num32_by_addr(&ipv6addr);
					registry = libipv6addr_registry_num_by_addr(&ipv6addr);

					if (feature_cc == 1) {
						/* country code */
						stat_inc_country_code(cc_index, 6);
					};

					if (feature_as == 1) {
						/* asnum */
						stat_inc_asnum(as_num32, 6);
					};

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
						case IPV6_ADDR_REGISTRY_AFRINIC:
							stat_inc(STATS_IPV6_AFRINIC);
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

					if ((ipv6addr.scope & IPV6_NEW_ADDR_IID) == IPV6_NEW_ADDR_IID) {
						if ((ipv6addr.scope & IPV6_NEW_ADDR_IID_RANDOM) != 0) {
							stat_inc(STATS_IPV6_IID_RANDOM);
						} else if ((ipv6addr.scope & IPV6_NEW_ADDR_IID_ISATAP) != 0) {
							stat_inc(STATS_IPV6_IID_ISATAP);
						} else if ((ipv6addr.scope & IPV6_NEW_ADDR_IID_LOCAL) != 0) {
							stat_inc(STATS_IPV6_IID_MANUAL);
						} else if ((ipv6addr.scope & IPV6_NEW_ADDR_IID_GLOBAL) != 0) {
							stat_inc(STATS_IPV6_IID_GLOBAL);
						} else {
							stat_inc(STATS_IPV6_IID_UNKNOWN);
						};
					};
				};
				
				break;

			case FORMAT_ipv4addr:
				/* is IPv4 address */
				stat_inc(STATS_IPV4);

				cc_index = libipv4addr_cc_index_by_addr(&ipv4addr);
				as_num32 = libipv4addr_as_num32_by_addr(&ipv4addr);
				registry = libipv4addr_registry_num_by_addr(&ipv4addr);

				stat_inc_country_code(cc_index, 4);
				stat_inc_asnum(as_num32, 4);

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
					case IPV4_ADDR_REGISTRY_AFRINIC:
						stat_inc(STATS_IPV4_AFRINIC);
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
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "...finished\n");
		};
	};

	/* print result */
	if (opt_printdirection == 0) {
		/* print in rows */

		/* print version number */
		printf("%-20s %d.%d\n", "*Version", STATS_VERSION_MAJOR, STATS_VERSION_MINOR);

		time(&timer);
		tm_info = gmtime(&timer);
		strftime(resultstring, sizeof(resultstring), "%Y:%m:%d %H:%M:%S%z %Z", tm_info);

		printf("*DateTime: %s\n", resultstring);
		printf("*UnixTime: %ju\n", (uintmax_t) timer);

		libipv6calc_db_wrapper_print_db_info(0, "*3*DB-Info: ");

		/* print version number */
		if (strlen(opt_token) > 0) {
			printf("%-20s %s\n", "*Token", opt_token);
		};

		for (i = 0; i < MAXENTRIES_ARRAY(ipv6logstats_statentries); i++) {
			printf("%-20s %lu\n", ipv6logstats_statentries[i].token, ipv6logstats_statentries[i].counter);
		};

		if (feature_cc == 1) {
			/* country_code / proto */
			for (index = 0; index < COUNTRYCODE_INDEX_MAX; index++) {
				if (counter_country[index] > 0) {
					DEBUGPRINT_WA(DEBUG_ipv6logstats_summary, "CC-Index: %d", index);

					libipv6calc_db_wrapper_country_code_by_cc_index(resultstring, sizeof(resultstring), index);

					printf("*3*CC-code-proto/%s/ALL   %lu\n", resultstring, counter_country[index]);
					printf("*3*CC-code-proto/%s/IPv4  %lu\n", resultstring, counter_country_ipv4[index]);
					printf("*3*CC-code-proto/%s/IPv6  %lu\n", resultstring, counter_country_ipv6[index]);
					printf("*3*CC-code-proto-list/%s  %lu %lu %lu\n", resultstring, counter_country[index], counter_country_ipv4[index], counter_country_ipv6[index]);
				};
			};

			/* proto / country_code */
			c_all = 0; c_ipv4 = 0; c_ipv6 = 0;
			for (index = 0; index < COUNTRYCODE_INDEX_MAX; index++) {
				if (counter_country[index] > 0) {
					libipv6calc_db_wrapper_country_code_by_cc_index(resultstring, sizeof(resultstring), index);
					printf("*3*CC-proto-code/ALL/%s   %lu\n", resultstring, counter_country[index]);
					c_all += counter_country[index];
				};
			};
			for (index = 0; index < COUNTRYCODE_INDEX_MAX; index++) {
				if (counter_country_ipv4[index] > 0) {
					libipv6calc_db_wrapper_country_code_by_cc_index(resultstring, sizeof(resultstring), index);
					printf("*3*CC-proto-code/IPv4/%s  %lu\n", resultstring, counter_country_ipv4[index]);
					c_ipv4 += counter_country_ipv4[index];
				};
			};
			for (index = 0; index < COUNTRYCODE_INDEX_MAX; index++) {
				if (counter_country_ipv6[index] > 0) {
					libipv6calc_db_wrapper_country_code_by_cc_index(resultstring, sizeof(resultstring), index);
					printf("*3*CC-proto-code/IPv6/%s  %lu\n", resultstring, counter_country_ipv6[index]);
					c_ipv6 += counter_country_ipv6[index];
				};
			};

			if ((c_all + c_ipv4 + c_ipv6) > 0) {
				printf("*3*CC-proto-code-list/ALL  %lu %lu %lu\n", c_all, c_ipv4, c_ipv6);
			};
		};

		if (feature_as == 1) {
			/* ASN number / proto */
			for (index = 0; index < ASNUM_MAX; index++) {
				if (counter_asn[index] > 0) {
					printf("*3*AS-num-proto/%d/ALL   %lu\n", index, counter_asn[index]);
					printf("*3*AS-num-proto/%d/IPv4  %lu\n", index, counter_asn_ipv4[index]);
					printf("*3*AS-num-proto/%d/IPv6  %lu\n", index, counter_asn_ipv6[index]);
					printf("*3*AS-num-proto-list/%d  %lu %lu %lu\n", index, counter_asn[index], counter_asn_ipv4[index], counter_asn_ipv6[index]);
				};
			};

			/* ASN proto / number */
			c_all = 0; c_ipv4 = 0; c_ipv6 = 0;
			for (index = 0; index < ASNUM_MAX; index++) {
				if (counter_asn[index] > 0) {
					printf("*3*AS-proto-num/ALL/%d   %lu\n", index, counter_asn[index]);
					c_all += counter_asn[index];
				};
			};
			for (index = 0; index < ASNUM_MAX; index++) {
				if (counter_asn_ipv4[index] > 0) {
					printf("*3*AS-proto-num/IPv4/%d  %lu\n", index, counter_asn_ipv4[index]);
					c_ipv4 += counter_asn_ipv4[index];
				};
			};
			for (index = 0; index < ASNUM_MAX; index++) {
				if (counter_asn_ipv6[index] > 0) {
					printf("*3*AS-proto-num/IPv6/%d  %lu\n", index, counter_asn_ipv6[index]);
					c_ipv6 += counter_asn_ipv6[index];
				};
			};

			if ((c_all + c_ipv4 + c_ipv6) > 0) {
				printf("*3*AS-proto-num-list/ALL  %lu %lu %lu\n", c_all, c_ipv4, c_ipv6);
			};
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
			for (i = 1; i < MAXENTRIES_ARRAY(ipv6logstats_statentries); i++) {
				if (i > 0) {
					printf(" ");
				};
				printf("%s", ipv6logstats_statentries[i].token);
			};
			printf(" #Version(%d.%d)\n", STATS_VERSION_MAJOR, STATS_VERSION_MINOR);
		};
		if (opt_onlyheader == 0) {
			if (strlen(opt_token) > 0) {
				printf("%s ", opt_token);
			};
			for (i = 1; i < MAXENTRIES_ARRAY(ipv6logstats_statentries); i++) {
				if (i > 0) {
					printf(" ");
				};
				printf("%lu", ipv6logstats_statentries[i].counter);
			};

			printf(" #%d.%d\n", STATS_VERSION_MAJOR, STATS_VERSION_MINOR);
		};
	};

	return;
};
