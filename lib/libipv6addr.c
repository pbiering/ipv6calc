/*
 * Project    : ipv6calc
 * File       : libipv6addr.c
 * Version    : $Id: libipv6addr.c,v 1.73 2013/02/24 19:12:14 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Function library for IPv6 address handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "libipv6addr.h"
#include "librfc1884.h"
#include "librfc3041.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"

#ifdef SUPPORT_DB_IPV6
#include "../databases/ipv6-assignment/dbipv6addr_assignment.h"
#endif


/*
 * function returns an octet of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numoctet  = number of octet (0 = MSB, 15 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getoctet"
uint8_t ipv6addr_getoctet(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet) {
	uint8_t retval;
	
	if ( numoctet > 15 ) {
		fprintf(stderr, "%s: given ocett number '%u' is out of range!\n", DEBUG_function_name, numoctet);
		exit(EXIT_FAILURE);
	};

	retval = ipv6addrp->in6_addr.s6_addr[numoctet];

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a word of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getword"
uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 7 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numword * 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a dword of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getdword"
uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword) {
	uint32_t retval;
	
	if ( numdword > 3 ) {
		fprintf(stderr, "%s: given dword number '%u' is out of range!\n", DEBUG_function_name, numdword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numdword * 4] << 24 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] << 16 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] ); 

	return (retval);
};
#undef DEBUG_function_name


/*
 * function sets an octet of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numoctet   = number of word (0 = MSB, 15 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setoctet"
void ipv6addr_setoctet(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet, const unsigned int value) {
	
	if ( numoctet > 15 ) {
		fprintf(stderr, "%s: given octet number '%u' is out of range!\n", DEBUG_function_name, numoctet);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000000ff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numoctet] = (uint8_t) value;

	return;
};
#undef DEBUG_function_name


/*
 * function sets a word of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setword"
void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, const unsigned int value) {
	if ( numword > 7 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000ffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numword * 2    ] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};
#undef DEBUG_function_name


/*
 * function sets a dword of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setdword"
void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, const unsigned int value) {
	
	if ( numdword > 3 ) {
		fprintf(stderr, "%s: given dword number '%u' is out of range!\n", DEBUG_function_name, numdword);
		exit(EXIT_FAILURE);
	};

	if ( value > 0xffffffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numdword * 4    ] = (uint8_t) ( ( value & 0xff000000 ) >> 24 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] = (uint8_t) ( ( value & 0x00ff0000 ) >> 16 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_clear"
void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp) {
	int i;

	for (i = 0; i < (int) (sizeof(ipv6addrp->in6_addr.s6_addr) / sizeof(ipv6addrp->in6_addr.s6_addr[0])); i++) {
		ipv6addrp->in6_addr.s6_addr[i] = 0;
	};
	
	/* Clear IPv6 address scope */
	ipv6addrp->scope = 0;

	/* Clear valid flag */
	ipv6addrp->flag_valid = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_clearall"
void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp) {
	ipv6addr_clear(ipv6addrp);

	/* Clear other field */
	ipv6addrp->bit_start = 1;
	ipv6addrp->bit_end = 128;
	ipv6addrp->flag_startend_use = 0;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->prefixlength = 0;
	ipv6addrp->flag_valid = 0;
	ipv6addrp->flag_scopeid = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function copies the IPv6 structure
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * mod: ipv6addrp2 = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_copy"
void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src) {

	*(ipv6addrp_dst) = *(ipv6addrp_src);
	
	return;
};
#undef DEBUG_function_name


/*
 * create checksum for anonymized IID
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: 4-bit checksum
 *
 * using same calculation as for ISAN: ISO 7064 , MOD 17,16
 * http://www.pruefziffernberechnung.de/I/ISAN.shtml
 */
uint32_t ipv6addr_checksum_anonymized_iid(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t iid[2];
	uint32_t checksum = 0;
	int i, n, iid_num;
	uint32_t a, b, c = 0;

	iid[0] = ipv6addr_getdword(ipv6addrp, 2); // 00-31 (8 nibbles)
	iid[1] = ipv6addr_getdword(ipv6addrp, 3); // 32-63 (8 nibbles, only 7 nibbles are used for calculation)

	// iid[0] = 0x123A567B; iid[1] = 0x8912E010; // test case 1 = 0xa (19)

	for (i = 1; i < 16; i++) {
		iid_num = (i - 1) / 8; // 0-1
		n = (i - 1) % 8; // 0-7

		if (i == 1) {
			a = 16; // init
		} else {
			a = c * 2;
		};

		b = (a % 17) + ((iid[iid_num] & (0xf << ((7-n)*4))) >> ((7-n)*4));
		c = b % 16;

		// if ( (ipv6calc_debug) != 0 ) {
		//	fprintf(stderr, "%s/%s: checksum calculation of IID: %08x%08x  i=%d a=%d b=%d c=%d\n", __FILE__, __func__, (unsigned int) iid[0], (unsigned int) iid[1], i, a, b, c);
		// };
	};

	/* find checksum xdigit */
	a = c * 2;

	for (checksum = 0; checksum <= 0xf; checksum++) {
		if ( (((a % 17) + checksum) % 16) == 1) {
			break;
		};
	};

	if ( (ipv6calc_debug) != 0 ) {
		fprintf(stderr, "%s/%s: checksum of IID: %08x%08x = %x\n", __FILE__, __func__, (unsigned int) iid[0], (unsigned int) iid[1], checksum);
	};

	return(checksum);
};


/*
 * set checksum for anonymized IID
 *
 * mod:  ipv6addrp  = pointer to IPv6 address structure
 */
void ipv6addr_set_checksum_anonymized_iid(ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t checksum = ipv6addr_checksum_anonymized_iid(ipv6addrp);

	/* checksum is stored in rightmost nibble */
	ipv6addr_setoctet(ipv6addrp, 15, (ipv6addr_getoctet(ipv6addrp, 15) & 0xf0) | checksum);
};


/*
 * verify checksum for anonymized IID
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: 0=ok 1=not ok
 */
int ipv6addr_verify_checksum_anonymized_iid(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t checksum = ipv6addr_checksum_anonymized_iid(ipv6addrp);

	if (checksum == (ipv6addr_getoctet(ipv6addrp, 15) & 0xf)) {
		return(0);
	};

	return(1);
};


/*
 * fuzzy detection of IID is generated by privacy extension
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: 0=probably generated by privacy extension, 1=manual set, -1=global
 */
