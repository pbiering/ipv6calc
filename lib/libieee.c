/*
 * Project    : ipv6calc
 * File       : libieee.c
 * Version    : $Id: libieee.c,v 1.2 2002/03/24 17:30:44 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for IEEE information
 */

#include <stdio.h>
#include <string.h>

#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "libieee.h"
#include "../databases/ieee-oui/dbieee_oui.h"

/*
 * Get vendor string
 * in:  o1,o2,o3: first octets
 * mod: resultstring
 * out: 0=found, 1=not found
 */
#define DEBUG_function_name "libieee/get_vendor_string"
int libieee_get_vendor_string(char *resultstring, const uint8_t o1,  const uint8_t o2, const uint8_t o3) {
	int retval = 1;
	int i;
	uint32_t idval;

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* catch special ones */
	if ((o1 == 0xfc && o2 == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		sprintf(resultstring, "Linux ISDN-NET/PLIP");
		return (0);
	};
	
	if ( (o1 & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

	if ( (o1 & 0x02) != 0 ) {
		/* Not global scope */
		return (1);
	};

	idval = (o1 << 16) | (o2 << 8) | o3;

	/* run through OUI list now */
	for (i = 0; i < (int) (sizeof(libieee_oui) / sizeof(libieee_oui[0])); i++) {
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

/*
 * Get short vendor string
 * in:  o1,o2,o3: first octets
 * mod: resultstring
 * out: 0=found, 1=not found
 */
#define DEBUG_function_name "libieee/get_short_vendor_string"
int libieee_get_short_vendor_string(char *resultstring, const uint8_t o1, const uint8_t o2, const uint8_t o3) {
	int retval = 1;
	int i;
	uint32_t idval;

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* catch special ones */
	if ((o1 == 0xfc && o2 == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		sprintf(resultstring, "Linux-ISDN-NET+PLIP");
		return (0);
	};
	
	if ( (o1 & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

	if ( (o1 & 0x02) != 0 ) {
		/* Not global scope */
		return (1);
	};

	idval = (o1 << 16) | (o2 << 8) | o3;

	/* run through OUI list now */
	for (i = 0; i < (int) (sizeof(libieee_oui) / sizeof(libieee_oui[0])); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			sprintf(resultstring, "%s", libieee_oui[i].shortstring_owner);
			return (0);
		};
	};

	/* not found */
   	retval = 1;	
	return (retval);
};
#undef DEBUG_function_name
