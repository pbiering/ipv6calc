/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.c
 * Version    : $Id$
 * Copyright  : 2013-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  supporting common options
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calcoptions.h"
#include "ipv6calcoptions_common.h"

#include "libipv6calc_db_wrapper.h"

extern long int ipv6calc_debug; // ipv6calc_debug usage ok
int ipv6calc_quiet = 0;
int ipv6calc_verbose = 0;


/* parse value */
static long int parse_dec_hex_val(const char *string) {
	long int value;

	if ((strlen(string) > 2) && ((strncmp(string, "0x", 2) == 0) || (strncmp(string, "0X", 2)) == 0)) {
		// convert hex
		if (sscanf(string + 2, "%lx", &value) == 0) {
			ipv6calc_debug = 0; // ipv6calc_debug usage ok
			ERRORPRINT_WA("can't parse value for debug option: %s", string);
		};
	} else {
		// convert dec
		if (sscanf(string, "%ld", &value) == 0) {
			ipv6calc_debug = 0; // ipv6calc_debug usage ok
			ERRORPRINT_WA("can't parse value for debug option: %s", string);
		};
	};

	return(value);
};


/* bootstrap debug option from environment */
void ipv6calc_debug_from_env(void) {
	char *ipv6calc_debug_env;
	long int ipv6calc_debug_val;

        ipv6calc_debug_env = getenv("IPV6CALC_DEBUG");

	if (ipv6calc_debug_env != NULL) {
		ERRORPRINT_WA("IPV6CALC_DEBUG found in environment: %s", ipv6calc_debug_env);

		ipv6calc_debug_val = parse_dec_hex_val(ipv6calc_debug_env);

		if (ipv6calc_debug_val != 0) {
			ipv6calc_debug = ipv6calc_debug_val; // ipv6calc_debug usage ok
			ERRORPRINT_WA("IPV6CALC_DEBUG proper parsed: %08lx", ipv6calc_debug); // ipv6calc_debug usage ok
		};
	};
};


/* 
 * add given options
 * hard exit code in case of troubles: 2
 */
void ipv6calc_options_add(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p, const char *shortopts_custom, const struct option longopts_custom[], const int longopts_custom_entries) {
	int i, l;
	unsigned int j, k;
	char tempstring[IPV6CALC_STRING_MAX];

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Called: longopts_custom_entries=%d shortopts=%s", longopts_custom_entries, shortopts_p);

	if (*maxentries_p + longopts_custom_entries >= (IPV6CALC_MAXLONGOPTIONS - 1)) {
		fprintf(stderr, "FATAL error, can't add options - FIX CODE by increasing IPV6CALC_MAXLONGOPTIONS\n");
		exit(2);
	};

	for (i = 0; i < longopts_custom_entries; i++) {
		/* check for duplicates */
		for (l = 0; l < *maxentries_p; l++) {
			if (strcmp(longopts[l].name, longopts_custom[i].name) == 0) {
				fprintf(stderr, "FATAL error, can't add options - DUPLICATE NAME FOUND: l=%d/%s i=%d/%s\n", l, longopts[l].name, i,  longopts_custom[i].name);
				exit(2);
			};
		};

		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Add long option on position %d/%d: %s", *maxentries_p, i, longopts_custom[i].name);

		longopts[*maxentries_p] = longopts_custom[i];
		(*maxentries_p)++;
	};

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Add short options: %s", shortopts_custom);

	/* check for duplicates */
	if (strlen(shortopts_p) > 0) {
		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Already given short options: %s", shortopts_p);
		for (j = 0; j < strlen(shortopts_p); j++) {
			for (k = 0; k < strlen(shortopts_custom); k++) {
				if (shortopts_custom[k] == ':') {
					continue;
				};
				if (shortopts_p[j] == shortopts_custom[k]) {
					fprintf(stderr, "FATAL error, can't add short options - DUPLICATE CHAR FOUND: %c\n", shortopts_p[j]);
					exit(2);
				};
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Already given short options: (empty)");
	};

	snprintf(tempstring, sizeof(tempstring), "%s%s", shortopts_p, shortopts_custom);
	snprintf(shortopts_p, shortopts_maxlen, "%s", tempstring);

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Resulting short options: %s", shortopts_p);

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
 * add given options "common basic"
 */
void ipv6calc_options_add_common_basic(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p) {
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Called");

	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_common, ipv6calc_longopts_common, MAXENTRIES_ARRAY(ipv6calc_longopts_common));

#ifdef SUPPORT_MMDB
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_MMDB");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_mmdb, ipv6calc_longopts_mmdb, MAXENTRIES_ARRAY(ipv6calc_longopts_mmdb));
#endif

#ifdef SUPPORT_IP2LOCATION
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_IP2LOCATION");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_ip2location, ipv6calc_longopts_ip2location, MAXENTRIES_ARRAY(ipv6calc_longopts_ip2location));
#endif

#ifdef SUPPORT_GEOIP2
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_GEOIP2");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_geoip2, ipv6calc_longopts_geoip2, MAXENTRIES_ARRAY(ipv6calc_longopts_geoip2));
#endif

#ifdef SUPPORT_DBIP2
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_DBIP2");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_dbip2, ipv6calc_longopts_dbip2, MAXENTRIES_ARRAY(ipv6calc_longopts_dbip2));
#endif

#ifdef SUPPORT_EXTERNAL
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_EXTERNAL");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_external, ipv6calc_longopts_external, MAXENTRIES_ARRAY(ipv6calc_longopts_external));
#endif

#ifdef SUPPORT_BUILTIN
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "SUPPORT_BUILTIN");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_builtin, ipv6calc_longopts_builtin, MAXENTRIES_ARRAY(ipv6calc_longopts_builtin));
#endif

#if defined SUPPORT_EXTERNAL || SUPPORT_IP2LOCATION || defined SUPPORT_DBIP2 || defined SUPPORT_GEOIP2
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "DB_COMMON");
	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_db_common, ipv6calc_longopts_db_common, MAXENTRIES_ARRAY(ipv6calc_longopts_db_common));
