/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.c
 * Version    : $Id: ipv6calctypes.c,v 1.1 2002/03/01 23:27:25 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Format and type handling
 */ 

#include <stdio.h>
#include <string.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"

/*
 * check given type string
 */
#define DEBUG_function_name "ipv6calctypes/checktype"
int ipv6calctypes_checktype(char *string) {
	int i, num = -1, retval = -1;
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Got string: %s\n", DEBUG_function_name, string);
	};

	for (i = 0; i < FORMAT_STRING_MAXNUM; i++) {
		if (ipv6calc_debug) {
			fprintf(stderr, "%s: Compare against: %s\n", DEBUG_function_name, ipv6calc_formatstrings[i]);
		};
		if (strlen(string) != strlen(ipv6calc_formatstrings[i])) {
			/* length not equal */
			continue;
		};

		if (strcmp(string, ipv6calc_formatstrings[i]) != 0) {
			/* strings not equal */
			continue;
		};

		/* strings equal */
		num = i;
		break;
	};

	if (num == -1) {
		if (ipv6calc_debug) {
			fprintf(stderr, "%s: Found no proper string\n", DEBUG_function_name);
		};
		goto END_ipv6calctypes_checktype;
	};
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Look for number: %d\n", DEBUG_function_name, num);
	};

	/* look for proper number */
	for (i = 0; i < FORMAT_STRING_MAXNUM; i++) {
		if (num == ipv6calc_formatstringaliasmap[i][0]) {
			retval = ipv6calc_formatstringaliasmap[i][1];
			break;
		};
	};

END_ipv6calctypes_checktype:
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Found format number: %d\n", DEBUG_function_name, retval);
	};

	return(retval);
};
