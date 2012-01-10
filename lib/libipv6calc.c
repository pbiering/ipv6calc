/*
 * Project    : ipv6calc/lib
 * File       : libipv6calc.c
 * Version    : $Id: libipv6calc.c,v 1.23 2012/01/10 20:50:16 peter Exp $
 * Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for some tools
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ipv6calctypes.h"
#include "libipv6calcdebug.h"
#include "librfc1924.h"
#include "librfc2874.h"
#include "librfc1886.h"


/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_upcase"
void string_to_upcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < (int) strlen(string); i++) {
			string[i] = toupper(string[i]);
		};
	};

	return;
};
#undef DEBUG_function_name


/*
 * function converts chars in a string to lowcase
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_lowcase"
void string_to_lowcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < (int) strlen(string); i++) {
			string[i] = tolower(string[i]);
		};
	};

	return;
};
#undef DEBUG_function_name


/*
 * reverse string
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_reverse"
void string_to_reverse(char *string) {
	int i;
	char helpchar;
	size_t length;

	length = strlen(string);

	if (length < 2) {
		/* nothing to do */
		return;
	};

	for (i = 0; i < ( (int) (length >> 1)); i++) {
		helpchar = string[i];
		string[i] = string[length - i - 1];
		string[length - i - 1] = helpchar;
	};

	return;
};
#undef DEBUG_function_name


/*
 * dotted-reverse string
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_reverse_dotted"
void string_to_reverse_dotted(char *string) {
	char resultstring[NI_MAXHOST], tempstring[NI_MAXHOST];
	char *token, *cptr, **ptrptr;
	int flag_first = 1;
	
	ptrptr = &cptr;

	/* clear result string */
	snprintf(resultstring, sizeof(resultstring) - 1 , "%s", "");

	/* check for starting dot */
	if ( string[0] == '.' ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "%s.", resultstring);
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);
	};

	token = strtok_r(string, ".", ptrptr);

	while (token != NULL) {
		if (flag_first == 1) {
			snprintf(tempstring, sizeof(tempstring) - 1, "%s%s", token, resultstring);
			flag_first = 0;
		} else {
			snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", token, resultstring);
		};
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);

		token = strtok_r(NULL, ".", ptrptr);
	};
	
	if ( string[strlen(string) - 1] == '.' ) {
		snprintf(tempstring, sizeof(tempstring) - 1, ".%s", resultstring);
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);
	};

	snprintf(string, NI_MAXHOST - 1, ".%s", resultstring);
	
	return;
};
#undef DEBUG_function_name


/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 * ret: format number
 */