#endif

	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Finished");
};


/* 
 * add given options "common anon"
 */
void ipv6calc_options_add_common_anon(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p) {
	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Called");

	ipv6calc_options_add(shortopts_p, shortopts_maxlen, longopts, maxentries_p, ipv6calc_shortopts_common_anon, ipv6calc_longopts_common_anon, MAXENTRIES_ARRAY(ipv6calc_longopts_common_anon));

	DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Finished");
};


/* 
 * call option handler "common basic"
 * return: 0: option found
 */
int ipv6calcoptions_common_basic(const int opt, const char *optarg, const struct option longopts[]) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Called opt=0x%08x", opt);

	/* general options */
	switch(opt) {
		case OPTION_NOOP:
			DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Found dummy option, skip");
			result = 0;
			break;

		case 'd':
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Found debug option with value: %s", optarg);
			ipv6calc_debug = parse_dec_hex_val(optarg);
			ERRORPRINT_WA("given debug value: %lx", ipv6calc_debug);
			result = 0;
			break;

		case 'q':
			DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Found quiet option");
			ipv6calc_quiet = 1;
			result = 0;
			break;

		case 'V':
			DEBUGPRINT_NA(DEBUG_ipv6calcoptions, "Found verbose option");
			ipv6calc_verbose++;
			ipv6calc_quiet = 0;
			result = 0;
			break;

		default:
			/* jump to other parsers */
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Call sub-parser for opt=0x%08x", opt);

			result = libipv6calc_db_wrapper_options(opt, optarg, longopts);
	};

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Return with result: %d", result);

	return(result);
};


/* 
 * call option handler "common anon"
 * return: 0: option found
 */