int ipv6addr_privacyextensiondetection(const ipv6calc_ipv6addr *ipv6addrp, s_iid_statistics *iid_statisticsp) {
	uint32_t iid[2];

	iid[0] = ipv6addr_getdword(ipv6addrp, 2); // 00-31
	iid[1] = ipv6addr_getdword(ipv6addrp, 3); // 32-63

	float m, e;

	int iid_digit[16]; // digit of IID

	int b, i, c, v;

	// debug
	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: Given IID: %08x%08x\n", __FILE__, __func__, (unsigned int) iid[0], (unsigned int) iid[1]);
	};

	// blacklists
	if ((iid[0] & 0x02000000u) == 0x02000000u) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
			fprintf(stderr, "%s/%s: universal/local bit set to: universal (no further privacy detection)\n", __FILE__, __func__);
		};
		return (-1);
	};

	if (((iid[0] & 0x000000ffu) == 0x000000ffu) && ((iid[1] & 0xff000000u) == 0xfe000000u)) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
			fprintf(stderr, "%s/%s: expanded EUI-48 (no further privacy detection)\n", __FILE__, __func__);
		};
		return (-1);
	};

	// clear structure
	iid_statisticsp->hexdigit = 0;
	iid_statisticsp->lls_residual = 0;

	for (c = 0; c < 16; c++) {
		iid_statisticsp->digit_blocks[c] = 0;
		iid_statisticsp->digit_amount[c] = 0;
		iid_statisticsp->digit_amount[c] = 0;
	};

	for (c = 0; c < 31; c++) {
		iid_statisticsp->digit_delta[c] = 0;
	};

	iid_statisticsp->digit_delta_amount = 0;


	/* create statistics */
	for (b = 0; b < 16; b++) {
		v = (iid[b/8] & (0xf << ((7 - (b % 8)) * 4))) >> ((7 - (b % 8)) * 4);
		iid_digit[b] = v;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
			fprintf(stderr, "%s/%s: analyze nibble %2d: %x\n", __FILE__, __func__, b, v);
		};

		iid_statisticsp->digit_amount[v]++;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: distribution\n", __FILE__, __func__);

		fprintf(stderr, "%s/%s: hex distribution: digit   ", __FILE__, __func__);
		for (b = 0; b < 16; b++) {
			fprintf(stderr, "|%2x", b);
		};
		fprintf(stderr, "|\n");
		fprintf(stderr, "%s/%s: hex distribution: count   ", __FILE__, __func__);
		for (b = 0; b < 16; b++) {
			fprintf(stderr, "|%2d", iid_statisticsp->digit_amount[b]);
		};
		fprintf(stderr, "|\n");
	};

	/* calculate variances */
	v = 0;

	/* calculate variance over hexdigits */
	c = 0;
	float variance = 0.0;
	for (b = 0; b < 16; b++) {
		if (iid_statisticsp->digit_amount[b] == 0) {
			continue;
		};
		c++;
		e = iid_statisticsp->digit_amount[b];

		m = 1.0;
		/* compensate universal/local bit = 0 by shifting average */
		if ((b & 0x02) == 0x02) {
			m -= 0.0625;
		} else {
			m += 0.0625;
		};

		if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
			fprintf(stderr, "%s/%s: hexdigit %x: amount=%.0f  exp.avg.=%.4f\n", __FILE__, __func__, b, e, m);
		};

		e = e - m; /* substract related average */
		e = e * e; /* square */
		variance += e;
	};

	variance = sqrt(variance / c);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: variance for hexdigits: %0.5f\n", __FILE__, __func__, variance);
	};

	iid_statisticsp->hexdigit = variance;


	/* calculate linear least square fit to detect sequences */
	float xm = 0, ym = 0, x2 = 0, xy = 0, a0, a1, r, r2 = 0;
	for (b = 0; b < 16; b++) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
			fprintf(stderr, "%s/%s: linear least square calc: x=%0.5f y=%0.5f\n", __FILE__, __func__, (float) b, (float) iid_digit[b]);
		};
		xm += (float) b;
		ym += (float) iid_digit[b];
		xy += (float) b * (float) iid_digit[b];
		x2 += (float) b * (float) b; 
	};
	xm /= 16.0; ym /= 16.0;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: linear least square calc: xm=%0.5f ym=%0.5f\n", __FILE__, __func__, xm, ym);
		fprintf(stderr, "%s/%s: linear least square calc: x2=%0.5f xy=%0.5f\n", __FILE__, __func__, x2, xy);
	};

	a1 = (xy - 16 * xm * ym) / (x2 - 16 * xm * xm);
	a0 = ym - a1 * xm;
	
	for (b = 0; b < 16; b++) {
		r = a0 + a1 * (float) b - (float) iid_digit[b];
		r2 += r * r;
	};

	r = sqrt(r2);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: linear least square result: a0=%0.5f a1=%0.5f r=%05f\n", __FILE__, __func__, a0, a1, r);
	};

	iid_statisticsp->lls_residual = r;


	/* check for repeating digits (digit blocks) */
	c = 0;
	i = iid_digit[0];
	for (b = 1; b < 16; b++) {
		if (i == iid_digit[b]) {
			c++;
		} else {
			iid_statisticsp->digit_blocks[c]++;	
			i = iid_digit[b];
			c = 0;
		};
	};
	iid_statisticsp->digit_blocks[c]++;


	/* analyze delta of neighbor digits (digit delta) */
	for (b = 1; b < 16; b++) {
		v = iid_digit[b] - iid_digit[b-1] + 15;

		if (iid_statisticsp->digit_delta[v] == 0) {
			// count, how many different digit deltas found
			iid_statisticsp->digit_delta_amount++;
		};

		iid_statisticsp->digit_delta[v]++;

	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_privacydetection) != 0 ) {
		fprintf(stderr, "%s/%s: digit blocks: ", __FILE__, __func__);
		for (c = 0; c < 16; c++) {
			fprintf(stderr, "%d:%d ", c+1, iid_statisticsp->digit_blocks[c]);
		};
		fprintf(stderr, "\n");
	};

	/* check against limits */
	if (iid_statisticsp->hexdigit < s_iid_statistics_ok_min.hexdigit || iid_statisticsp->hexdigit > s_iid_statistics_ok_max.hexdigit) {
		return (1);
	} else if (iid_statisticsp->lls_residual < s_iid_statistics_ok_min.lls_residual || iid_statisticsp->lls_residual > s_iid_statistics_ok_max.lls_residual) {
		return (1);
	} else if (iid_statisticsp->digit_delta_amount < s_iid_statistics_ok_min.digit_delta_amount || iid_statisticsp->digit_delta_amount > s_iid_statistics_ok_max.digit_delta_amount) {
		return (1);
	} else {
		for (c = 0; c < 16; c++) {
			// digit blocks
			if (iid_statisticsp->digit_blocks[c] < s_iid_statistics_ok_min.digit_blocks[c] || iid_statisticsp->digit_blocks[c] > s_iid_statistics_ok_max.digit_blocks[c]) {
				return (1);
			};

			// digit amount
			if (iid_statisticsp->digit_amount[c] < s_iid_statistics_ok_min.digit_amount[c] || iid_statisticsp->digit_amount[c] > s_iid_statistics_ok_max.digit_amount[c]) {
				return (1);
			};
		};
		for (c = 0; c < 31; c++) {
			// digit delta
			if (iid_statisticsp->digit_delta[c] < s_iid_statistics_ok_min.digit_delta[c] || iid_statisticsp->digit_delta[c] > s_iid_statistics_ok_max.digit_delta[c]) {
				return (1);
			};
		};

	};

	return (0);
};


/*
 * Get type of an IPv6 address
 *
 * with credits to kernel and USAGI developer team
 * basic code was taken from "kernel/net/ipv6/addrconf.c"
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 */

