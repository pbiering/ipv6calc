/*
 * Project    : ipv6calc
 * File       : libieee.c
 * Version    : $Id: libieee.c,v 1.1 2002/03/11 21:52:25 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for IEEE information
 */

#include <stdio.h>
#include <string.h>

#include "ipv6calc.h"
#include "libieee.h"
#include "libieeeoui.h"

/*
 * Get vendor string
 * in:  o1,o2,o3: first octets
 * mod: resultstring
 * out: 0=found, 1=not found
 */

#define DEBUG_function_name "libieee/get_vendor_string"
int libieee_get_vendor_string(char *resultstring, const unsigned short o1,  const unsigned short o2,  const unsigned short o3) {
	int retval = 1;
	int i;
	unsigned int idval;

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* catch special ones */
	if ((o1 == 0xfc && o2 == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		sprintf(resultstring, "Linux ISDN-NET/PLIP");
		return (0);
	};
	
	if (o1 & 0x01) {
		/* Multicast */
		return (1);
	};

	if (o1 & 0x02) {
		/* Not global scope */
		return (1);
	};

	idval = (o1 << 16) | (o2 << 8) | o3;

	/* run through OUI list now */
	for (i = 0; i < sizeof(libieee_oui) / sizeof(libieee_oui[1]); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			sprintf(resultstring, "%s", libieee_oui[i].string_owner);
			return (0);
		};
	};

	/* not found */
   	retval = 1;	
	return (retval);
};
#undef DEBUG_function_name