int ipv6calcoptions_common_anon(const int opt, const char *optarg, const struct option longopts[], s_ipv6calc_anon_set *ipv6calc_anon_set_p) {
	int result = -1, i;
	int mask_ipv4;
	int mask_ipv6;
	int mask_eui64;
	int mask_mac;

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Called opt=0x%08x", opt);

	if (sizeof (*longopts) == 0) { }; // make compiler happy (avoid unused "...")

	/* general options */
	switch(opt) {
		case CMD_ANON_MASK_AUTOADJUST:
			if (strcmp(optarg, "yes") == 0) {
				i = 1;
			} else if (strcmp(optarg, "no") == 0) {
				i = 0;
			} else {
				fprintf(stderr, " unsupported  value for option 'mask-autoadjust', only yes|no is supported\n");
				exit(EXIT_FAILURE);
			};
			if (ipv6calc_anon_set_p->mask_autoadjust != i) {
				ipv6calc_anon_set_p->mask_autoadjust = i;
				snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
			};
			result = 0;
			break;

		case CMD_ANON_MASK_IID:
		case CMD_ANON_MASK_EUI64:
			if (opt == CMD_ANON_MASK_IID) {
				fprintf(stderr, " option 'mask-iid' is deprecated, please use 'mask-eui64'\n");
			};
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Found option: mask-eui=%s", optarg);
			mask_eui64 = atoi(optarg);
			if (mask_eui64 < 0 || mask_eui64 > 64) {
				fprintf(stderr, " value for option 'mask-eui64' out-of-range  [0-64]\n");
				exit(EXIT_FAILURE);
			};

			if (ipv6calc_anon_set_p->mask_eui64 != mask_eui64) {
				ipv6calc_anon_set_p->mask_eui64 = mask_eui64;
				snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
			};
			result = 0;
			break;

		case CMD_ANON_MASK_IPV4:
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Found option: mask-ipv4=%s", optarg);
			mask_ipv4 = atoi(optarg);
			if (mask_ipv4 < 0 || mask_ipv4 > 32) {
				fprintf(stderr, " value for option 'mask-ipv4' out-of-range  [0-32]\n");
				exit(EXIT_FAILURE);
			};

			if (ipv6calc_anon_set_p->mask_ipv4 != mask_ipv4) {
				ipv6calc_anon_set_p->mask_ipv4 = mask_ipv4;
				snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
			};
			result = 0;
			break;

		case CMD_ANON_MASK_IPV6:
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Found option: mask-ipv6=%s", optarg);
			mask_ipv6 = atoi(optarg);
			if (mask_ipv6 < 0 || mask_ipv6 > 64) {
				fprintf(stderr, " value for option 'mask-ipv6' out-of-range  [0-64]\n");
				exit(EXIT_FAILURE);
			};

			if (ipv6calc_anon_set_p->mask_ipv6 != mask_ipv6) {
				ipv6calc_anon_set_p->mask_ipv6 = mask_ipv6;
				snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
			};
			result = 0;
			break;

		case CMD_ANON_MASK_MAC:
			mask_mac = atoi(optarg);
			if (mask_mac < 0 || mask_mac > 48) {
				fprintf(stderr, " value for option 'mask-mac' out-of-range  [0-48]\n");
				exit(EXIT_FAILURE);
			};

			if (ipv6calc_anon_set_p->mask_mac != mask_mac) {
				ipv6calc_anon_set_p->mask_mac = mask_mac;
				snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
			};
			result = 0;
			break;

		case CMD_ANON_PRESET_STANDARD:
			result = libipv6calc_anon_set_by_name(ipv6calc_anon_set_p, "as");
			if (result != 0) {
				fprintf(stderr, "ipv6calc anonymization preset not found: anonymize-standard\n");
				exit(EXIT_FAILURE);
			};
			break;

		case CMD_ANON_PRESET_CAREFUL:
			result = libipv6calc_anon_set_by_name(ipv6calc_anon_set_p, "ac");
			if (result != 0) {
				fprintf(stderr, "ipv6calc anonymization preset not found: anonymize-careful\n");
				exit(EXIT_FAILURE);
			};
			result = 0;
			break;

		case CMD_ANON_PRESET_PARANOID:
			result = libipv6calc_anon_set_by_name(ipv6calc_anon_set_p, "ap");
			if (result != 0) {
				fprintf(stderr, "ipv6calc anonymization preset not found: anonymize-paranoid\n");
				exit(EXIT_FAILURE);
			};
			result = 0;
			break;

		case CMD_ANON_PRESET_OPTION:
			result = libipv6calc_anon_set_by_name(ipv6calc_anon_set_p, optarg);
			if (result != 0) {
				fprintf(stderr, "ipv6calc anonymization preset not found: %s\n", optarg);
				exit(EXIT_FAILURE);
			};
			result = 0;
			break;

		case CMD_ANON_METHOD_OPTION:
			for (i = 0; i < ipv6calc_anon_methods_entries; i++) {
				if (strcmp(ipv6calc_anon_methods[i].name, optarg) == 0) {
					ipv6calc_anon_set_p->method = ipv6calc_anon_methods[i].method;
					snprintf(ipv6calc_anon_set_p->name, sizeof(ipv6calc_anon_set_p->name), "%s", "custom");
					break;
				};
			};

			if (i == ipv6calc_anon_methods_entries) {
				fprintf(stderr, "anonymization method not supported: %s\n", optarg);
				exit(EXIT_FAILURE);
			};
			result = 0;
			break;
	};

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Return with result: %d", result);

	return(result);
};


/* get option name for an option
 * in : opt
 * ret: char* to string
 */
