/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.c
 * Version    : $Id$
 * Copyright  : 2002-2017 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"
#include "config.h"

#include "libieee.h"
#include "databases/lib/libipv6calc_db_wrapper.h"
#include "databases/lib/libipv6calc_db_wrapper_GeoIP.h"
#include "databases/lib/libipv6calc_db_wrapper_IP2Location.h"
#include "databases/lib/libipv6calc_db_wrapper_DBIP.h"
#include "databases/lib/libipv6calc_db_wrapper_BuiltIn.h"
#include "databases/lib/libipv6calc_db_wrapper_External.h"


#ifdef SUPPORT_IP2LOCATION
/* 
 *  * API_VERSION is defined as a bareword in IP2Location.h, 
 *   * we need this trick to stringify it. Blah.
 *    */
#define makestr(x) #x
#define xmakestr(x) makestr(x)

extern char* file_ip2location_ipv4;
extern char* file_ip2location_ipv6;
#endif

/* to be defined in each application */
extern void printversion(void);
extern void printcopyright(void);


/* format option arguments */
void printhelp_print(void) {
	fprintf(stderr, "   --printprefix        : print only prefix of IPv6 address\n");
        fprintf(stderr, "   --printsuffix        : print only suffix of IPv6 address\n");
};

void printhelp_mask(void) {
	fprintf(stderr, "   --maskprefix         : mask IPv6 address with prefix length (clears suffix bits)\n");
        fprintf(stderr, "   --masksuffix         : mask IPv6 address with suffix length (clears prefix bits)\n");
};

void printhelp_case(void) {
	fprintf(stderr, "   --uppercase|-u       : print chars of IPv6 address in upper case\n");
        fprintf(stderr, "   --lowercase|-l       : print chars of IPv6 address in lower case [default]\n");
};

void printhelp_printstartend(void) {
	fprintf(stderr, "   --printstart <1-128> : print part of IPv6 address start from given number\n");
	fprintf(stderr, "   --printend   <1-128> : print part of IPv6 address end at given number\n");
};

void printhelp_doublecommands(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Only one command may be specified!\n");
};

void printhelp_missinginputdata(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Missing or to few input data given!\n");
};

/* list of input types */
void printhelp_inputtypes(const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring), "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring), "%%-%ds\n", (int) maxlen);
	}

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Format string: %s", printformatstring);

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available input types:\n");

	/* run through matrix */
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatmatrix); i++) {
		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Row %d: %08x - %08x", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);
		if (ipv6calc_formatmatrix[i][1] != 0) {
			/* available for input, look for name now */
			for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
				DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Format-Row %d: %08x - %s - %s", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				if (ipv6calc_formatstrings[j].number == ipv6calc_formatmatrix[i][0]) {
					if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
						fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
					} else {
						fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token);
					};
				};
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* list of output types */
void printhelp_outputtypes(const uint32_t inputtype, const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring), "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring), "%%-%ds\n", (int) maxlen);
	};

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Format string: %s", printformatstring);

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
			fprintf(stderr, "\n Available output types filtered by input type:\n");
		} else {
			fprintf(stderr, "\n Available output types:\n");
		};
	};

	/* run through matrix */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Format-Row %d: %08x - %s - %s", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);

		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatmatrix); i++) {
			if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
				if (ipv6calc_formatmatrix[i][0] != inputtype) {
					/* skip */
					continue;
				};
			};
		
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Row %d: %08x - %08x", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);

			if ((ipv6calc_formatmatrix[i][1] & ipv6calc_formatstrings[j].number) != 0) {
				/* available for output, look for name now */
				if (strlen(ipv6calc_formatstrings[j].explanation) > 0) {
					fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				} else {
					fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token, "(empty)");
				};
				break;
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		fprintf(stderr, "\n For examples and available format options use:\n");
		fprintf(stderr, "    -O|--out <type> --examples\n");
		fprintf(stderr, "\n");
	};
};


