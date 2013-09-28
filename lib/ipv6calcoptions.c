/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.c
 * Version    : $Id: ipv6calcoptions.c,v 1.5 2013/09/28 20:32:40 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  supporting common options
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calcoptions.h"
#include "ipv6calcoptions_common.h"

#include "databases/lib/libipv6calc_db_wrapper.h"

extern long int ipv6calc_debug;
int ipv6calc_quiet;


/* parse value */
static long int parse_dec_hex_val(const char *string) {
	long int value;

	if ((strlen(string) > 2) && ((strncmp(string, "0x", 2) == 0) || (strncmp(string, "0X", 2)) == 0)) {
		// convert hex
		if (sscanf(string + 2, "%lx", &value) == 0) {
			ipv6calc_debug = 0;
			fprintf(stderr, "%s/%s: can't parse value for debug option: %s\n", __FILE__, __func__, string);
		};
	} else {
		// convert dec
		if (sscanf(string, "%ld", &value) == 0) {
			ipv6calc_debug = 0;
			fprintf(stderr, "%s/%s: can't parse value for debug option: %s\n", __FILE__, __func__, string);
		};
	};

	return(value);
};


/* bootstrap debug option from environment */
void ipv6calc_debug_from_env(void) {
	char *ipv6calc_debug_env;
	long int ipv6calc_debug_val;

        ipv6calc_debug_env = getenv("IPV6CALC_DEBUG");

	if (ipv6calc_debug_env != 0) {
		fprintf(stderr, "%s/%s: IPV6CALC_DEBUG found in environment: %s\n", __FILE__, __func__, ipv6calc_debug_env);

		ipv6calc_debug_val = parse_dec_hex_val(ipv6calc_debug_env);

		if (ipv6calc_debug_val != 0) {
			ipv6calc_debug = ipv6calc_debug_val;
			fprintf(stderr, "%s/%s: IPV6CALC_DEBUG proper parsed: %08lx\n", __FILE__, __func__, ipv6calc_debug);
		};
	};
};


/* 
 * add common options
 * hard exit code in case of troubles: 2
 */
static void ipv6calc_options_common_add(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p) {
	int i, j, k;
	char tempstring[NI_MAXHOST];

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Called\n", __FILE__, __func__);

	if (*maxentries_p + MAXENTRIES_ARRAY(ipv6calc_longopts_common) >= (MAXLONGOPTIONS - 1)) {
		fprintf(stderr, "FATAL error, can't add ipv6calc_options_common options - FIX CODE by increasing MAXLONGOPTIONS\n");
		exit(2);
	};

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_longopts_common); i++) {
		/* check for duplicates */
		for (j = 0; j < *maxentries_p; j++) {
			if (strcmp(longopts[j].name, ipv6calc_longopts_common[i].name) == 0) {
				fprintf(stderr, "FATAL error, can't add ipv6calc_options_common options - DUPLICATE NAME FOUND: %s\n", longopts[j].name);
				exit(2);
			};
		};

		DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Add common long option on position %d/%d: %s\n", __FILE__, __func__, *maxentries_p, i, ipv6calc_longopts_common[i].name);

		longopts[*maxentries_p] = ipv6calc_longopts_common[i];
		(*maxentries_p)++;
	};

	/* check for duplicates */
	for (j = 0; j < strlen(shortopts_p); j++) {
		for (k = 0; k < strlen(ipv6calc_shortopts_common); k++) {
			if (ipv6calc_shortopts_common[k] == ':') {
				continue;
			};
			if (shortopts_p[j] == ipv6calc_shortopts_common[k]) {
				fprintf(stderr, "FATAL error, can't add ipv6calc_options_common short options - DUPLICATE CHAR FOUND: %c\n", shortopts_p[j]);
				exit(2);
			};
		};
	};

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Add common short options: %s\n", __FILE__, __func__, ipv6calc_shortopts_common);

	snprintf(tempstring, sizeof(tempstring), "%s%s", shortopts_p, ipv6calc_shortopts_common);
	snprintf(shortopts_p, shortopts_maxlen, "%s", tempstring);

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Resulting short options: %s\n", __FILE__, __func__, shortopts_p);

	/* TODO: implement strlen checks */

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Return\n", __FILE__, __func__);

	return;
};