uint32_t ipv6addr_gettype(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t type = 0, r;
	uint32_t st, st1, st2, st3;
	s_iid_statistics variances;

	st =  ipv6addr_getdword(ipv6addrp, 0); /* 32 MSB */
	st1 = ipv6addr_getdword(ipv6addrp, 1);
	st2 = ipv6addr_getdword(ipv6addrp, 2);
	st3 = ipv6addr_getdword(ipv6addrp, 3); /* 32 LSB */

	/* unspecified address */
	if ( (st == 0) && (st1 == 0) && (st2 == 0) && (st3 == 0) ) {
		type |= IPV6_NEW_ADDR_UNSPECIFIED;
		return (type);
	};

	/* address space information  */
	if ((st & 0xFE000000u) == 0xFC000000u) {
		/* FC00::/7 -> Unique Local IPv6 Unicast Address */
		type |= IPV6_ADDR_ULUA;
	};

	/* address space information  */
	if ((st & 0xE0000000u) == 0x20000000u) {
		/* 2000::/3 -> global unicast */
		type |= IPV6_NEW_ADDR_AGU;
	};
	
	/* address space information  */
	if ((st & 0xFFFF0000u) == 0x3FFE0000u) {
		/* 3ffe::/16 -> experimental 6bone */
		type |= IPV6_NEW_ADDR_6BONE;
	};

	if ((st & 0xFFFF0000u) == 0x20020000u) {
		/* 2002::/16 -> 6to4 tunneling */
		type |= IPV6_NEW_ADDR_6TO4;

		if (	(ipv6addr_getword(ipv6addrp, 3) == 0) &&
			(ipv6addr_getword(ipv6addrp, 4) == 0) &&
			(ipv6addr_getword(ipv6addrp, 5) == 0) &&
			(ipv6addr_getword(ipv6addrp, 6) == ipv6addr_getword(ipv6addrp, 1)) &&
			(ipv6addr_getword(ipv6addrp, 7) == ipv6addr_getword(ipv6addrp, 2)) ) {
			/* 2002:<ipv4addr>::<ipv4addr> -> usually Microsoft does this */
			type |= IPV6_NEW_ADDR_6TO4_MICROSOFT;
		};
	};

	if (st == (uint32_t) 0x3FFE831Fu || st == (uint32_t) 0x20010000u) {
		/* 3ffe:831f::/32 -> Teredo (6bone, older draft) */
		/* 2001:0000::/32 -> Teredo (RFC 4380) */
		type |= IPV6_NEW_ADDR_TEREDO;
	};

	if ((st & 0xFFFFFFF0u) == 0x20010010u) {
		/* 2001:0010::/28 -> ORCHID (RFC 4843) */
		type |= IPV6_NEW_ADDR_ORCHID;
	};
	
	if ((st == 0x0064ff9bu) && (st1 == 0) && (st2 == 0)) {
		/* 64:ff9b::/96 -> NAT64 (RFC 6052) */
		type |= IPV6_NEW_ADDR_NAT64;
	};
	
	if (((type & (IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4)) == 0) && ((st & 0xE0000000u) == 0x20000000u)) {
		/* 2000::/3 -> productive IPv6 address space */
		/*  except 3ffe::/16 (6BONE) and 2002::/16 (6TO4) */
		type |= IPV6_NEW_ADDR_PRODUCTIVE;
	};
	
	if ((st2 == (uint32_t) 0x00000001u) && (st3 & 0xFF000000u) == 0xFF000000u) {
		/* ..:0000:0001:ffxx:xxxx solicited node suffix */
		type |= IPV6_NEW_ADDR_SOLICITED_NODE;
	};

	if (st2 == (uint32_t) 0x00005EFEu && ((type & IPV6_NEW_ADDR_TEREDO) == 0)) {
		/* ..:0000:5EFE:xx.xx.xx.xx ISATAP suffix (RFC 4214) */
		/* but not if TEREDO */
		type |= IPV6_NEW_ADDR_IID_ISATAP;
	};

	/* multicast */
	if ((st & 0xFF000000u) == 0xFF000000u) {
		type |= IPV6_ADDR_MULTICAST;

		switch((st & 0x00FF0000u)) {
			case (0x00010000u):
				type |= IPV6_ADDR_LOOPBACK;
				break;

			case (0x00020000u):
				type |= IPV6_ADDR_LINKLOCAL;
				break;

			case (0x00050000u):
				type |= IPV6_ADDR_SITELOCAL;
				break;
		};
		return (type);
	};

	/* special */
	if ((st | st1) == 0) {
		if (st2 == 0) {
			if (st3 == 0) {
				type |= IPV6_ADDR_ANY;
				return (type);
			};

			if (st3 == (uint32_t) 0x00000001u) {
				type |= IPV6_ADDR_LOOPBACK | IPV6_ADDR_UNICAST;
				return (type);
			};

			type |= IPV6_ADDR_COMPATv4 | IPV6_ADDR_UNICAST;
			return (type);
		}

		if (st2 == (uint32_t) 0x0000ffffu)
			type |= IPV6_ADDR_MAPPED;
			return (type);
	};

	
	/* Consider all addresses with the first three bits different of
	   000 and 111 as unicasts.
	   also link-local,site-local,ULULA
	 */

	if ((((st & 0xE0000000u) != 0x00000000u) && ((st & 0xE0000000u) != 0xE0000000u)) || ((st & 0xFC000000u) == 0xFC000000u)) {
		type |= IPV6_ADDR_UNICAST;

		if ((type & IPV6_NEW_ADDR_TEREDO) != 0) {
			/* teredo has no IID */
			return (type);
		};

		type |= IPV6_NEW_ADDR_IID;

		if ((st & 0xFFC00000u) == 0xFE800000u) {
			type |=  IPV6_ADDR_LINKLOCAL;
			if ( ((st2 == 0x80005445u) && (st3 ==0x5245444fu)) \
			    || ((st2 == 0x0000FFFFu) && (st3 ==0xFFFFFFFDu)) \
			) {
				/* fe80::8000:5445:5245:444F : LSB string: "TEREDO" */
				/* fe80::ffff:ffff:fffd */
				type |= IPV6_NEW_ADDR_LINKLOCAL_TEREDO | IPV6_NEW_ADDR_IID_TEREDO;
			};
		} else if ((st & 0xFFC00000u) == 0xFEC00000u) {
			type |= IPV6_ADDR_SITELOCAL;
		};

		if ((type & IPV6_NEW_ADDR_IID) != 0) {
			/* check IID */
			if ((st2 & 0x02000000u) == 0x02000000u) {
				type |= IPV6_NEW_ADDR_IID_GLOBAL;

				if (((st2 & (uint32_t) 0x000000FFu) == (uint32_t) 0x000000FFu) && ((st3 & (uint32_t) 0xFE000000u) == (uint32_t) 0xFE000000u)) {
					type |= IPV6_NEW_ADDR_IID_EUI48;
				} else {
					type |= IPV6_NEW_ADDR_IID_EUI64;
				};
			} else {
				/* check for anonymized ID */
				if ((st2 & ANON_TOKEN_MASK_00_31) == ANON_TOKEN_VALUE_00_31) {
					if ( (ipv6calc_debug) != 0 ) {
						fprintf(stderr, "%s/%s: probably anonymized IID found\n", __FILE__, __func__);
					};

					/* verify now checksum */
 					if (ipv6addr_verify_checksum_anonymized_iid(ipv6addrp) == 0) {
						if ( (ipv6calc_debug) != 0 ) {
							fprintf(stderr, "%s/%s: checksum ok - anonymized IID found\n", __FILE__, __func__);
						};

						if (((st2 & ANON_IID_PRIVACY_MASK_00_31) == ANON_IID_PRIVACY_VALUE_00_31) && ((st3 & ANON_IID_PRIVACY_MASK_32_63) == ANON_IID_PRIVACY_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_PRIVACY | IPV6_ADDR_ANONYMIZED | IPV6_NEW_ADDR_IID_LOCAL;
							return (type);
						} else if (((st2 & ANON_IID_STATIC_MASK_00_31) == ANON_IID_STATIC_VALUE_00_31) && ((st3 & ANON_IID_STATIC_MASK_32_63) == ANON_IID_STATIC_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_LOCAL | IPV6_ADDR_ANONYMIZED;
							return (type);
						} else if (((st2 & ANON_IID_EUI48_MASK_00_31) == ANON_IID_EUI48_VALUE_00_31) && ((st3 & ANON_IID_EUI48_MASK_32_63) == ANON_IID_EUI48_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_EUI48 | IPV6_ADDR_ANONYMIZED;
							return (type);
						} else if (((st2 & ANON_IID_EUI64_MASK_00_31) == ANON_IID_EUI64_VALUE_00_31) && ((st3 & ANON_IID_EUI64_MASK_32_63) == ANON_IID_EUI64_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_EUI64 | IPV6_ADDR_ANONYMIZED;
							return (type);
						};
					} else {
						if ( (ipv6calc_debug) != 0 ) {
							fprintf(stderr, "%s/%s: checksum WRONG - no anonymized IID found\n", __FILE__, __func__);
						};
					};
				};

				type |= IPV6_NEW_ADDR_IID_LOCAL;

				if ((type & (IPV6_NEW_ADDR_LINKLOCAL_TEREDO | IPV6_NEW_ADDR_IID_ISATAP | IPV6_NEW_ADDR_TEREDO)) == 0) {
					/* fuzzy detection of IID privacy extension */
					r = ipv6addr_privacyextensiondetection(ipv6addrp, &variances);
					if (r == 0) {
						type |= IPV6_NEW_ADDR_IID_PRIVACY;
					} else if (r == 2) {
						type |= IPV6_NEW_ADDR_IID_PRIVACY | IPV6_ADDR_ANONYMIZED;
					} else if (r == 3) {
						type |= IPV6_NEW_ADDR_IID_LOCAL | IPV6_ADDR_ANONYMIZED;
					};
				};
			};
		};
		return (type);
	};

	type |= IPV6_ADDR_RESERVED;
	return (type);
};

/*
 * get registry number of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: assignment number (-1 = no result)
 */
#define DEBUG_function_name "libipv6calc/getregistry"
int ipv6addr_getregistry(const ipv6calc_ipv6addr *ipv6addrp) {
	char resultstring[NI_MAXHOST];
	int i;

	i = libipv6addr_get_registry_string(ipv6addrp, resultstring);

	if (i == 2) {
		return(IPV6_ADDR_REGISTRY_RESERVED);
	} else if (i != 0) {
		return(IPV6_ADDR_REGISTRY_UNKNOWN);
	};

#ifdef SUPPORT_DB_IPV6
	if (strcmp(resultstring, "IANA") == 0) {
		return(IPV6_ADDR_REGISTRY_IANA);
	} else if (strcmp(resultstring, "APNIC") == 0) {
		return(IPV6_ADDR_REGISTRY_APNIC);
	} else if (strcmp(resultstring, "ARIN") == 0) {
		return(IPV6_ADDR_REGISTRY_ARIN);
	} else if (strcmp(resultstring, "RIPENCC") == 0) {
		return(IPV6_ADDR_REGISTRY_RIPE);
	} else if (strcmp(resultstring, "LACNIC") == 0) {
		return(IPV6_ADDR_REGISTRY_LACNIC);
	} else if (strcmp(resultstring, "AFRINIC") == 0) {
		return(IPV6_ADDR_REGISTRY_AFRINIC);
	} else if (strcmp(resultstring, "6BONE") == 0) {
		return(IPV6_ADDR_REGISTRY_6BONE);
	} else if (strcmp(resultstring, "6TO4") == 0) {
		return(IPV6_ADDR_REGISTRY_RESERVED);
	} else {
		return(IPV6_ADDR_REGISTRY_UNKNOWN);
	};
#else
	return(IPV6_ADDR_REGISTRY_UNKNOWN);
#endif
}
#undef DEBUG_function_name


/*
 * Get IPv6 address assignement information as string
 *
 * in : ipv6addrp = pointer to IPv6 address structure
 * mod: resultstring
 * ret: 0: ok, 1: unknown, 2: reserved
 */
#define DEBUG_function_name "libipv6calc/get_registry_string"
int libipv6addr_get_registry_string(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int i;
	int match = -1;

	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	uint32_t ipv6_64_95 = ipv6addr_getdword(ipv6addrp, 2);
	uint32_t ipv6_96_127 = ipv6addr_getdword(ipv6addrp, 3);
	
	uint16_t ipv6_00_15 = ipv6addr_getword(ipv6addrp, 0);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Given ipv6 prefix: %08x%08x\n", DEBUG_function_name, (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);
	};

	if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0) && (ipv6_96_127 == 0)) {
		// :: (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.2)");
		return (2);
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0) && (ipv6_96_127 == 1)) {
		// ::1 (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.3)");
		return (2);
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0)) {
		// ::x.x.x.x (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.5.1)");
		return (2);
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0x0000ffff)) {
		// ::ffff:x.x.x.x (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.5.2)");
		return (2);
	} else if (ipv6_00_31 == 0x20010000) {
		// 2001:0000::/32 (RFC 4380)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4380#6)");
		return (2);
	} else if ((ipv6_00_31 & 0xfffffff0) == 0x20010010) {
		// 2001:0010::/28 (RFC 4843)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4843#2)");
		return (2);
	} else if (ipv6_00_31 == 0x20010db8) {
		// 2001:0db8::/32 (RFC 3849)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC3849#4)");
		return (2);
	} else if ((ipv6_00_15 & 0xffff) == 0x2002) {
		// 2002::/16 (RFC 3056)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC3056#2)");
		return (2);
	} else if ((ipv6_00_15 & 0xfe00) == 0xfc00) {
		// fc00::/7 (RFC 4193)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4193#3.1)");
		return (2);
	} else if ((ipv6_00_15 & 0xffe0) == 0xfe80) {
		// fe80::/10 (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.6)");
		return (2);
	} else if ((ipv6_00_15 & 0xffe0) == 0xfec0) {
		// fec0::/10 (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.5.7)");
		return (2);
	} else if ((ipv6_00_15 & 0xff00) == 0xff00) {
		// ffxx::/8 (RFC 4291)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC4291#2.7)");
		return (2);
	};