const char *ipv6calcoption_name(const int opt, const struct option longopts[]) {
	int i = 0;

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Called opt=%d", opt);

	while (longopts[i].name != NULL) {
		if (opt == longopts[i].val) {
			return(longopts[i].name);
		};
		i++;
	};

	return("UNKNOWN");
};


/* get common options from environment */
void ipv6calc_common_options_from_env(const struct option longopts[], s_ipv6calc_anon_set *ipv6calc_anon_set_p) {
	int i, result;
	unsigned int s;
	char tempstring[IPV6CALC_STRING_MAX];
	char *environment_value;

	i = 0;
	while(longopts[i].name != NULL) {
		// convert long option name to environment name
		snprintf(tempstring, sizeof(tempstring), "IPV6CALC_%s", longopts[i].name);
		for (s = 0; s < strlen(tempstring); s++) {
			switch(tempstring[s]) {
			    case '-':
				tempstring[s] = '_';
				break;

			    default:
				tempstring[s] = toupper(tempstring[s]);
				break;
			};
		};

		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "Long option: %s/%d/%08x env=%s", longopts[i].name, longopts[i].has_arg, longopts[i].val, tempstring);

		environment_value = getenv(tempstring);

		if (environment_value != NULL) {
			DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "found in environment: %s=%s (%08x)", tempstring, environment_value, longopts[i].val);

			if (longopts[i].has_arg == 0) {
				if (
				       (strcasecmp(environment_value, "no") == 0)
				    || (strcasecmp(environment_value, "off") == 0)
				    || (strcasecmp(environment_value, "0") == 0)
				) {
					goto NEXT_in_environment;
					// skip
				};

				if (
				       (strcasecmp(environment_value, "yes") != 0)
				    && (strcasecmp(environment_value, "on") != 0)
				    && (strcasecmp(environment_value, "1") != 0)
				) {
					fprintf(stderr, "value for environment can only be yes|on|1 or no|off|0: %s=%s\n", tempstring, environment_value);
					exit(EXIT_FAILURE);
				};
			};

			result = ipv6calcoptions_common_basic(longopts[i].val, environment_value, longopts);
			if (result == 0) {
				// found
				goto NEXT_in_environment;
			};

			if (ipv6calc_anon_set_p != NULL) {
				result = ipv6calcoptions_common_anon(longopts[i].val, environment_value, longopts, ipv6calc_anon_set_p);
				if (result == 0) {
					// found
					goto NEXT_in_environment;
				};
			};
		};
NEXT_in_environment:
		i++;
	};
};


/*
 * set a particular (common) option by name/value
 * in : longopts, name, value, ipv6calc_anon_set_p
 * out: 0: found/set  <>0: not found
 */
int ipv6calc_set_option(const struct option longopts[], const char *name, const char *value, s_ipv6calc_anon_set *ipv6calc_anon_set_p) {
	int i, result = -1;

	DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "called with option: %s=%s", name, value);

	i = 0;
	while(longopts[i].name != NULL) {
		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "check option: %s/%d/%08x", longopts[i].name, longopts[i].has_arg, longopts[i].val);

		if (strcmp(name, longopts[i].name) != 0) {
			// no match
			goto NEXT_option;
		};

		DEBUGPRINT_WA(DEBUG_ipv6calcoptions, "found option: %s=%s (%08x)", longopts[i].name, value, longopts[i].val);

		if ((longopts[i].has_arg == 0) && (value != NULL)) {
			// check value of toggle
			if (
			       (strcasecmp(value, "no") == 0)
			    || (strcasecmp(value, "off") == 0)
			    || (strcasecmp(value, "0") == 0)
			) {
				// skip
				break;
			};

			if (
			       (strcasecmp(value, "yes") != 0)
			    && (strcasecmp(value, "on") != 0)
			    && (strcasecmp(value, "1") != 0)
			) {
				fprintf(stderr, "value for option can only be yes|on|1 or no|off|0: %s=%s\n", longopts[i].name, value);
				exit(EXIT_FAILURE);
			};
		};

		result = ipv6calcoptions_common_basic(longopts[i].val, value, longopts);
		if (result == 0) {
			// found
			break;
		};

		if (ipv6calc_anon_set_p != NULL) {
			result = ipv6calcoptions_common_anon(longopts[i].val, value, longopts, ipv6calc_anon_set_p);
			if (result == 0) {
				// found
				break;
			};
		};
NEXT_option:
		i++;
	};

	return(result);
};