/* list of action types */
void printhelp_actiontypes(const uint32_t formatoptions, const struct option longopts[]) {
	int i, j, o, test, has_options;
	size_t maxlen = 0;
	char printformatstring[20], printformatstring2[20], printformatstring3[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_actionstrings); j++) {
		if (strlen(ipv6calc_actionstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_actionstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_actionstrings'!\n");
		exit (EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		snprintf(printformatstring, sizeof(printformatstring), "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring), "%%-%ds\n", (int) maxlen);
	};

	snprintf(printformatstring2, sizeof(printformatstring2), "  %%-%ds%%s\n", (int) maxlen + 4);
	snprintf(printformatstring3, sizeof(printformatstring3), "  %%-%ds--%%s", (int) maxlen + 5);

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Action string: %s", printformatstring);

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available action types:\n");

	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_actionstrings); j++) {
		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Action-Row %d: %08x - %s - %s", j, (unsigned int) ipv6calc_actionstrings[j].number, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);

		if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
			fprintf(stderr, printformatstring, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);

			test = 2;
			has_options = 0;
			while (test != 0) {
				if (test == 1) {	
					fprintf(stderr, printformatstring2, "", "Options:");
				};

				/* search for defined options */
				for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_actionoptionmap); i++) {
					DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Option %d", i)

					if (ipv6calc_actionstrings[j].number == ipv6calc_actionoptionmap[i][0]) {
						if (ipv6calc_actionoptionmap[i][1] == 0) {
							/* no options supported */
							break;
						};
						
						DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Option value: %08x", (unsigned int) ipv6calc_actionoptionmap[i][1]);

						/* run through options */
						o = 0;
						while(longopts[o].name != NULL) {
							if ((ipv6calc_actionoptionmap[i][1] == (uint32_t) longopts[o].val)) {
								has_options = 1;
								if (test == 1) {	
									fprintf(stderr, printformatstring3, "", longopts[o].name);
									if (longopts[o].has_arg > 0) {
										fprintf(stderr, " ...");
									};
									if (ipv6calc_actionoptionmap[i][2] > 0) {
										fprintf(stderr, " (optional)");
									} else {
										fprintf(stderr, " (required)");
									};
									fprintf(stderr, "\n");
								};
							};
							o++;
						};
					};
				};
				if (has_options == 0) {
					break;
				};
				test--;
			};
		} else {
			fprintf(stdout, printformatstring, ipv6calc_actionstrings[j].token);
		};
	};
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* print global common help */
void printhelp_common(const uint32_t help_features) {
	fprintf(stderr, "\n");

	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "                                can also be set by IPV6CALC_DEBUG environment value\n");
	fprintf(stderr, "  [-v|--version [-v [-v]]]   : version information (2 optional detail levels)\n");
	fprintf(stderr, "  [-v|--version -h]          : explanation of feature tokens\n");
	fprintf(stderr, "  [-V|--verbose]             : be more verbose\n");
	fprintf(stderr, "  [-h|--help|-?]             : this online help\n");

	if ((help_features & IPV6CALC_HELP_QUIET) != 0) {
		fprintf(stderr, "  [-q|--quiet]               : be more quiet\n");
	};

	if ((help_features & IPV6CALC_HELP_IP2LOCATION) != 0) {
#ifdef SUPPORT_IP2LOCATION
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--disable-ip2location           ] : IP2Location support disabled\n");
		fprintf(stderr, "  [--db-ip2location-disable        ] : IP2Location support disabled\n");
		fprintf(stderr, "  [--db-ip2location-dir <directory>] : IP2Location database directory (default: %s)\n", ip2location_db_dir);
#ifdef SUPPORT_IP2LOCATION_DYN
		fprintf(stderr, "  [--db-ip2location-lib <file>     ] : IP2Location library file (default: %s)\n", ip2location_lib_file);
#endif
		fprintf(stderr, "  [--db-ip2location-only-type <TYPE>]: IP2Location database only selected type (1-%d)\n", IP2LOCATION_DB_MAX);
		fprintf(stderr, "  [--db-ip2location-allow-softlinks] : IP2Location database softlinks allowed\n");
		fprintf(stderr, "     by default they are ignored because it is hard to autodetect COMM/LITE/SAMPLE\n");
		fprintf(stderr, "  [--db-ip2location-lite-to-sample-autoswitch-max-delta-months <MONTHS>]:\n");
		fprintf(stderr, "     autoswitch from LITE to SAMPLE databases if possible and delta is not more than %d months (0=disabled)\n", ip2location_db_lite_to_sample_autoswitch_max_delta_months);
		fprintf(stderr, "  [--db-ip2location-comm-to-lite-switch-min-delta-months <MONTHS>]:\n");
		fprintf(stderr, "     switch from COMM to LITE databases if possible and delta more than %d months (0=disabled)\n", ip2location_db_comm_to_lite_switch_min_delta_months);
