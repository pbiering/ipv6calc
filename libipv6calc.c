/*
 * Project    : ipv6calc
 * File       : libipv6calc.c
 * Version    : $Id: libipv6calc.c,v 1.7 2002/03/01 23:26:45 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for some tools
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_upcase"
void string_to_upcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < strlen(string); i++) {
			string[i] = toupper(string[i]);
		};
	};

	return;
};
#undef DEBUG_function_name