#ifdef SUPPORT_DB_IPV6
	for (i = 0; i < (int) ( sizeof(dbipv6addr_assignment) / sizeof(dbipv6addr_assignment[0])); i++) {
		/* run through database array */
		if ( (ipv6_00_31 & dbipv6addr_assignment[i].ipv6mask_00_31) != dbipv6addr_assignment[i].ipv6addr_00_31 ) {
			/* MSB 00-31 do not match */
			continue;
		};

		if ( dbipv6addr_assignment[i].ipv6mask_32_63 != 0 ) {
			if ( (ipv6_32_63 & dbipv6addr_assignment[i].ipv6mask_32_63) != dbipv6addr_assignment[i].ipv6addr_32_63 ) {
				/* MSB 32-63 do not match */
				continue;
			};
		};

		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Found match: prefix=%08x%08x mask=%08x%08x  registry=%s (entry: %d)\n", DEBUG_function_name, \
				(unsigned int) dbipv6addr_assignment[i].ipv6addr_00_31, \
				(unsigned int) dbipv6addr_assignment[i].ipv6addr_32_63, \
				(unsigned int) dbipv6addr_assignment[i].ipv6mask_00_31, \
				(unsigned int) dbipv6addr_assignment[i].ipv6mask_32_63, \
				dbipv6addr_assignment[i].string_registry, i);
		};
		match = i;
	};

	/* result */
	if ( match > -1 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", dbipv6addr_assignment[match].string_registry);
		return(0);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "unknown");
		return(1);
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "%s", "(IPv6 database not compiled in)");
	return(1);
#endif
};
#undef DEBUG_function_name


