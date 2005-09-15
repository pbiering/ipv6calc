/*
 * Project    : ipv6calc
 * File       : librfc1924.c
 * Version    : $Id: librfc1924.c,v 1.7 2005/09/15 12:32:21 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for conversions defined in RFC 1924
 *
 * Based on ftp://munnari.oz.au/ipv6imp/rfc1924-encode.c
 *  by * Robert Elz <kre at munnari dot OZ dot AU>
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

typedef struct {
	unsigned char b[128];		/* never anything but 0 or 1 */
} bitvec;


static char librfc1924_charset[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '!', '#', '$', '%', '&', '(', ')', '*',
	'+', '-', ';', '<', '=', '>', '?', '@', '^', '_',
	'`', '{', '|', '}', '~',
	0
};


static bitvec lsl(bitvec bv, int n) {
	register int i;
	bitvec r;

	for (i = 128; --i >= 128-n; ) {
		r.b[i] = 0;
	};
	while (i >= 0) {
		r.b[i] = bv.b[i + n];
		i--;
	};
	return r;
};

static bitvec lsr(bitvec bv, int n) {
	register int i;
	bitvec r;

	for (i = 0; i < n; i++) {
		r.b[i] = 0;
	};
	while (i < 128) {
		r.b[i] = bv.b[i - n];
		i++;
	};
	return r;
};

/* not needed 
static bitvec asr(bitvec bv, int n) {
	register int i;
	bitvec r;

	for (i = 0; i < n; i++) {
		r.b[i] = bv.b[0];
	};
	while (i < 128) {
		r.b[i] = bv.b[i - n];
		i++;
	};
	return r;
};
*/ 

static int topbit(bitvec bv) {
	return bv.b[0];
}

static int botbit(bitvec bv) {
	return bv.b[127];
};

/* not needed
static void settop(bitvec *bv, int v) {
	bv->b[0] = v;
};
*/

static void setbot(bitvec *bv, int v) {
	bv->b[127] = v;
};

static bitvec zbv() {
	bitvec r;
	register int i;

	for (i = 0; i < 128; i++) {
		r.b[i] = 0;
	};
	return r;
};

static bitvec addbv(bitvec a, bitvec b) {
	register int i, j;
	bitvec r;

	j = 0;
	for (i = 128; --i >= 0; ) {
		j += a.b[i] + b.b[i];
		r.b[i] = j & 1;
		j >>= 1;
	};
	return r;
};

static bitvec subbv(bitvec a, bitvec b) {
	register int i;
	register unsigned j;
	register unsigned k;
	bitvec r;

	j = 0;
	for (i = 128; --i >= 0; ) {
		k = (a.b[i] ^ b.b[i] ^ j) & 1;
		j = ( ~j & ~a.b[i] & b.b[i] ) | ( j & ~a.b[i] ) | ( j & b.b[i] );
		r.b[i] = k;
	};
	return r;
};

static int cmpbv(bitvec a, bitvec b) {
	register int i, j;

	for (i = 0; i < 128; i++) {
		j = (int)a.b[i] - (int)b.b[i];
		if (j != 0)
			break;
	};
	return j;
};

static bitvec bvx85(bitvec bv) {
	/* 85 == 64 + 16 + 4 + 1 */
	return addbv(lsl(bv, 6), addbv(lsl(bv, 4), addbv(lsl(bv, 2), bv)));
};

static bitvec bvdiv85(bitvec bv, int *rem) {
	bitvec e5;
	bitvec div;
	bitvec r;
	register int i, j;

	r = zbv();
	setbot(&r, 1);
	e5 = addbv(lsl(r, 6), addbv(lsl(r, 4), addbv(lsl(r, 2), r)));

	div = lsl(e5, 121);		/* e5 is 7 bits, 121 + 7 == 128 */

	r = zbv();

	for (i = 0; i < 122; i++) {
		if (cmpbv(bv, div) < 0) {
			div = lsr(div, 1);
			r = lsl(r, 1);
			continue;
		};
		bv = subbv(bv, div);
		div = lsr(div, 1);
		r = lsl(r, 1);
		setbot(&r, 1);
	};

	j = 0;
	bv = lsl(bv, 121);

	for (i = 0; i < 7; i++) {
		j <<= 1;
		j |= topbit(bv);
		bv = lsl(bv, 1);
	};
	if (rem)
		*rem = j;

	return r;
};

static bitvec smallbv(register int n) {
	bitvec r, t;
	register int i;

	r = zbv();

	i = 0;
	while (n != 0) {
		t = lsl(t, 1);
		setbot(&t, n & 1);
		n >>= 1;
		i++;
	};

	while (--i >= 0) {
		r = lsl(r, 1);
		setbot(&r, botbit(t));
		t = lsr(t, 1);
	};
	return r;
};

static bitvec unpk85(register char *str) {
	bitvec r;
	register char c;
	register int i;
	register char *p;

	r = zbv();
	for (i = 0; i < 20; i++) {
		c = *str++;
		if (c == 0 || (p = strchr(librfc1924_charset, c)) == 0)
			return zbv();
		r = addbv(bvx85(r), smallbv(p - librfc1924_charset));
	};
	return (r);
};

static char *pk85(bitvec bv) {
	int vals[20];
	register int i;
	static char str[21];
	register char *p;

	for (i = 0; i < 20; i++) {
		bv = bvdiv85(bv, &vals[i]);
	};

	p = str;
	while (--i >= 0)
		*p++ = librfc1924_charset[vals[i]];
	*p = '\0';

	return str;
};


/*
 * IPv6addr_structure to base85 format
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */

int ipv6addrstruct_to_base85(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1, nbit, ndword;
	unsigned int mask;
	bitvec bv;

	/* fill bitvector */
	bv = zbv();

	for (nbit = 0; nbit <= 127; nbit++) {
		/* calculate dword (32 bit) */
		ndword = (nbit & 0x60) >> 5;
		
		/* calculate mask */
		mask = 0x80000000 >> ((nbit & 0x01f));
		
		if ( (ipv6addr_getdword(ipv6addrp, ndword) & mask) != 0) {
			bv.b[nbit] = 1;
		};
	};

	/* convert */	
	snprintf(resultstring, NI_MAXHOST - 1, "%s", pk85(bv) );	

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
	int retval = 1, nbit, ndword;
	unsigned int mask;
	bitvec bv;
	char tempstring[NI_MAXHOST];

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
		
	bv = unpk85(tempstring);

	/* Clear IPv6 address structure */
	ipv6addr_clear(ipv6addrp);

	/* fill IPv6 address structure */
	for (nbit = 0; nbit <= 127; nbit++) {
		/* calculate word (16 bit) - matches with addr6p[]*/
		ndword = (nbit & 0x60) >> 5;
		
		/* calculate mask */
		mask = 0x80000000 >> ((nbit & 0x1f));
	
		if ( bv.b[nbit] == 1 ) {
			ipv6addr_setdword(ipv6addrp, ndword, ipv6addr_getdword(ipv6addrp, ndword) | mask);
		};
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