/* 
 * add options
 * hard exit code in case of troubles: 2
 */
void ipv6calc_options_add(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p, const char *shortopts_custom, const struct option longopts_custom[], const int longopts_custom_entries) {
	int i, j, k;
	char tempstring[NI_MAXHOST];

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Called\n", __FILE__, __func__);

	if (*maxentries_p + MAXENTRIES_ARRAY(longopts_custom) >= (MAXLONGOPTIONS - 1)) {
		fprintf(stderr, "FATAL error, can't add custom options - FIX CODE by increasing MAXLONGOPTIONS\n");
		exit(2);
	};

	for (i = 0; i < longopts_custom_entries; i++) {
		/* check for duplicates */
		for (j = 0; j < *maxentries_p; j++) {
			if (strcmp(longopts[j].name, longopts_custom[i].name) == 0) {
				fprintf(stderr, "FATAL error, can't add ipv6calc_options_common options - DUPLICATE NAME FOUND: %s\n", longopts[j].name);
				exit(2);
			};
		};

		DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Add custom long option on position %d/%d: %s\n", __FILE__, __func__, *maxentries_p, i, longopts_custom[i].name);

		longopts[*maxentries_p] = longopts_custom[i];
		(*maxentries_p)++;
	};

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Add custom short options: %s\n", __FILE__, __func__, shortopts_custom);

	/* check for duplicates */
	if (strlen(shortopts_p) > 0) {
		DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Already given short options: %s\n", __FILE__, __func__, shortopts_p);
		for (j = 0; j < strlen(shortopts_p); j++) {
			for (k = 0; k < strlen(shortopts_custom); k++) {
				if (shortopts_custom[k] == ':') {
					continue;
				};
				if (shortopts_p[j] == shortopts_custom[k]) {
					fprintf(stderr, "FATAL error, can't add ipv6calc_options_common short options - DUPLICATE CHAR FOUND: %c\n", shortopts_p[j]);
					exit(2);
				};
			};
		};
	} else {
		DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Already given short options: (empty)\n", __FILE__, __func__);
	};

	snprintf(tempstring, sizeof(tempstring), "%s%s", shortopts_p, shortopts_custom);
	snprintf(shortopts_p, shortopts_maxlen, "%s", tempstring);

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Resulting short options: %s\n", __FILE__, __func__, shortopts_p);

	ipv6calc_options_common_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p); 

	// end of options
	longopts[*maxentries_p].name    = NULL;
	longopts[*maxentries_p].has_arg = 0;
	longopts[*maxentries_p].flag    = NULL;
	longopts[*maxentries_p].val     = 0;

	/* TODO: implement strlen checks */

	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Return");

	return;
};


/* 
 * call further option handlers
 * return: 0: option found
 */
int ipv6calcoptions(const int opt, const char *optarg, const struct option longopts[]) {
	int result = -1;

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Called opt=0x%08x\n", __FILE__, __func__, opt);

	/* general options */
	switch(opt) {
		case 'd':
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Found debug option with value: %s", optarg);
			ipv6calc_debug = parse_dec_hex_val(optarg);
			fprintf(stderr, "%s/%s: given debug value: %lx\n", __FILE__, __func__, ipv6calc_debug);
			result = 0;
			break;

		case 'q':
			DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Found quiet option");
			ipv6calc_quiet = 1;
			result = 0;
			break;
		default:
			/* jump to other parsers */
			DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Call sub-parser for opt=0x%08x\n", __FILE__, __func__, opt);

			result = libipv6calc_db_wrapper_options(opt, optarg, longopts);
	};

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Return with result: %d\n", __FILE__, __func__, result);

	return(result);
};


/* get option name for an option
 * in : opt
 * ret: char* to string
 */
const char *ipv6calcoption_name(const int opt, const struct option longopts[]) {
	int i = 0;

	DEBUGPRINT(DEBUG_ipv6calcoptions, "%s/%s: Called opt=%d\n", __FILE__, __func__, opt);

	while (longopts[i].name != NULL) {
		if (opt == longopts[i].val) {
			return(longopts[i].name);
		};
		i++;
	};

	return("UNKNOWN");
};