#endif
	};

	if ((help_features & IPV6CALC_HELP_GEOIP) != 0) {
#ifdef SUPPORT_GEOIP
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--disable-geoip                 ] : GeoIP support disabled\n");
		fprintf(stderr, "  [--db-geoip-disable              ] : GeoIP support disabled\n");
		fprintf(stderr, "  [--db-geoip-dir       <directory>] : GeoIP database directory (default: %s)\n", geoip_db_dir);
#ifdef SUPPORT_GEOIP_DYN
		fprintf(stderr, "  [--db-geoip-lib       <file>     ] : GeoIP library file (default: %s)\n", geoip_lib_file);
#endif
#endif
	};

	if ((help_features & IPV6CALC_HELP_DBIP) != 0) {
#ifdef SUPPORT_DBIP
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--disable-dbip                  ] : db-ip.com support disabled\n");
		fprintf(stderr, "  [--db-dbip-disable               ] : db-ip.com support disabled\n");
		fprintf(stderr, "  [--db-dbip-dir        <directory>] : db-ip.com database directory (default: %s)\n", dbip_db_dir);
		fprintf(stderr, "  [--db-dbip-only-type <TYPE>]       : db-ip.com database only selected type (1-%d)\n", DBIP_DB_MAX);
		fprintf(stderr, "  [--db-dbip-comm-to-free-switch-min-delta-months <MONTHS>]:\n");
		fprintf(stderr, "     switch from COMM to FREE databases if possible and delta more than %d months (0=disabled)\n", dbip_db_comm_to_free_switch_min_delta_months);
#endif
	};

	if ((help_features & IPV6CALC_HELP_EXTERNAL) != 0) {
#ifdef SUPPORT_EXTERNAL
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--disable-external              ] : External support disabled\n");
		fprintf(stderr, "  [--db-external-disable           ] : External support disabled\n");
		fprintf(stderr, "  [--db-external-dir    <directory>] : External database directory (default: %s)\n", external_db_dir);
#endif
	};

	if ((help_features & IPV6CALC_HELP_BUILTIN) != 0) {
#ifdef SUPPORT_BUILTIN
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--disable-builtin               ] : BuiltIn support disabled\n");
		fprintf(stderr, "  [--db-builtin-disable            ] : BuiltIn support disabled\n");
#endif

#if defined SUPPORT_EXTERNAL || defined SUPPORT_DBIP || defined SUPPORT_GEOIP || SUPPORT_IP2LOCATION
		fprintf(stderr, "\n");
		fprintf(stderr, "  [--db-priorization <entry1>[:...]] : Database priorization order list (overwrites default)\n");
		fprintf(stderr, "                                         colon separated:");
		int i;
		for (i = 0; i < MAXENTRIES_ARRAY(data_sources); i++) {
			fprintf(stderr, " %s", data_sources[i].shortname);
		};
		fprintf(stderr, "\n");
#endif
	};

	fprintf(stderr, "\n");
	return;
};

void printhelp_shortcut_options(const struct option longopts[], const s_ipv6calc_longopts_shortopts_map longopts_shortopts_map[]) {
	int i = 0;
	int j;
	char c;
	const char *info;
	
	fprintf(stderr, "\n");
	fprintf(stderr, " Usage with shortcut options: <shortcut option> [<format option> ...] <input data>\n");
	fprintf(stderr, "  for more information and available format options use: <shortcut option> -?|-h|--help\n");
	fprintf(stderr, "\n");

	while(longopts[i].name != NULL) {
		if (longopts[i].val >= CMD_shortcut_start && longopts[i].val <= CMD_shortcut_end) {
			c = '\0';
			info = NULL;

			if (longopts_shortopts_map != NULL) {
				DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Search in longopts_shortopts_map for %08x", longopts[i].val);
				j = 0;
				while (longopts_shortopts_map[j].val > 0) {
					DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Check against longopts_shortopts_map entry %d:%08x", j, longopts_shortopts_map[j].val);
					if (longopts[i].val == longopts_shortopts_map[j].val) {
						c = longopts_shortopts_map[j].c;
						info = longopts_shortopts_map[j].info;
						break;
					};
					j++;
				};
			};

			if (c != '\0') {
				fprintf(stderr, "  -%c|--%s", c, longopts[i].name);
			} else {
				fprintf(stderr, "     --%s", longopts[i].name);
			};

			if (info != NULL) {
				fprintf(stderr, " (%s)", info);
			};

			fprintf(stderr, "\n");
		};
		i++;
	};
	
	fprintf(stderr, "\n");
	return;
};


