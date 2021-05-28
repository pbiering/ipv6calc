/*
 * Project    : ipv6calc/ipv6calc
 * File       : ipv6calchelp_local.c
 * Version    : $Id$
 * Copyright  : 2013-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"
#include "config.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

// Note: part of help is still located at ../lib/ipv6calchelp.c

/* display info */
void printversion(void) {
	char resultstring[IPV6CALC_STRING_MAX] = "";

	libipv6calc_db_wrapper_features(resultstring, sizeof(resultstring));

	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);

	fprintf(stderr, " %s", resultstring);

	if (feature_zeroize == 1) {
		fprintf(stderr, " ANON_ZEROISE");
	};

	if (feature_anon == 1) {
		fprintf(stderr, " ANON_ANONYMIZE");
	};

	if (feature_kp == 1) {
		fprintf(stderr, " ANON_KEEP-TYPE-ASN-CC");
	};

	if (feature_kg == 1) {
		fprintf(stderr, " ANON_KEEP-TYPE-GEONAMEID");
	};

	fprintf(stderr, "\n");
};

void printversion_help(void) {
	char resultstring[IPV6CALC_STRING_MAX] = "";

	libipv6calc_db_wrapper_features(resultstring, sizeof(resultstring));

	fprintf(stderr, "\n");

	fprintf(stderr, "Explanation of available(x)/possible(o) feature tokens\n");

	libipv6calc_db_wrapper_features_help();

	fprintf(stderr, "%-25s%c %s\n", "ANON_ZEROISE", (feature_zeroize == 1)? 'x' : 'o', "Anonymization method 'zeroize'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_ANONYMIZE", (feature_anon == 1) ? 'x' : 'o', "Anonymization method 'anonymize'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_KEEP-TYPE-ASN-CC", (feature_kp == 1) ? 'x' : 'o', "Anonymization method 'keep-type-asn-cc'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_KEEP-TYPE-GEONAMEID", (feature_kg == 1) ? 'x' : 'o', "Anonymization method 'keep-type-geonameid'");

	fprintf(stderr, "\n");
};

void printversion_verbose(int level_verbose) {
	printversion();
	fprintf(stderr, "\n");

	ipv6calc_print_features_verbose(level_verbose);
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6calc_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program formats and calculates IPv6 addresses and can do many more tricky things\n");
	fprintf(stderr, "See '%s -?|-h|--help for online help\n", PROGRAM_NAME);
	fprintf(stderr, "See '%s -v|--version -?|-h|--help' for explanation of feature tokens\n", PROGRAM_NAME);
	fprintf(stderr, "See '%s -vv or -vvv for more internal version/feature information\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6calc_printhelp(const struct option longopts[], const s_ipv6calc_longopts_shortopts_map longopts_shortopts_map[]) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");

	fprintf(stderr, "This program formats and calculates IPv6/IPv4/MAC addresses and can do many more tricky things\n");

	printhelp_common(IPV6CALC_HELP_ALL & (~(IPV6CALC_HELP_QUIET)));

	fprintf(stderr, "  [-q|--quiet]               : be more quiet (auto-enabled in pipe mode)\n");
	fprintf(stderr, "  [-f|--flush]               : flush each line in pipe mode\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Usage with new style options:\n");
	fprintf(stderr, "  [--in|-I <input type>]   : specify input  type\n");
	fprintf(stderr, "                             (default: autodetect)\n");
	fprintf(stderr, "  [--out|-O <output type>] : specify output type\n");
	fprintf(stderr, "                             (sometimes: autodetect)\n");
	fprintf(stderr, "  [--action|-A <action>]   : specify action\n");
	fprintf(stderr, "                             (default: format conversion, sometimes: autodetect)\n");
	fprintf(stderr, "  [<format option> ...] : specify format options\n");
	fprintf(stderr, "  <input data> [...]    : input data\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Available input  types:  [-m] -I|--in     -?|-h|--help\n");
	fprintf(stderr, "  Available output types:  [-m] -O|--out    -?|-h|--help\n");
	fprintf(stderr, "  Available action types:  [-m] -A|--action -?|-h|--help\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Special filter action :  -E -?|-h|--help\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Other usage:\n");
	fprintf(stderr, "  --showinfo|-i [--machine_readable|-m] : show information about input data\n");
	fprintf(stderr, "  --showinfo|-i --mrqva                 : Machine Readable Quote Values Always\n");
	fprintf(stderr, "  --showinfo|-i --mrqvn                 : Machine Readable Quote Values Never\n");
	fprintf(stderr, "  --showinfo|-i --mrst <TOKEN>          : Machine Readable Select Token\n");
	fprintf(stderr, "  --showinfo|-i --mrstpa <TOKEN>        : Machine Readable Select Token Print Always\n");
	fprintf(stderr, "  --showinfo|-i --mrmt <TOKENPREFIX>    : Machine Readable Match Token Prefix\n");
	fprintf(stderr, "  --showinfo|-i --mrmts <TOKENSUFFIX>   : Machine Readable Match Token Suffix\n");
	fprintf(stderr, "  --showinfo|-i --mrtvo <TOKEN>         : Machine Readable Token Value Only\n");
	fprintf(stderr, "  --showinfo|-i --show-tokens           : show available tokens (aka types) on '-m'\n");

	printhelp_shortcut_options(longopts, longopts_shortopts_map);

	fprintf(stderr, "\n");
	return;
};

