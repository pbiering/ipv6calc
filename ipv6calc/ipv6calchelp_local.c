/*
 * Project    : ipv6calc/ipv6calc
 * File       : ipv6calchelp_local.c
 * Version    : $Id: ipv6calchelp_local.c,v 1.1 2013/10/24 19:29:03 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calchelp.h"
#include "config.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

// Note: part of help is still located at ../lib/ipv6calchelp.c

/* display info */
void printversion(void) {
	char resultstring[NI_MAXHOST] = "";

	libipv6calc_db_wrapper_features(resultstring, sizeof(resultstring));

	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, " IP2Location");
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, " GeoIP");
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, " GeoIPv6");
#endif
#endif

	fprintf(stderr, " %s\n", resultstring);
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
	fprintf(stderr, "See '%s -?|-h|--help or -v -v' for more details\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6calc_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");

	fprintf(stderr, "This program formats and calculates IPv6 addresses and can do many more tricky things\n");

	fprintf(stderr, "\n");

	printhelp_common();

	fprintf(stderr, "\n");

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
	fprintf(stderr, " Other usage:\n");
	fprintf(stderr, "  --showinfo|-i [--machine_readable|-m] : show information about input data\n");
	fprintf(stderr, "  --showinfo|-i --show_types            : show available types on '-m'\n");

	fprintf(stderr, "\n");
	fprintf(stderr, " To see old-style option use: --printoldoptions\n");
	fprintf(stderr, "\n");
	return;
};