/* print help for output type examples */


static void printhelp_output_base85(void) {
	fprintf(stderr, " Print a given IPv6 address in base85 format (RFC 1924), e.g.\n");
	fprintf(stderr, "  1080:0:0:0:8:800:200c:417a -> 4)+k&C#VzJ4br>0wv%%Yp\n");
};

static void printhelp_output_bitstring(void) {
	fprintf(stderr, " Print a given IPv6 address as a bitstring label for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff::1    -> \\[x3ffeffff000000000000000000000001/128].ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff::1/64 -> \\[x3ffeffff000000000000000000000001/64].ip6.arpa.\n");
	fprintf(stderr, "  --printsuffix 3ffe:ffff::1/64 -> \\[x0000000000000001/64]\n");
	fprintf(stderr, "  --printprefix 3ffe:ffff::1/64 -> \\[x3ffeffff00000000/64]\n");
};

static void printhelp_output_ipv6addr(void) {
	fprintf(stderr, " Print a given IPv6 address depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1    -> 3ffe:ffff:100:f101:0:0:0:1\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 3ffe:ffff:100:f101:0:0:0:1/64\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffe:ffff:0100:f101:0000:0000:0000:0001\n");
	fprintf(stderr, "  Compressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:0100:f101:0000:0000:0000:0001 -> 3ffe:ffff:100:f101::1\n");
};

static void printhelp_output_ipv6literal(void) {
	fprintf(stderr, " Print a given IPv6 address in literal format depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8-0-0-0-0-0-1.ipv6-literal.net\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net\n");
	fprintf(stderr, "  Compressed (default), e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8--1.ipv6-literal.net\n");
	fprintf(stderr, "  With Scope ID, e.g.\n");
	fprintf(stderr, "   fe80::1%%0    -> fe80--1s0.ipv6-literal.net\n");
};

static void printhelp_output_hex(void) {
	fprintf(stderr, " Print a given IPv4/v6 address in hex format depending on format options:\n");
	fprintf(stderr, "  without any format option, e.g.\n");
	fprintf(stderr, "   0123:4567:89ab:cdef:0000:1111:2222:3333 -> 0123456789abcdef0000111122223333\n");
	fprintf(stderr, "   1.2.3.4                                 -> 01020304\n");
};

static void printhelp_output_eui64(void) {
	fprintf(stderr, " Print a generated EUI-64 identifier, e.g.:\n");
	fprintf(stderr, "  00:50:BF:06:B4:F5 -> 0250:bfff:fe06:b4f5\n");
};

static void printhelp_output_iid_token(void) {
	fprintf(stderr, " Print generated interface identifier and token, e.g.:\n");
	fprintf(stderr, "  -> 4462:bdea:8654:776d 486072ff7074945e\n");
};

static void printhelp_output_revnibble_int(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
};

static void printhelp_output_revnibble_arpa(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
};

static void printhelp_output_ifinet6void(void) {
	fprintf(stderr, " Print a given IPv6 address to same format shown in Linux /proc/net/if_inet6:\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1    -> 3ffeffff0100f1010000000000000001 00\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64 -> 3ffeffff0100f1010000000000000001 00 40\n");
};

static void printhelp_output_ipv4addr(void) {
	fprintf(stderr, " Print an IPv4 address\n");
};

static void printhelp_output_revipv4(void) {
	fprintf(stderr, " Print an IPv4 address in reverse format for PTR/DNS\n");
	fprintf(stderr, "  1.2.3.4  -> 4.3.2.1.in-addr.arpa\n");
};

static void printhelp_output_addrtype(void) {
	fprintf(stderr, " Print type of a given IPv4/IPv6 address:\n");
	fprintf(stderr, "  IPv4 address  -> ipv4-addr.addrtype.ipv6calc\n");
	fprintf(stderr, "  IPv6 address  -> ipv6-addr.addrtype.ipv6calc\n");
};

static void printhelp_output_ipv6addrtype(void) {
	fprintf(stderr, " Print type of a given IPv6 address:\n");
	fprintf(stderr, "  3ffe::/16         -> 6bone-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2002::/16         -> 6to4-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2001::/16         -> productive-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fe80::/10         -> link-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fec0::/10         -> site-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::ffff:0:0:0:0/96 -> mapped-ipv4.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::0:0:0:0/96      -> compat-ipv4.ipv6addrtype.ipv6calc\n");
};

