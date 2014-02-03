/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.c
 * Version    : $Id: ipv6calctypes.c,v 1.4 2014/02/03 20:48:03 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
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
 * in : format string
 * ret: format type
 */
uint32_t ipv6calctypes_checktype(const char *string) {
	int i;
	uint32_t number = FORMAT_undefined;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Got string: %s", string);

	for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against: %s", ipv6calc_formatstrings[i].token);

		/* check main token */
		if (strcmp(string, ipv6calc_formatstrings[i].token) == 0) {
			number = ipv6calc_formatstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_formatstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against aliases in string: %s", ipv6calc_formatstrings[i].aliases);

		snprintf(tokenlist, sizeof(tokenlist) - 1, "%s", ipv6calc_formatstrings[i].aliases);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against alias token: %s", token);
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_formatstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( number == FORMAT_undefined ) {
		DEBUGPRINT_NA(DEBUG_libipv6calctypes, "Found no proper string");
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Found format number: %08x", (unsigned int) number);
	};

	return(number);
};


/*
 * check given type string
 * in : action string
 * ret: action type
 */
uint32_t ipv6calctypes_checkaction(const char *string) {
	int i;
	uint32_t number = ACTION_undefined;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Got string: %s", string);

	for (i = 0; i < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against: %s", ipv6calc_actionstrings[i].token);

		/* check main token */
		if (strcmp(string, ipv6calc_actionstrings[i].token) == 0) {
			number = ipv6calc_actionstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_actionstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against aliases in string: %s", ipv6calc_actionstrings[i].aliases);

		snprintf(tokenlist, sizeof(tokenlist) - 1, "%s", ipv6calc_formatstrings[i].aliases);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against alias token: %s", token);
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_actionstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( number == FORMAT_undefined ) {
		DEBUGPRINT_NA(DEBUG_libipv6calctypes, "Found no proper string");
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Found action number: %08x", (unsigned int)  number);
	};

	return(number);
};