/*
 * function stores an IPv6 literal address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/addrliteral_to_ipv6addrstruct"
int addrliteral_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, i;
	char tempstring[NI_MAXHOST], tempstring2[NI_MAXHOST], *cptr;
	const char *literalstring = ".ipv6-literal.net";

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name, addrstring);
	};

	/* lowercase string */
	for (i = 0; i <= strlen(addrstring); i++) {
		/* including trailing \0 */
		tempstring2[i] = tolower(addrstring[i]);
	}

	/* search for literal string */
	cptr = strstr(tempstring2, literalstring);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: String lengths addrstring=%d strstr=%d literal=%d\n", DEBUG_function_name, (unsigned int) strlen(addrstring), (unsigned int) strlen(cptr), (unsigned int) strlen(literalstring));
	};

	if (cptr == NULL) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given IPv6 literal address, has no 'ipv6-literal.net' included!");
		return (1);
	};

	if (strlen(cptr) != strlen(literalstring)) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given IPv6 literal address, ends not with 'ipv6-literal.net'!");
		return (1);
	};

	/* copy without literal */
	strncpy(tempstring, addrstring, strlen(addrstring) - strlen(literalstring));

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: String without literal suffix: %s\n", DEBUG_function_name, tempstring);
	};

	/* replace - with : */
	for (i = 0; i < strlen(tempstring); i++) {
		if (tempstring[i] == '-') {
			tempstring[i] = ':';
		} else if (tempstring[i] == 's') {
			tempstring[i] = '%';
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: String converted to non-literal format: %s\n", DEBUG_function_name, tempstring);
	};

	/* call normal IPv6 parsing function */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores an IPv6 address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/addr_to_ipv6addrstruct"
int addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, i, cpoints = 0, ccolons = 0, cxdigits = 0;
	char *addronlystring, *cp, tempstring[NI_MAXHOST], tempstring2[NI_MAXHOST], *cptr, **ptrptr;
	int expecteditems = 0;
	int temp[8];
	unsigned int compat[4];
	uint32_t scope = 0;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name, addrstring);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "Input too long: %s\n", addrstring);
		return (1);
	};

	ipv6addr_clearall(ipv6addrp);

	snprintf(tempstring, sizeof(tempstring) - 1, "%s", addrstring);
	
	/* save prefix length first, if available */
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Call strtok_r, searching for / in %s\n", DEBUG_function_name, tempstring);
	};

	addronlystring = strtok_r(tempstring, "/", ptrptr);
	
	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addrstring);
		return (1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got address only string: %s\n", DEBUG_function_name, addronlystring);
	};

	cp = strtok_r (NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 128 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) i;
		
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: prefix length %u\n", DEBUG_function_name, (unsigned int) ipv6addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
		};
	};

	snprintf(tempstring2, sizeof(tempstring2) - 1, "%s", addronlystring);

	/* save scope ID, if available */
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Call strtok_r, searching for %% in %s\n", DEBUG_function_name, tempstring2);
	};

	addronlystring = strtok_r(tempstring2, "%%", ptrptr);
	
	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addronlystring);
		return (1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got address only string: %s\n", DEBUG_function_name, addronlystring);
	};

	cp = strtok_r (NULL, "%", ptrptr);
	if ( cp != NULL ) {
		ipv6addrp->flag_scopeid = 1;
		snprintf(ipv6addrp->scopeid, sizeof(ipv6addrp->scopeid) - 1, "%s", cp);
		
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: scope ID    : %s\n", DEBUG_function_name, ipv6addrp->scopeid);
			fprintf(stderr, "%s: flag_scopeid: %d\n", DEBUG_function_name, ipv6addrp->flag_scopeid);
		};
	};

	if ((strlen(addronlystring) < 2) || (strlen(addronlystring) > 45)) {
		/* min: :: */
		/* max: ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128 */
		/* max: ffff:ffff:ffff:ffff:ffff:ffff:123.123.123.123 */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given IPv6 address, has not 2 to 45 chars!");
		return (1);
	};

	/* uncompress string, if necessary */
	if (strstr(addronlystring, "::") != NULL) {
		result = compaddr_to_uncompaddr(addronlystring, tempstring);
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Result of uncompressed string: '%s'\n", DEBUG_function_name, tempstring);
		};
		if ( result != 0 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
			retval = 1;
			return (retval);
		};
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Copy string: '%s'\n", DEBUG_function_name, addronlystring);
		};
		strncpy(tempstring, addronlystring, sizeof(tempstring) - 1);
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Check string: '%s'\n", DEBUG_function_name, tempstring);
	};

	/* count ":", "." and xdigits */
	for (i = 0; i < (int) strlen(tempstring); i++) {
		if (tempstring[i] == ':') {
			ccolons++;
		};
		if (tempstring[i] == '.') {
			cpoints++;
		};
		if (isxdigit(tempstring[i])) {
			cxdigits++;
		};
	};

	/* check amount of ":", must be 6 (compat) or 7 (other) */
	if ( ! ( ( ( ccolons == 7 ) && ( cpoints == 0 ) ) ||  ( ( ccolons == 6 ) && ( cpoints == 3 ) ) ) ) {
		if (strstr(addronlystring, "::")) {
			snprintf(resultstring, NI_MAXHOST - 1, "Error in given address expanded to '%s' is not valid!", tempstring);
		} else {
			snprintf(resultstring, NI_MAXHOST - 1, "Error in given address '%s' is not valid!", addrstring);
		};
		retval = 1;
		return (retval);
	};

	/* amount of ":" + "." + xdigits must be length */
	if (ccolons + cpoints + cxdigits != (int) strlen(tempstring)) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given address '%s' is not valid!", tempstring);
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 3; i++ ) {
		compat[i] = 0;
	};

	ipv6addr_clear(ipv6addrp);
	
	if ( ccolons == 6 ) {
		/* compatv4/mapped format */
		expecteditems = 10;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &compat[0], &compat[1], &compat[2], &compat[3]);
		/* check compat */
		for ( i = 0; i <= 3; i++ ) {
			if ( compat[i] > 255 )	{
				snprintf(resultstring, NI_MAXHOST - 1, "Error, given compatv4/mapped address '%s' is not valid on position %d!", addrstring, i);
				retval = 1;
				return (retval);
			};
		};
		temp[6] = (int) (( compat[0] << 8 ) | compat[1]);
		temp[7] = (int) (( compat[2] << 8 ) | compat[3]);
		scope = IPV6_ADDR_COMPATv4;
	} else {
		/* normal format */
		expecteditems = 8;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 7; i++ ) {
		if ( (temp[i] < 0) || (temp[i] > 0xffff) )	{
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given address '%s' is not valid on position %d!", addronlystring, i);
			retval = 1;
			return (retval);
		};
	};
	
	/* copy into structure */
	for ( i = 0; i <= 7; i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Push word %u: %04x\n", DEBUG_function_name, (unsigned int) i, (unsigned int) temp[i]);
		};
		ipv6addr_setword(ipv6addrp, (unsigned int) i, (unsigned int) temp[i]);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: In structure %08x %08x %08x %08x\n", DEBUG_function_name, (unsigned int) ipv6addr_getdword(ipv6addrp, 0), (unsigned int) ipv6addr_getdword(ipv6addrp, 1), (unsigned int) ipv6addr_getdword(ipv6addrp, 2), (unsigned int) ipv6addr_getdword(ipv6addrp, 3));
		fprintf(stderr, "%s: In structure %04x %04x %04x %04x %04x %04x %04x %04x\n", DEBUG_function_name, (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) ipv6addr_getword(ipv6addrp, 1), (unsigned int) ipv6addr_getword(ipv6addrp, 2), (unsigned int) ipv6addr_getword(ipv6addrp, 3), (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
	};
	
	scope = ipv6addr_gettype(ipv6addrp); 

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got type info: 0x%08x\n", DEBUG_function_name, (unsigned int) scope);
	};

	ipv6addrp->scope = scope;
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: First word is: %04x, address info value: %08x\n", DEBUG_function_name, (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) scope);
		fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
	};
	
	ipv6addrp->flag_valid = 1;
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
static int ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	int i;
	char tempstring[NI_MAXHOST];
	
	/* print array */
	if ( (ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) {
		if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "%04x:%04x:%04x:%04x:%04x:%04x:%u.%u.%u.%u", \
				(unsigned int) ipv6addr_getword(ipv6addrp, 0), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 1), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 2), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 3), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 4), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 5), \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
			);
		} else {
			snprintf(tempstring, sizeof(tempstring) - 1, "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", \
				(unsigned int) ipv6addr_getword(ipv6addrp, 0), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 1), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 2), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 3), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 4), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 5), \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
			);
		};
	} else {
		if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", \
				(unsigned int) ipv6addr_getword(ipv6addrp, 0), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 1), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 2), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 3), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 4), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 5), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 6), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 7)  \
			);
		} else {
			snprintf(tempstring, sizeof(tempstring) - 1, "%x:%x:%x:%x:%x:%x:%x:%x", \
				(unsigned int) ipv6addr_getword(ipv6addrp, 0), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 1), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 2), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 3), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 4), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 5), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 6), \
				(unsigned int) ipv6addr_getword(ipv6addrp, 7)  \
			);
		};
	};

	if ((ipv6addrp->flag_prefixuse == 1) && ((formatoptions & FORMATOPTION_literal) == 0)) {
		/* append prefix length */
		snprintf(resultstring, NI_MAXHOST - 1, "%s/%u", tempstring, (unsigned int) ipv6addrp->prefixlength);
	} else {
		if ((formatoptions & FORMATOPTION_literal) != 0) {
			/* replace : by - */
			for (i =0; i < strlen(tempstring); i++) {
				if (tempstring[i] == ':') {
					tempstring[i] = '-';
				};
			};

			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, NI_MAXHOST - 1, "%ss%s.ipv6-literal.net", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, NI_MAXHOST - 1, "%s.ipv6-literal.net", tempstring);
			}
		} else {
			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s%%%s", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
			};
		};
	};

	retval = 0;	
	return (retval);
};