static void printhelp_output_ouitype(void) {
	fprintf(stderr, " Print OUI name of a given IPv6 address:\n");
	fprintf(stderr, "  IID local scope   -> local-scoppe.ouitype.ipv6calc\n");
	fprintf(stderr, "  IID global scope  -> vendorname.ouitype.ipv6calc\n");
};

static void printhelp_output_octal(void) {
	fprintf(stderr, " Print octal representation of a given IPv6 address:\n");
	fprintf(stderr, "  (useful for djbdns/tinydns)\n");
	fprintf(stderr, "  3ffe:ffff::1 ->\n    \\77\\376\\377\\377\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\1\n");
	fprintf(stderr, "  --printfulluncompressed 3ffe:ffff::1 ->\n    \\077\\376\\377\\377\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\001\n");
};

void printhelp_output_dispatcher(const uint32_t outputtype) {
	int i, j;

	printversion();
	
	fprintf(stderr, "\n");

	switch (outputtype) {
		case FORMAT_base85:
			printhelp_output_base85();
			break;

		case FORMAT_bitstring:
			printhelp_output_bitstring();
			break;
			
		case FORMAT_ipv6addr:
			printhelp_output_ipv6addr();
			break;
			
		case FORMAT_ipv6literal:
			printhelp_output_ipv6literal();
			break;
			
		case FORMAT_eui64:
			printhelp_output_eui64();
			break;

		case FORMAT_revnibbles_int:
			printhelp_output_revnibble_int();
			break;

		case FORMAT_revnibbles_arpa:
			printhelp_output_revnibble_arpa();
			break;

		case FORMAT_ifinet6:
			printhelp_output_ifinet6void();
			break;
			
		case FORMAT_iid_token:
			printhelp_output_iid_token();
			break;

		case FORMAT_ipv4addr:
			printhelp_output_ipv4addr();
			break;

		case FORMAT_addrtype:
			printhelp_output_addrtype();
			break;

		case FORMAT_ipv6addrtype:
			printhelp_output_ipv6addrtype();
			break;
			
		case FORMAT_ouitype:
			printhelp_output_ouitype();
			break;
			
		case FORMAT_revipv4:
			printhelp_output_revipv4();
			break;
			
		case FORMAT_octal:
			printhelp_output_octal();
			break;

		case FORMAT_hex:
			printhelp_output_hex();
			break;
			
		default:
			fprintf(stderr, " Examples currently missing...!\n");
			break;
	};
	
	/* looking for outtype */
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_outputformatoptionmap); i++) {
		if (outputtype == ipv6calc_outputformatoptionmap[i][0]) {
			if (ipv6calc_outputformatoptionmap[i][1] == 0) {
				fprintf(stderr, " No format options supported\n");
				break;
			};
			
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Format value: %08x", (unsigned int) ipv6calc_outputformatoptionmap[i][1]);
			
			fprintf(stderr, "\n");
			fprintf(stderr, " Available format options:\n");

			/* run through format options */
			for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatoptionstrings); j++) {
				if ((ipv6calc_outputformatoptionmap[i][1] & ipv6calc_formatoptionstrings[j].number) != 0) {
					fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
				};
			};
			break;
		};
	};
	fprintf(stderr, "\n");
};

