/*
 * Project    : ipv6calc
 * File       : libipv6calc.c
 * Version    : $Id: libipv6calc.c,v 1.3 2002/04/04 21:58:21 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for some tools
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ipv6calctypes.h"
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
 * function converts chars in a string to upcase
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
 * function converts chars in a string to upcase
 * in : pointer to a string
 * ret: format number
 */
#define DEBUG_function_name "libipv6calc/autodetectinput"
uint32_t libipv6calc_autodetectinput(const char *string) {
	uint32_t type = FORMAT_undefined;
	int i, numdots = 0, numcolons = 0, numdigits = 0, numxdigits = 0, result;
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
		if (isdigit(string[i])) { numdigits++; };
		if (isxdigit(string[i])) { numxdigits++; };
	};

	if ( length == 20 && numdots == 0 && numcolons == 0 ) {
	        /* probably a base85 one */
		result = librfc1924_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: base85 */
			type = FORMAT_base85;
			goto END_libipv6calc_autodetectinput;
		};
	};
	
	if (length >= 7 && length <= 15 && numdots == 3 && numcolons == 0 && numdigits == numxdigits) {
		/* IPv4: ddd.ddd.ddd.ddd */
		type = FORMAT_ipv4addr;
		goto END_libipv6calc_autodetectinput;
	};

	if ( strncmp(string, "\\[", 2) == 0 ) {
		/* check for Bitstring label: \[x..../dd] */
		result = librfc2874_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: bitstring label */
			type = FORMAT_bitstring;
			goto END_libipv6calc_autodetectinput;
		};
	};
	
	if (length == 32 && numxdigits == 32 && numdots == 0 && numcolons == 0) {
		/* ifinet6 xxxx..xxxx  */
		type = FORMAT_ifinet6;
		goto END_libipv6calc_autodetectinput;
	};
	
	if (length >= 11 && length <= 17 && numxdigits >= 6 && numxdigits <= 12 && numdots == 0 && numcolons == 5) {
		/* MAC 00:00:00:00:00:00 */
		type = FORMAT_mac;
		goto END_libipv6calc_autodetectinput;
	};
	
	if (length >= 15 && length <= 23 && numxdigits >= 8 && numxdigits <= 16 && numdots == 0 && numcolons == 7) {
		/* EUI-64 00:00:00:00:00:00:00:00 */
		type = FORMAT_mac;
		goto END_libipv6calc_autodetectinput;
	};
	
	if (numcolons == 0 && numdots > 0) {
		/* check for reverse nibble string */
		result = librfc1886_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: reverse nibble string */
			type = FORMAT_revnibbles_int;
			goto END_libipv6calc_autodetectinput;
		};
	};
	
	if (numcolons != 0 && numdots <= 3) {
		/* hopefully an IPv6 address */
		type = FORMAT_ipv6addr;
		goto END_libipv6calc_autodetectinput;
	};
	
END_libipv6calc_autodetectinput:	
	return (type);
};
#undef DEBUG_function_name
