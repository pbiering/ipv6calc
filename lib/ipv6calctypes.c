/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.c
 * Version    : $Id: ipv6calctypes.c,v 1.1 2002/03/18 19:59:23 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Format and type handling
 */ 

#include <stdio.h>
#include <string.h>
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"

/*
 * check given type string
 */
#define DEBUG_function_name "ipv6calctypes/checktype"
int ipv6calctypes_checktype(const char *string) {
	int i;
	int number = -1;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	if ((ipv6calc_debug & DEBUG_libipv6calctypes) != 0) {
		fprintf(stderr, "%s: Got string: %s\n", DEBUG_function_name, string);
	};

	for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
		if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
			fprintf(stderr, "%s: Compare against: %s\n", DEBUG_function_name, ipv6calc_formatstrings[i].token);
		};

		/* check main token */
		if (strcmp(string, ipv6calc_formatstrings[i].token) == 0) {
			number = ipv6calc_formatstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_formatstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
			fprintf(stderr, "%s: Compare against aliases in string: %s\n", DEBUG_function_name, ipv6calc_formatstrings[i].aliases);
		};

		strncpy(tokenlist, ipv6calc_formatstrings[i].aliases, sizeof(tokenlist) - 1);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
				fprintf(stderr, "%s: Compare against alias token: %s\n", DEBUG_function_name, token);
			};
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_formatstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
		if (number < 0) {
			fprintf(stderr, "%s: Found no proper string\n", DEBUG_function_name);
		} else {
			fprintf(stderr, "%s: Found format number: %04x\n", DEBUG_function_name, number);
		};
	};

	return(number);
};
#undef DEBUG_function_name


/*
 * check given type string
 */
#define DEBUG_function_name "ipv6calctypes/checkaction"
int ipv6calctypes_checkaction(const char *string) {
	int i;
	int number = -1;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
		fprintf(stderr, "%s: Got string: %s\n", DEBUG_function_name, string);
	};

	for (i = 0; i < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); i++) {
		if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
			fprintf(stderr, "%s: Compare against: %s\n", DEBUG_function_name, ipv6calc_actionstrings[i].token);
		};

		/* check main token */
		if (strcmp(string, ipv6calc_actionstrings[i].token) == 0) {
			number = ipv6calc_actionstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_actionstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
			fprintf(stderr, "%s: Compare against aliases in string: %s\n", DEBUG_function_name, ipv6calc_actionstrings[i].aliases);
		};

		strncpy(tokenlist, ipv6calc_formatstrings[i].aliases, sizeof(tokenlist) - 1);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
				fprintf(stderr, "%s: Compare against alias token: %s\n", DEBUG_function_name, token);
			};
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_actionstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6calctypes) != 0 ) {
		if (number < 0) {
			fprintf(stderr, "%s: Found no proper string\n", DEBUG_function_name);
		} else {
			fprintf(stderr, "%s: Found action number: %04x\n", DEBUG_function_name, number);
		};
	};

	return(number);
};