/*
 * stores the prefix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure, formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddrprefix"
static int ipv6addrstruct_to_uncompaddrprefix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* test for misuse */
	if ( ((ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && (ipv6addrp->prefixlength > 96) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, cannot print prefix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 0 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, cannot print prefix of a address with prefix length 0!");
		retval = 1;
		return (retval);
	};

	max = ( (unsigned int) ipv6addrp->prefixlength - 1 ) / 16u;
	i = 0;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%04x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%04x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1) - 1, "%s", tempstring2);
	};
	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring1);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function stores the suffix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddrsuffix"
static int ipv6addrstruct_to_uncompaddrsuffix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* test for misuse */
	if ( ( (ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0) && ( ipv6addrp->prefixlength > 96 ) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, cannot print suffix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 128 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, cannot print suffix of a address with prefix length 128!");
		retval = 1;
		return (retval);
	};

	max = 7;
	i = (unsigned int) ipv6addrp->prefixlength / 16u;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( ( ( ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && ( i == 6 ) ) {
			snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%u.%u.%u.%u", tempstring1, \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
			);
			i = max;
		} else if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%04x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%04x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1) - 1, "%s", tempstring2);
	};
	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring1);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddr"
int libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: get format option: %08x\n", DEBUG_function_name, (unsigned int) formatoptions);
	};

	if ( (formatoptions & FORMATOPTION_printprefix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrprefix(ipv6addrp, resultstring, formatoptions);
	} else if ( (formatoptions & FORMATOPTION_printsuffix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrsuffix(ipv6addrp, resultstring, formatoptions);		
	} else {
		retval = ipv6addrstruct_to_uncompaddr(ipv6addrp, resultstring, formatoptions);		
	};

	if (retval == 0) {
		/* don't modify case on error messages */
		if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
			/* nothing to do */
		} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
			string_to_upcase(resultstring);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * mask prefix bits (set suffix bits to 0)
 * 
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_masksuffix"
void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 127; i >= 0; i--) {
		nbit = (unsigned int) i;
		if (nbit >= (unsigned int) ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = ((uint16_t) 0x8000u) >> (( ((uint16_t) nbit) & ((uint16_t) 0x0fu)));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
				fprintf(stderr, "%s: bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x\n", DEBUG_function_name, nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);
			};

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};
#undef DEBUG_function_name


/*
 * mask suffix bits (set prefix bits to 0) 
 *
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_masksuffix"
void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 127; i >= 0; i--) {
		nbit = (unsigned int) i;

		if (nbit < (unsigned int) ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = ((uint32_t) 0x8000u) >> (((uint32_t) nbit) & ((uint32_t) 0x0fu ));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
				fprintf(stderr, "libipv6addr/ipv6calc_ipv6addr_masksuffix: bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x\n", nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);
			};

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};
#undef DEBUG_function_name


/*
 * function stores an 16 char token into a structure
 *
 * in : *addrstring = 16 char token
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/tokenlsb64_to_ipv6addrstruct"
int tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result;
	int temp[4];
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	if ( strlen(addrstring) != 16 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given token '%s' is not valid (length != 16)!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	result = sscanf(addrstring, "%04x%04x%04x%04x", &temp[0], &temp[1], &temp[2], &temp[3]);
	if ( result != 4 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address '%s', got %d items instead of 4!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring) - 1,  "0:0:0:0:%04x:%04x:%04x:%04x", \
		(unsigned int) temp[0] ,\
		(unsigned int) temp[1], \
		(unsigned int) temp[2], \
		(unsigned int) temp[3]  \
	);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores an interface identifier into a structure
 *
 * in : *addrstring = interface identifier
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/identifier_to_ipv6addrstruct"
int identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, i, ccolons = 0;
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	if ((strlen(addrstring) < 2) || (strlen(addrstring) > 19)) {
		/* min: :: */
		/* max: ffff:ffff:ffff:ffff */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given identifier identifier, has not 2 to 19 chars!");
		retval = 1;
		return (retval);
	};

	/* count ":", must be 2 to 3 */
	for (i = 0; i < (int) strlen(addrstring); i++) {
		if (addrstring[i] == ':') {
			ccolons++;
		};
	};
	if ((ccolons < 2) || (ccolons > 3)) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given identifier '%s' is not valid!", addrstring);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring) - 1, "0:0:0:0:%s", addrstring);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string '%s'\n", DEBUG_function_name, resultstring);
	};

	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_tokenlsb64"
int libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, uint32_t formatoptions) {
	int retval = 1;
	
	/* print array */
	snprintf(resultstring, NI_MAXHOST - 1, "%04x%04x%04x%04x", \
		(unsigned int) ipv6addr_getword(ipv6addrp, 4), \
		(unsigned int) ipv6addr_getword(ipv6addrp, 5), \
		(unsigned int) ipv6addr_getword(ipv6addrp, 6), \
		(unsigned int) ipv6addr_getword(ipv6addrp, 7)  \
	);

	if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
		/* nothing to do */
	} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string '%s'\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * function prints an IPv6 address in native octal format
 *
 * in:  ipv6addr = IPv6 address structure
 * formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_octal"
int libipv6addr_to_octal(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o",
			(unsigned int) ipv6addrp->in6_addr.s6_addr[0],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[1],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[2],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[3],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[4],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[5],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[6],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[7],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[8],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[9],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[10], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[11], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
		);
	} else {
		snprintf(tempstring, sizeof(tempstring) - 1, "\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o",
			(unsigned int) ipv6addrp->in6_addr.s6_addr[0],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[1],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[2],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[3],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[4],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[5],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[6],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[7],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[8],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[9],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[10], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[11], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
		);
	};

	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * function prints an IPv6 address in native hex format
 *
 * in:  ipv6addr = IPv6 address structure
 * formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_hex"
