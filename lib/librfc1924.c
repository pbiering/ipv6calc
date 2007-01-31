/*
 * Project    : ipv6calc
 * File       : librfc1924.c
 * Version    : $Id: librfc1924.c,v 1.10 2007/01/31 16:21:47 peter Exp $
 * Copyright  : 2001-2007 by Peter Bieringer <pb (at) bieringer.de>
 *              and 2006 by Niko Tyni <ntyni (at) iki.fi>
 *
 * Information:
 *  Function library for conversions defined in RFC 1924
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2 dated June, 1991.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libipv6addr.h"
#include "librfc1924.h"

/*
 * Base 85 (RFC 1924) encodings of IPv6 addresses
 */

static char librfc1924_charset[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 

	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 

	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 

	'!', '#', '$', '%', '&', '(', ')', '*', '+', '-', ';', '<', '=', 
	'>', '?', '@', '^', '_', '`', '{', '|', '}', '~', '\0'
};

/* 
 * to minimize work, we convert between base 85 and 
 * base 65536 (2**16), as that's the biggest power of 2 
 * we can use and still stay within word size (32 bits)
 * 
 * the conversions are done with the elemental 'long division'
 * algorithm, just with digits in strange bases
 */

void pk85(uint16_t *words, char *ret)
{
	uint16_t buf[8]; /* working space */
	uint32_t accu;
	int i,j;

	/* initialize the buffer */
	for (i = 0; i < 8; i++) {
		buf[i] = words[i];
	}

	/* do the division digit by digit */
	for (i = 0; i < 20; i++) {
		accu = 0;
		for (j = 0; j < 8; j++) {
			accu = (accu << 16) + buf[j];
			if (accu >= 85) {
				buf[j] = accu / 85; /* quotient  */
				accu = accu % 85;   /* remainder */
			} else {
				buf[j] = 0;
			}
		}
		/* reverse, as the least significant digit comes out first */
		ret[19 - i] = librfc1924_charset[accu];
	}
	ret[20] = '\0';
	return;
}

void unpk85(char *b, uint16_t *ret)
{
	uint32_t buf[20]; /* working space */
	uint32_t accu;
	int i, j;

	/* initialize the buffer */
	for (i = 0; i < 20; i++) {
		/* find the offset in the character table */
		buf[i] = strchr(librfc1924_charset, b[i]) - librfc1924_charset;
	}

	/* do the division digit by digit */
	for (i = 0; i < 8; i++) {
		accu = 0;
		for (j = 0; j < 20; j++) {
			accu = accu * 85 + buf[j];
			if ((accu & 0xffff0000) != 0) {      /* >= 2**16  */
				buf[j] = accu >> 16;  /* quotient  */
				accu = accu & 0xffff; /* remainder */
			} else {
				buf[j] = 0;
			}
		}
		/* reverse, as the least significant digit comes out first */
		ret[7 - i] = accu;
	}
	return;
}

/*
 * IPv6addr_structure to base85 format
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */

int ipv6addrstruct_to_base85(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1, i;

	uint16_t words[8];
	char ret[21];

	for (i=0; i < 8; i++) {
		words[i] = ipv6addr_getword(ipv6addrp, (unsigned int) i) ;
	};

	/* convert */
	pk85(words, ret);
	snprintf(resultstring, NI_MAXHOST - 1, "%s", ret);	

	retval = 0;	
	return (retval);
};


/* function base85 format to IPv6addr_structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */

int base85_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, i;
	char tempstring[NI_MAXHOST];
	uint16_t result[8];

	retval = librfc1924_formatcheck(addrstring, resultstring);

	if (retval != 0) {
		/* format check fails */
		return (1);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "Input too long: %s\n", addrstring);
		return (1);
	};

	strncpy(tempstring, addrstring, sizeof(tempstring) - 1);
		
	unpk85(tempstring, result);

	/* Clear IPv6 address structure */
	ipv6addr_clear(ipv6addrp);

	/* fill IPv6 address structure */
	for (i = 0; i < 8; i++) {
		ipv6addr_setword(ipv6addrp, (unsigned int) i, (unsigned int) result[i]);
	};
	
	/* Get scope */
	ipv6addrp->scope = ipv6addr_gettype(ipv6addrp);
	
	ipv6addrp->flag_valid = 1;

	retval = 0;	
	return (retval);
};


/*
 * checks for proper format of a base85 string
 *
 * in : string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc1924/formatcheck"
int librfc1924_formatcheck(const char *string, char *infostring) {
	size_t length, cnt;

	/* clear result string */
	infostring[0] = '\0';

	length = strlen(string);

	/* check length */
	if ( length != 20 ) {
		snprintf(infostring, NI_MAXHOST - 1, "Given base85 formatted address has not 20 chars!");
		return (1);
	};

	/* check for base85 chars only content */
	cnt = strspn(string, librfc1924_charset);
	if ( cnt != 20 ) {
		snprintf(infostring, NI_MAXHOST - 1, "Illegal character in given base85 formatted address on position %d (%c)!", (int) cnt + 1, string[cnt]);
		return (1);
	};

	return (0);
};
#undef DEBUG_function_name