/* help regarding action command */
/* in: embedded = 1 : do not show version and command */
void printhelp_action_dispatcher(const uint32_t action, const int embedded) {
	int i, j;
	char method_name[32];

	if (embedded != 1) {
		printversion();
	};
	
	fprintf(stderr, "\n");

	switch (action) {
		case ACTION_mac_to_eui64:
			fprintf(stderr, " help still missing - sorry.\n");
			break;
			
		case ACTION_ipv4_to_6to4addr:
			fprintf(stderr, "  Convert IPv4 to 6to4 prefix / extract IPv4 from 6to4 prefix\n");
			fprintf(stderr, "   ipv6calc -A conv6to4 192.0.2.1\n");
			fprintf(stderr, "    2002:c000:201::\n");
			fprintf(stderr, "   ipv6calc -A conv6to4 2002:c000:201::\n");
			fprintf(stderr, "    192.0.2.1\n");
			break;

		case ACTION_ipv4_to_nat64:
			fprintf(stderr, "  Convert IPv4 to NAT64 address / extract IPv4 from NAT64 address\n");
			fprintf(stderr, "   ipv6calc -A convnat64 192.0.2.1\n");
			fprintf(stderr, "    64:ff9b::c000:201\n");
			fprintf(stderr, "   ipv6calc -A convnat64 64:ff9b::c000:201\n");
			fprintf(stderr, "    192.0.2.1\n");
			break;

		case ACTION_anonymize:
			if (embedded != 1) {
				fprintf(stderr, " Anonymize given address according to preset or custom values, e.g.\n");
				fprintf(stderr, "  ipv6calc -A anonymize 2001:db8:2280:6901:224:21ff:fe01:2345 --anonymize-preset zeroize-standard\n");
				fprintf(stderr, "   2001:db8:2280:6900:224:21ff:fe00:0\n");
				fprintf(stderr, "  ipv6calc -A anonymize 2001:db8:2280:6901:224:21ff:fe01:2345 --anonymize-preset anonymize-standard\n");
				fprintf(stderr, "   2001:db8:2280:6909:a929:4291:4022:4217\n");
				fprintf(stderr, "\n");
			};

			fprintf(stderr, "  Shortcut for anonymization presets:\n");
			fprintf(stderr, "   --anonymize-standard (default)\n");
			fprintf(stderr, "   --anonymize-careful\n");
			fprintf(stderr, "   --anonymize-paranoid\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  Supported methods [--anonymize-method METHOD]:\n");
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_anon_methods); i++) {
				fprintf(stderr, "   %-10s: %s\n", ipv6calc_anon_methods[i].name, ipv6calc_anon_methods[i].description);
			};
			fprintf(stderr, "\n");

			fprintf(stderr, "  Available presets (shortcut names) [--anonymize-preset|ap PRESET-NAME]:\n");

			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_anon_set_list); i++) {
				snprintf(method_name, sizeof(method_name), "%s", "unknown"); // default

				for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_anon_methods); j++) {
					if (ipv6calc_anon_methods[j].method == ipv6calc_anon_set_list[i].method) {
						snprintf(method_name, sizeof(method_name), "%s", ipv6calc_anon_methods[j].name);
						break;
					};
				};

				fprintf(stderr, "   %-20s (%2s): mask-ipv6=%3d mask-ipv4=%2d mask-eui64=%2d mask-mac=%2d mask-autoadjust=%-3s method=%s\n", ipv6calc_anon_set_list[i].name, ipv6calc_anon_set_list[i].name_short, ipv6calc_anon_set_list[i].mask_ipv6, ipv6calc_anon_set_list[i].mask_ipv4, ipv6calc_anon_set_list[i].mask_eui64, ipv6calc_anon_set_list[i].mask_mac, (ipv6calc_anon_set_list[i].mask_autoadjust == 1) ? "yes" : "no", method_name);
			};
			fprintf(stderr, "\n");

			fprintf(stderr, "  Custom control:\n");
			fprintf(stderr, "  --mask-ipv4  <bits>     : mask IPv4 address [0-32] (even if occurs in IPv6 address)\n");
			fprintf(stderr, "  --mask-ipv6  <bits>     : mask IPv6 prefix [0-64] (only applied to related address types)\n");
			fprintf(stderr, "  --mask-eui64 <bits>     : mask EUI-64 address or IPv6 interface identifier [0-64]\n");
			fprintf(stderr, "  --mask-mac   <bits>     : mask MAC address [0-48]\n");
			fprintf(stderr, "  --mask-autoadjust yes|no: autoadjust mask to keep type/vendor information regardless of less given mask\n");

			break;
			
		case ACTION_iid_token_to_privacy:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_prefix_mac_to_ipv6:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_6rd_local_prefix:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_filter:
			fprintf(stderr, " Filter given addresses from stdin by filter expression, e.g.\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E iid-local\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E iid-local,global-unicast\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E ^iid-random\n");
			fprintf(stderr, "\n");
			fprintf(stderr, " (note: since version 0.95.0 '-A filter' is autoselected if option '-E <filter expression>' is given)\n");
			fprintf(stderr, "\n");
			fprintf(stderr, " negation of filter expression with prefix: ^\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv6 address filter tokens:\n");
			fprintf(stderr, "   ipv6 ");
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
				fprintf(stderr, " %s", ipv6calc_ipv6addrtypestrings[i].token);
			};
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv4 address filter tokens:\n");
			fprintf(stderr, "   ipv4 ");
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv4addrtypestrings); i++ ) {
				fprintf(stderr, " %s", ipv6calc_ipv4addrtypestrings[i].token);
			};
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv4/v6 address filter tokens based on databases:\n");
			fprintf(stderr, "   [^][ipv4.|ipv6.]db.cc=<CC>|unknown (Country Code [2 chars])\n");
			fprintf(stderr, "   [^][ipv4.|ipv6.]db.asn=<ASN>|unknown (Autonomous System Number)\n");
			fprintf(stderr, "   [^][ipv4.|ipv6.]db.reg=<REGISTRY>\n");
			fprintf(stderr, "    Registry tokens:");
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_registries); i++ ) {
				fprintf(stderr, " %s", ipv6calc_registries[i].token);
			};
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv4/v6 address filter tokens based on address/mask:\n");
			fprintf(stderr, "   [^]ipv4.addr=<IPV4-ADDRESS>[<PREFIX-LENGTH>]\n");
			fprintf(stderr, "   [^]ipv6.addr=<IPV6-ADDRESS>[<PREFIX-LENGTH>]\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv4/v6 address filter tokens based on address ranges (<=|<|>|>=):\n");
			fprintf(stderr, "   [^]ipv4.addr(<=|<|>|>=)<IPV4-ADDRESS>\n");
			fprintf(stderr, "   [^]ipv6.addr(<=|<|>|>=)<IPV6-ADDRESS>\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "   as alternative in case <|> creating problems also supported: =(le|lt|gt|ge)=:\n");
			fprintf(stderr, "   [^]ipv4.addr=(le|lt|gt|ge)=<IPV4-ADDRESS>\n");
			fprintf(stderr, "   [^]ipv6.addr=(le|lt|gt|ge)=<IPV6-ADDRESS>\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  EUI-48/MAC address filter tokens:\n");
			fprintf(stderr, "   ");
			fprintf(stderr, " IMPLEMENTATION MISSING");
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  EUI-64 address filter tokens:\n");
			fprintf(stderr, "   ");
			fprintf(stderr, " IMPLEMENTATION MISSING");
			fprintf(stderr, "\n");
			break;

		case ACTION_test:
			fprintf(stderr, " Test given address(es) against supported tests:\n");
			fprintf(stderr, "  --test_prefix <PREFIX>       : inside a prefix\n");
			fprintf(stderr, "  --test_gt|--test_ge <ADDRESS>: greater(/equal) than an address\n");
			fprintf(stderr, "  --test_lt|--test_le <ADDRESS>: less(/equal) than an address\n");
			fprintf(stderr, "\n");
			fprintf(stderr, " Test given address(es) from stdin, e.g.\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A test] --test_prefix 2001:db8::/32\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A test] --test_prefix 2001:db9::/32\n");
			fprintf(stderr, "\n");
			fprintf(stderr, " Single address test, return code: 0=inside/matching 1=outside/not-matching 2=uncomparable\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_prefix 2001:db8::/32 2001:db8::1\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_prefix 2001:db9::/32 2001:db8::1\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_ge 2001:db8:: --test_le 2001:db8:ffff:ffff:ffff:ffff:ffff:ffff 2001:db8::1\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_ge 2001:db9:: --test_le 2001:db9:ffff:ffff:ffff:ffff:ffff:ffff 2001:db8::1\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_ge 2001:db9:: --test_lt 2001:dba:: 2001:db8::1\n");
			fprintf(stderr, "  ipv6calc [-A test] --test_ge 2001:db8:: --test_lt 2001:db9:: 2001:db8::1\n");
			fprintf(stderr, "\n");
			break;
	};
};