int libipv6addr_to_hex(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	snprintf(tempstring, sizeof(tempstring) - 1, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			(unsigned int) ipv6addrp->in6_addr.s6_addr[0],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[1],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[2],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[3],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[4],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[5],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[6],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[7],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[8],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[9],  \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[10], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[11], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
			(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
		);

	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * retrieve payload of anonymized IID
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: payload (max. 32 bit)
 */
uint32_t ipv6addr_get_payload_anonymized_iid(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t typeinfo) {
	uint32_t iid[2];
	uint32_t payload = 0;

	iid[0] = ipv6addr_getdword(ipv6addrp, 2);
	iid[1] = ipv6addr_getdword(ipv6addrp, 3);

	if ((typeinfo & IPV6_NEW_ADDR_IID_EUI48) == IPV6_NEW_ADDR_IID_EUI48) {
		payload = (iid[1] >> ANON_IID_EUI48_PAYLOAD_SHIFT) & ((2 << ANON_IID_EUI48_PAYLOAD_LENGTH) - 1);
	} else if ((typeinfo & IPV6_NEW_ADDR_IID_EUI64) == IPV6_NEW_ADDR_IID_EUI64) {
		payload = (iid[1] >> ANON_IID_EUI64_PAYLOAD_SHIFT) & ((2 << ANON_IID_EUI64_PAYLOAD_LENGTH) - 1);
	};

	return(payload);
};


/*
 * anonymize IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 *      mask values used depending on address type:
 *      mask_iid (flag)
 *      mask_ipv4 (number of bits) [0-32]
 *      mask_ipv6_prefix (number of bits) [0-64]
 * ret: <void>
 */
#define DEBUG_function_name "libipv6addr/anonymize"
void libipv6addr_anonymize(ipv6calc_ipv6addr *ipv6addrp, const unsigned int mask_iid, const unsigned int mask_ipv6, const unsigned int mask_ipv4) {
	/* anonymize IPv4 address according to settings */
	uint32_t typeinfo;
	uint32_t iid[2];
	char tempstring[NI_MAXHOST];
	char helpstring[NI_MAXHOST];
	int i, j;

	ipv6calc_ipv4addr ipv4addr;

	if ( (ipv6calc_debug) != 0 ) {
		fprintf(stderr, "%s/%s: Anonymize IPv6 address: flags: mask_iid=%d mask_ipv6=%d mask_ipv4=%d\n", __FILE__, __func__, mask_iid, mask_ipv6, mask_ipv4);
	};

	/* get type */
	typeinfo = ipv6addr_gettype(ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		j = 0;
		snprintf(tempstring, sizeof(tempstring) - 1, "TYPE=");
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring) - 1, "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring) - 1, "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				j = 1;
			};
		};
		fprintf(stderr, "%s\n", tempstring);
	};

	if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
		/* extract IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 2 + i));
		};

		libipv4addr_anonymize(&ipv4addr, mask_ipv4);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 2 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
		};
	 };

	if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
		/* extract Teredo client IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 12 + i) ^ 0xff);
		};

		libipv4addr_anonymize(&ipv4addr, mask_ipv4);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i) ^ 0xff);
		};

		/* clear client port */
		ipv6addr_setword(ipv6addrp, 5, 0 ^ 0xffff);
	};

	if ( (typeinfo & (IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4)) != 0 ) {
		/* extract IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 12 + i));
		};

		libipv4addr_anonymize(&ipv4addr, mask_ipv4);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
		};
	};

	/* Interface identifier included */
	if ( ( ((typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		if (ipv6addr_getoctet(ipv6addrp, 11) == 0xff && ipv6addr_getoctet(ipv6addrp, 12) == 0xfe) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: EUI-48 identifier found\n", DEBUG_function_name);
			};
			/* EUI-48 */
			if (mask_iid == 1) {
				/* mask unique ID by zero'izing */
				ipv6addr_setoctet(ipv6addrp, 13, 0x0u);
				ipv6addr_setoctet(ipv6addrp, 14, 0x0u);
				ipv6addr_setoctet(ipv6addrp, 15, 0x0u);
			} else {
				/* set anon interface ID, include shifted OUI bytes */
				if ( (ipv6calc_debug) != 0 ) {
					fprintf(stderr, "%s/%s: Anonymize IPv6 address: OUI=%02x:%02x:%02x\n", __FILE__, __func__, ipv6addr_getoctet(ipv6addrp, 8) & 0xfc, ipv6addr_getoctet(ipv6addrp, 9), ipv6addr_getoctet(ipv6addrp, 10));
				};

				iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_EUI48_VALUE_00_31;
				iid[1] = ANON_IID_EUI48_VALUE_32_63 | ((ipv6addr_getoctet(ipv6addrp, 8) & 0xfc) << (ANON_IID_EUI48_PAYLOAD_SHIFT + 16)) | (ipv6addr_getoctet(ipv6addrp, 9) << (ANON_IID_EUI48_PAYLOAD_SHIFT + 8)) | (ipv6addr_getoctet(ipv6addrp, 10) << (ANON_IID_EUI48_PAYLOAD_SHIFT));

				ipv6addr_setdword(ipv6addrp, 2, iid[0]);
				ipv6addr_setdword(ipv6addrp, 3, iid[1]);
				ipv6addr_set_checksum_anonymized_iid(ipv6addrp);
			};
		} else {
			if ( (typeinfo & IPV6_NEW_ADDR_IID_PRIVACY) == IPV6_NEW_ADDR_IID_PRIVACY ) {
				/* replace IID with special value */
				iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_PRIVACY_VALUE_00_31;
				iid[1] = ANON_IID_PRIVACY_VALUE_32_63;

				ipv6addr_setdword(ipv6addrp, 2, iid[0]);
				ipv6addr_setdword(ipv6addrp, 3, iid[1]);
				ipv6addr_set_checksum_anonymized_iid(ipv6addrp);
			} else if ( (ipv6addr_getoctet(ipv6addrp, 8) & 0x02) != 0 ) {
				/* Check for global EUI-64 */
				if (mask_iid == 1) {
					/* mask unique ID */
					ipv6addr_setoctet(ipv6addrp, 11, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 12, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 13, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 14, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 15, 0x0u);
				} else {
					iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_EUI64_VALUE_00_31;
					iid[1] = ANON_IID_EUI64_VALUE_32_63 | ((ipv6addr_getoctet(ipv6addrp, 8) & 0xfc) << (ANON_IID_EUI64_PAYLOAD_SHIFT + 16)) | (ipv6addr_getoctet(ipv6addrp, 9) << (ANON_IID_EUI64_PAYLOAD_SHIFT + 8)) | (ipv6addr_getoctet(ipv6addrp, 10) << (ANON_IID_EUI64_PAYLOAD_SHIFT));

					ipv6addr_setdword(ipv6addrp, 2, iid[0]);
					ipv6addr_setdword(ipv6addrp, 3, iid[1]);
					ipv6addr_set_checksum_anonymized_iid(ipv6addrp);
				};
			} else {
				if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
					/* zero'ise unique ID */
					ipv6addr_setoctet(ipv6addrp, 13, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 14, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 15, 0x0u);
				} else if ( (typeinfo & IPV6_NEW_ADDR_IID_ISATAP) != 0 )  {
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
					};

					libipv4addr_anonymize(&ipv4addr, mask_ipv4);

					/* store back */
					for (i = 0; i <= 3; i++) {
						ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
					};
				} else if ( ( ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0 && ipv6addr_getword(ipv6addrp, 6) != 0)) )   {
					/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
					};

					libipv4addr_anonymize(&ipv4addr, mask_ipv4);

					/* store back */
					for (i = 0; i <= 3; i++) {
						ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
					};
				} else {
					if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
						/* extract IPv4 address */
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
						};

						libipv4addr_anonymize(&ipv4addr, mask_ipv4);

						/* store back */
						for (i = 0; i <= 3; i++) {
							ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
						};
					} else {
						/* Identifier has local scope */
						if (mask_iid == 1) {
							/* mask IID with special value */
							ipv6addr_setdword(ipv6addrp, 2, 0);
							ipv6addr_setdword(ipv6addrp, 3, 0);
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_STATIC_VALUE_00_31;
							iid[1] = ANON_IID_STATIC_VALUE_32_63;

							ipv6addr_setdword(ipv6addrp, 2, iid[0]);
							ipv6addr_setdword(ipv6addrp, 3, iid[1]);
							ipv6addr_set_checksum_anonymized_iid(ipv6addrp);
						};
					};
				};
			};
		};
	};

	/* ORCHID hash */
	if ( (typeinfo & IPV6_NEW_ADDR_ORCHID) != 0 ) {
		/* mask 100 LSBs */
		ipv6addr_setword(ipv6addrp, 7, 0x0u);
		ipv6addr_setword(ipv6addrp, 6, 0x0u);
		ipv6addr_setword(ipv6addrp, 5, 0x0u);
		ipv6addr_setword(ipv6addrp, 4, 0x0u);
		ipv6addr_setword(ipv6addrp, 3, 0x0u);
		ipv6addr_setword(ipv6addrp, 2, 0x0u);
		ipv6addr_setword(ipv6addrp, 1, ipv6addr_getword(ipv6addrp, 1) & 0xFFF0);
	};

	/* prefix included */
	if ( ((typeinfo & (IPV6_ADDR_SITELOCAL | IPV6_ADDR_ULUA | IPV6_NEW_ADDR_AGU)) != 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		if (mask_ipv6 == 0) {
			/* clear IPv6 address prefix 0-63 */
			ipv6addr_setdword(ipv6addrp, 0, 0u);
			ipv6addr_setdword(ipv6addrp, 1, 0u);
		} else if (mask_ipv6 == 64) {
			/* nothing to do */
		} else if (mask_ipv6 < 1 || mask_ipv6 > 64) {
			/* should not happen here */
			fprintf(stderr, "%s: 'mask_ipv6' has an unexpected illegal value!\n", DEBUG_function_name);
			exit(EXIT_FAILURE);
		} else {
			if (mask_ipv6 < 64 && mask_ipv6 > 32) {
				ipv6addr_setdword(ipv6addrp, 1, ipv6addr_getdword(ipv6addrp, 1) & (0xffffffffu << ((unsigned int) 64 - mask_ipv6)));
			} else if (mask_ipv6 == 32) {
				ipv6addr_setdword(ipv6addrp, 1, 0u);
			} else if (mask_ipv6 < 32 && mask_ipv6 >= 0) {
				ipv6addr_setdword(ipv6addrp, 1, 0u);
				ipv6addr_setdword(ipv6addrp, 0, ipv6addr_getdword(ipv6addrp, 0) & (0xffffffffu << ((unsigned int) 32 - mask_ipv6)));
			};
		};

		/* restore prefix in special cases */
		if ( ((typeinfo & IPV6_ADDR_SITELOCAL) != 0) && (mask_ipv6 < 10) ) { 
			ipv6addr_setword(ipv6addrp, 0, ipv6addr_getword(ipv6addrp, 1) | 0xfec0u);
		} else if ( ((typeinfo & IPV6_ADDR_ULUA) != 0) && (mask_ipv6 < 7) ) {
			ipv6addr_setoctet(ipv6addrp, 0, ipv6addr_getoctet(ipv6addrp, 0) | 0xfdu);
		};
	};

	return;
};
#undef DEBUG_function_name