#define DEBUG_function_name "libipv6calc/autodetectinput"
uint32_t libipv6calc_autodetectinput(const char *string) {
	uint32_t type = FORMAT_auto_noresult;
	int i, j = 0, result;
	int numdots = 0, numcolons = 0, numdigits = 0, numxdigits = 0, numdashes = 0, numspaces = 0, numslashes = 0, numalnums = 0, numchar_s = 0, numpercents = 0;
	char resultstring[NI_MAXHOST];
	size_t length;

	length = strlen(string);

	if (length == 0) {
		/* input is empty */
		goto END_libipv6calc_autodetectinput;
	};

	for (i = 0; i < (int) length; i++) {
		if (string[i] == '.') { numdots++; };
		if (string[i] == ':') { numcolons++; };
		if (string[i] == '-') { numdashes++; };
		if (string[i] == '/') { numslashes++; };
		if (string[i] == ' ') { numspaces++; };
		if (string[i] == '%') { numpercents++; };
		if (string[i] == 's') { numchar_s++; };
		if (isdigit((int) string[i])) { numdigits++; };
		if (isxdigit((int) string[i])) { numxdigits++; };
		if (isalnum((int) string[i])) { numalnums++; };
	};

	if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
		fprintf(stderr, "%s: Autodetection source:\n", DEBUG_function_name);
		fprintf(stderr, "%s:  numdots    :%d\n", DEBUG_function_name, numdots);
		fprintf(stderr, "%s:  numcolons  :%d\n", DEBUG_function_name, numcolons);
		fprintf(stderr, "%s:  numdashes  :%d\n", DEBUG_function_name, numdashes);
		fprintf(stderr, "%s:  numspaces  :%d\n", DEBUG_function_name, numspaces);
		fprintf(stderr, "%s:  numslashes :%d\n", DEBUG_function_name, numslashes);
		fprintf(stderr, "%s:  numdigits  :%d\n", DEBUG_function_name, numdigits);
		fprintf(stderr, "%s:  numxdigits :%d\n", DEBUG_function_name, numxdigits);
		fprintf(stderr, "%s:  numalnums  :%d\n", DEBUG_function_name, numalnums);
		fprintf(stderr, "%s:  numpercents:%d\n", DEBUG_function_name, numpercents);
		fprintf(stderr, "%s:  numchar_s  :%d\n", DEBUG_function_name, numchar_s);
		fprintf(stderr, "%s:  length     :%d\n", DEBUG_function_name, (int) length);
	};

	if ( length == 20 && numdots == 0 && numcolons == 0 ) {
	        /* probably a base85 one */
		if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_base85\n", DEBUG_function_name);
		};
		result = librfc1924_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: base85 */
			type = FORMAT_base85;
			goto END_libipv6calc_autodetectinput;
		} else if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_base85 not successful, result: %s\n", DEBUG_function_name, resultstring);
		};
	};
	
	if (length >= 7 && length <= 15 && numdots == 3 && numcolons == 0 && numdigits == numxdigits && numdigits >= 4 && numdigits <= 12 && numslashes <= 1 && (numdots + numdigits + numslashes) == length) {
		/* IPv4: ddd.ddd.ddd.ddd */
		type = FORMAT_ipv4addr;
		goto END_libipv6calc_autodetectinput;
	};

	if ( strncmp(string, "\\[", 2) == 0 ) {
		/* check for Bitstring label: \[x..../dd] */
		if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_bitstring\n", DEBUG_function_name);
		};
		result = librfc2874_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: bitstring label */
			type = FORMAT_bitstring;
			goto END_libipv6calc_autodetectinput;
		} else if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_bitstring not successful, result: %s\n", DEBUG_function_name, resultstring);
		};
	};
	
	if (length == 32 && numxdigits == 32 && numdots == 0 && numcolons == 0) {
		/* ifinet6 xxxx..xxxx  */
		type = FORMAT_ifinet6;
		goto END_libipv6calc_autodetectinput;
	};
	
	if (length == 8 && numxdigits == 8 && numdots == 0 && numcolons == 0) {
		/* IPv4 hexadecimal: xxxxxxxx */
		type = FORMAT_ipv4hex;
		goto END_libipv6calc_autodetectinput;
	};
	
	if ((length >= 11 && length <= 17 && numxdigits >= 6 && numxdigits <= 12 && numdots == 0 && ( (numcolons == 5 && numdashes == 0 && numspaces == 0) || (numcolons == 0 && numdashes == 5 && numspaces == 0) || (numcolons == 0 && numdashes == 0 && numspaces == 5))) || (length == 13 && numcolons == 0 && numdashes == 1 && numspaces == 0 && numxdigits == 12) || (length == 12 && numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 12)) {
		/* MAC 00:00:00:00:00:00 or 00-00-00-00-00-00 or "xx xx xx xx xx xx" or "xxxxxx-xxxxxx" or xxxxxxxxxxxx */

		if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_mac\n", DEBUG_function_name);
		};

		/* Check whether minimum 1 xdigit is between colons, dashes, spaces */
		if (numcolons == 0 && numdashes == 1 && numspaces == 0 && numxdigits == 12) {

		/* Check xxxxxx-xxxxxx */
		j = 0;
		for (i = 0; i < (int) length; i++) {
			if (isxdigit((int) string[i])) {
				j++;
				if ( j > 6 ) {
					/* more than 6 xdigits */
					j = -1;
					break;
				};
				continue;
		       	} else if (string[i] == '-' ) {
				if ( j == 0 ) {
					/* dash follow dash */
					j = -1;
					break;
				};
				j = 0;
				continue;
		       	};
			/* normally not reached */
			j = -1;
			break;
		};

		} else if (numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 12) {
			/* nothing more to check */
		} else {

		j = 0;
		for (i = 0; i < (int) length; i++) {
			if (isxdigit((int) string[i])) {
				j++;
				if ( j > 2 ) {
					/* more than 2 xdigits */
					j = -1;
					break;
				};
				continue;
		       	} else if (string[i] == ':' || string[i] == '-' || string[i] == ' ') {
				if ( j == 0 ) {
					/* colon/dash/space follows colon/dash/space */
					j = -1;
					break;
				};
				j = 0;
				continue;
		       	};
			/* normally not reached */
			j = -1;
			break;
		};

		}; /* end of if */

		if ( j != -1 ) {
			type = FORMAT_mac;
			if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
				fprintf(stderr, "%s: Autodetection found type: mac\n", DEBUG_function_name);
			};
			goto END_libipv6calc_autodetectinput;
		};
	};
	
	if (numcolons == 0 && numdots > 0 && numslashes == 0 && numspaces == 0 && (numalnums + numdots) == length) {
		/* check for reverse nibble string */
		if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_revnibbels_int\n", DEBUG_function_name);
		};
		result = librfc1886_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: reverse nibble string */
			type = FORMAT_revnibbles_int;
			goto END_libipv6calc_autodetectinput;
		} else if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
			fprintf(stderr, "%s:  check FORMAT_revnibbels_int not successful, result: %s\n", DEBUG_function_name, resultstring);
		};
	};
	
	if (numcolons != 0 && numdots <= 3 && numslashes <= 1 && ((numpercents == 0 && (numdots + numcolons + numxdigits + numslashes) == length) || (numpercents == 1 && (numdots + numcolons + numxdigits + numslashes + numpercents) <= length))) {
		/* hopefully an IPv6 address */
		/* fe80::1 */
		/* fe80::1%eth0 */
		type = FORMAT_ipv6addr;
		goto END_libipv6calc_autodetectinput;
	};

	if (numcolons == 0 && numdots == 2 && numslashes == 0 && numdashes >= 3 && ((numchar_s == 0 && (numdashes + numdots + numxdigits + 10) == length) || (numchar_s == 1 && (numdashes + numdots + numxdigits + numchar_s + 10) <= length))) {
		/* hopefully an IPv6 literal address (e.g. 2001-DB8--1.IPV6-LITERAL.NET) IPV6-LITERAL.NET has 10 chars which are not xdigit  */
		/* also supported with scope: fe80--218-8bff-fe17-a226s4.ipv6-literal.net */
		type = FORMAT_ipv6literal;
		goto END_libipv6calc_autodetectinput;
	};
	
END_libipv6calc_autodetectinput:	
	if ( (ipv6calc_debug & DEBUG_libipv6calc) != 0 ) {
		if (type != FORMAT_auto_noresult) {
			fprintf(stderr, "%s: Autodetection found type: 0x%08x\n", DEBUG_function_name, (unsigned int) type);
		} else {
			fprintf(stderr, "%s: Autodetection not successful\n", DEBUG_function_name);
		};
	};
	return (type);
};
#undef DEBUG_function_name