/***************************
 * verbose feature information
 * *************************/

/* display features in verbose mode */
void ipv6calc_print_features_verbose(const int level_verbose) {
	char string[NI_MAXHOST] = "";

	libipv6calc_db_wrapper_capabilities(string, sizeof(string));
	fprintf(stderr, "Capabilities: %s\n\n", string);

	fprintf(stderr, "Internal main     library version: %s  API: %s  (%s)\n"
		, libipv6calc_lib_version_string()
		, libipv6calc_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

	fprintf(stderr, "Internal database library version: %s  API: %s  (%s)\n\n"
		, libipv6calc_db_lib_version_string()
		, libipv6calc_db_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

#if defined ENABLE_BUNDLED_MD5 || defined ENABLE_BUNDLED_GETOPT
	fprintf(stderr, "Bundled with:");
#ifdef ENABLE_BUNDLED_MD5
	fprintf(stderr, " MD5");
#endif
#ifdef ENABLE_BUNDLED_GETOPT
	fprintf(stderr, " GETOPT");
#endif
	fprintf(stderr, "\n\n");
#endif

	fprintf(stderr, "Compiled: %s  %s\n\n", __DATE__, __TIME__);


#ifdef SUPPORT_GEOIP
#ifdef GEOIP_INCLUDE_VERSION
	fprintf(stderr, "GeoIP support enabled, compiled with include file version: %s\n", GEOIP_INCLUDE_VERSION);
#endif
#ifndef SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_V6
#if defined (SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6) && defined (SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6)
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support\n");
#else
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support (in compatibility mode)\n");
#endif
#else // SUPPORT_GEOIP_V6
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 support only\n");
#endif // SUPPORT_GEOIP_V6
#ifdef SUPPORT_GEOIP_LIB_VERSION
	fprintf(stderr, "GeoIP dynamic library version (on this system): %s\n", libipv6calc_db_wrapper_GeoIP_lib_version());
#else
	fprintf(stderr, "GeoIP dynamic library version (on this system): compiled without detection\n");
#endif
#else // SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, "GeoIP support by dynamic library load, compiled with IPv4 & IPv6 support\n");
#else  // SUPPORT_GEOIP_V6
	fprintf(stderr, "GeoIP support by dynamic library load, compiled with IPv4 support only\n");
#endif // SUPPORT_GEOIP_V6
	fprintf(stderr, "GeoIP configured dynamic library file and detected version: %s %s\n", geoip_lib_file, libipv6calc_db_wrapper_GeoIP_lib_version());
#endif // SUPPORT_GEOIP_DYN

	//TODO: list of GeoIP files
#ifdef SUPPORT_GEOIP_V6
	//TODO: list of GeoIP files
#endif
	libipv6calc_db_wrapper_GeoIP_wrapper_info(string, sizeof(string));
	fprintf(stderr, "%s\n\n", string);
#else
	fprintf(stderr, "GeoIP support not compiled-in\n\n");
#endif

#ifdef SUPPORT_IP2LOCATION
#ifdef IP2LOCATION_INCLUDE_VERSION
	fprintf(stderr, "IP2Location support enabled, compiled with include file version: %s\n", IP2LOCATION_INCLUDE_VERSION);
#endif
#if SUPPORT_IP2LOCATION_API_VERSION_STRING
	fprintf(stderr, "IP2Location support enabled, compiled with API version: %s, dynamically linked with version: %s\n", xmakestr(API_VERSION), libipv6calc_db_wrapper_IP2Location_lib_version());
#else
	fprintf(stderr, "IP2Location support enabled, compiled with API version: %s (retrieving linked version not supported)\n", xmakestr(API_VERSION));
#endif // SUPPORT_IP2LOCATION_API_VERSION_STRING
#ifndef SUPPORT_IP2LOCATION_DYN
#else
	fprintf(stderr, "IP2Location support by dynamic library load\n");
	fprintf(stderr, "IP2Location configured dynamic library file and detected version: %s %s\n", ip2location_lib_file, libipv6calc_db_wrapper_IP2Location_lib_version());
#endif
	// TODO: show base directory
	/*
	if (file_ip2location_ipv4 != NULL && strlen(file_ip2location_ipv4) > 0) {
		fprintf(stderr, "IP2Location IPv4 default file: %s\n", file_ip2location_ipv4);
	} else {
		fprintf(stderr, "IP2Location IPv4 default file: not configured\n");
	};
	if (file_ip2location_ipv6 != NULL && strlen(file_ip2location_ipv6) > 0) {
		fprintf(stderr, "IP2Location IPv6 default file: %s\n", file_ip2location_ipv6);
	} else {
		fprintf(stderr, "IP2Location IPv6 default file: not configured\n");
	};
	*/
	libipv6calc_db_wrapper_IP2Location_wrapper_info(string, sizeof(string));
	fprintf(stderr, "%s\n\n", string);
#else
	fprintf(stderr, "IP2Location support not compiled-in\n\n");
#endif

#ifdef SUPPORT_BUILTIN
	libipv6calc_db_wrapper_BuiltIn_wrapper_info(string, sizeof(string));
	fprintf(stderr, "%s\n\n", string);
#else
	fprintf(stderr, "BuiltIn support not compiled-in\n\n");
#endif

	libipv6calc_db_wrapper_print_db_info(level_verbose, "");
};