/*
 * clear filter IPv6 address
 *
 * in : *filter    = filter structure
 */
void ipv6addr_filter_clear(s_ipv6calc_filter_ipv6addr *filter) {
	filter->active = 0;
	filter->typeinfo_must_have = 0;
	filter->typeinfo_may_not_have = 0;
	return;
};


/*
 * parse filter IPv6 address
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int ipv6addr_filter_parse(s_ipv6calc_filter_ipv6addr *filter, const char *token) {
	int i, result = 1, negate = 0, offset = 0;
	const char *prefix = "ipv6";
	const char *prefixdot = "ipv6.";

	if (token == NULL) {
		return (result);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s/%s: input: %s\n", __FILE__, __func__, token);
	};

	if (token[0] == '^') {
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s/%s: found negate prefix in token: %s\n", __FILE__, __func__, token);
		};

		negate = 1;
		offset += 1;
	};

	if (strcmp(token + offset, prefix) == 0) {
		/* any */
		if (negate == 1) {
			filter->typeinfo_may_not_have = ~IPV6_ADDR_ANY;
		} else {
			filter->typeinfo_must_have = IPV6_ADDR_ANY;
		};
		filter->active = 1;
		result = 0;
		goto END_ipv6addr_filter_parse;

	} else if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s/%s: token with prefix, suffix: %s\n", __FILE__, __func__, token + offset);
		};

	} else if (strstr(token, ".") != NULL) {
		/* other prefix */
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s/%s: prefix did not match: %s\n", __FILE__, __func__, token + offset);
		};
		return(1);
	};

	for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s/%s: check token against: %s\n", __FILE__, __func__, ipv6calc_ipv6addrtypestrings[i].token);
		};

		if (strcmp(ipv6calc_ipv6addrtypestrings[i].token, token + offset) == 0) {
			if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
				fprintf(stderr, "%s/%s: token match: %s\n", __FILE__, __func__, ipv6calc_ipv6addrtypestrings[i].token);
			};

			if (negate == 1) {
				filter->typeinfo_may_not_have |= ipv6calc_ipv6addrtypestrings[i].number;
			} else {
				filter->typeinfo_must_have |= ipv6calc_ipv6addrtypestrings[i].number;
			};
			filter->active = 1;
			result = 0;
			break;
		};
	};

	if (result != 0) {
		if ((ipv6calc_debug & DEBUG_libipv6addr) != 0) {
			fprintf(stderr, "%s/%s: token not supported: %s\n", __FILE__, __func__, token);
		};
		return (result);
	};

END_ipv6addr_filter_parse:
	if ((ipv6calc_debug & DEBUG_libipv6addr) != 0) {
		fprintf(stderr, "%s/%s: filter 'must_have'         : 0x%08x\n", __FILE__, __func__, filter->typeinfo_must_have);
		fprintf(stderr, "%s/%s: filter 'may_not_have'      : 0x%08x\n", __FILE__, __func__, filter->typeinfo_may_not_have);
		fprintf(stderr, "%s/%s: filter 'active'            : %d\n", __FILE__, __func__, filter->active);
	};

	return (result);
};


/*
 * filter IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 * in : *filter    = filter structure
 * ret: 0=match 1=not match
 */
int ipv6addr_filter(const ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_filter_ipv6addr *filter) {
	uint32_t typeinfo;
	int result = 1;

	if (filter->active == 0) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s/%s: no filter active (SKIP)\n", __FILE__, __func__);
		};
		return (1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s/%s: start\n", __FILE__, __func__);
	};

	/* get type */
	typeinfo = ipv6addr_gettype(ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s/%s: compare typeinfo against must_have: 0x%08x/0x%08x\n", __FILE__, __func__, typeinfo, filter->typeinfo_must_have);
	};

	if ((typeinfo & filter->typeinfo_must_have) == filter->typeinfo_must_have) {
		if ((typeinfo & filter->typeinfo_may_not_have) == 0) {
			result = 0;
		};
	};

	return (result);
};
