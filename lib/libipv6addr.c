/*
 * Project    : ipv6calc
 * File       : libipv6addr.c
 * Version    : $Id$
 * Copyright  : 2001-2019 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 * License    : GNU GPL v2
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
#include "libieee.h"
#include "libeui64.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"


/*
 * function returns an octet of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numoctet  = number of octet (0 = MSB, 15 = LSB)
 * additional: calls exit on out of range
 */
uint8_t ipv6addr_getoctet(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet) {
	uint8_t retval;
	
	if ( numoctet > 15 ) {
		ERRORPRINT_WA("given octet number '%u' is out of range!", numoctet);
		exit(EXIT_FAILURE);
	};

	retval = ipv6addrp->in6_addr.s6_addr[numoctet];

	return (retval);
};


/*
 * function returns a word of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * additional: calls exit on out of range
 */
uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 7 ) {
		ERRORPRINT_WA("given word number '%u' is out of range!", numword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numword * 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] );

	return (retval);
};


/*
 * function returns a dword of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword) {
	uint32_t retval;
	
	if ( numdword > 3 ) {
		ERRORPRINT_WA("given dword number '%u' is out of range!", numdword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numdword * 4] << 24 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] << 16 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] ); 

	return (retval);
};


/*
 * function sets an octet of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numoctet   = number of word (0 = MSB, 15 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv6addr_setoctet(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet, const unsigned int value) {
	
	if ( numoctet > 15 ) {
		ERRORPRINT_WA("given octet number '%u' is out of range!", numoctet);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000000ff ) {
		ERRORPRINT_WA("given value '%x' is out of range!", value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numoctet] = (uint8_t) value;

	return;
};


/*
 * function sets a word of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, const unsigned int value) {
	if ( numword > 7 ) {
		ERRORPRINT_WA("given word number '%u' is out of range!", numword);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000ffffu ) {
		ERRORPRINT_WA("given value '%x' is out of range!", value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numword * 2    ] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};


/*
 * function sets a dword of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, const unsigned int value) {
	
	if ( numdword > 3 ) {
		ERRORPRINT_WA("given dword number '%u' is out of range!", numdword);
		exit(EXIT_FAILURE);
	};

	if ( value > 0xffffffffu ) {
		ERRORPRINT_WA("given value '%x' is out of range!", value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numdword * 4    ] = (uint8_t) ( ( value & 0xff000000 ) >> 24 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] = (uint8_t) ( ( value & 0x00ff0000 ) >> 16 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp) {
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6addrp->in6_addr.s6_addr); i++) {
		ipv6addrp->in6_addr.s6_addr[i] = 0;
	};
	
	/* Clear IPv6 address scope */
	ipv6addrp->typeinfo = 0;
	ipv6addrp->typeinfo2 = 0;
	ipv6addrp->flag_typeinfo = 0;

	/* Clear valid flag */
	ipv6addrp->flag_valid = 0;

	/* Clear test mode */
	ipv6addrp->test_mode = 0;

	return;
};


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp) {
	ipv6addr_clear(ipv6addrp);

	/* Clear other field */
	ipv6addrp->bit_start = 1;
	ipv6addrp->bit_end = 128;
	ipv6addrp->flag_startend_use = 0;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->prefixlength = 0;
	ipv6addrp->prefix2length = 0;
	ipv6addrp->flag_valid = 0;
	ipv6addrp->flag_scopeid = 0;

	return;
};


/*
 * function copies the IPv6 structure
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * mod: ipv6addrp2 = pointer to IPv6 address structure
 */
void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src) {

	*(ipv6addrp_dst) = *(ipv6addrp_src);
	
	return;
};


/*
 * function compares the IPv6 structure
 *
 * in:  ipv6addrp1  = pointer to IPv6 address structure
 * in:  ipv6addrp2  = pointer to IPv6 address structure
 * in:  compare_flags:
 *  0=less than/equal/greater than
 *  1=honor prefix length on addr2
 *
 * returns: 0: addr1 equal with addr2, 1: addr1 > addr2, -1: addr1 < addr2 (compare_flags == 0)
 * returns: 0: addr1 equal with addr2 or covered by addr2/prefix (compare_flags == 1)
 */
int ipv6addr_compare(const ipv6calc_ipv6addr *ipv6addrp1, const ipv6calc_ipv6addr *ipv6addrp2, const uint16_t compare_flags) {
	int i;
	uint32_t mask;

	DEBUGPRINT_WA(DEBUG_libipv6addr, "compare addr1 with addr2 (compare flags: %08x)", compare_flags);

	if (compare_flags == 1) {
		for (i = 0; i < 4; i++) {
			if ((ipv6addrp2->flag_prefixuse == 0)
			  || ((ipv6addrp2->flag_prefixuse == 1) && (ipv6addrp2->prefixlength >= (i + 1) * 32))) {
				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare dword %i (prefixuse=%d): %08x <-> %08x", i, ipv6addrp2->flag_prefixuse, ipv6addr_getdword(ipv6addrp2, i), ipv6addr_getdword(ipv6addrp1, i));
				/* compare 32 bits */
				if (ipv6addr_getdword(ipv6addrp2, i) != ipv6addr_getdword(ipv6addrp1, i)) {
					return(1);
				};
			} else if (ipv6addrp2->flag_prefixuse == 1) {
				mask = ~(0xffffffffu >> (ipv6addrp2->prefixlength - i * 32));
				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare dword %i with mask 0x%08x: %08x <-> %08x", i, mask, (ipv6addr_getdword(ipv6addrp2, i) & mask), (ipv6addr_getdword(ipv6addrp1, i) & mask));
				if ((ipv6addr_getdword(ipv6addrp2, i) & mask) != (ipv6addr_getdword(ipv6addrp1, i) & mask)) {
					return(1);
				} else {
					return(0);
				};
			};
		};
	} else if (compare_flags == 0) {
		for (i = 0; i < 4; i++) {
			if (ipv6addr_getdword(ipv6addrp1, i) > ipv6addr_getdword(ipv6addrp2, i)) {
				return(1);
			} else if (ipv6addr_getdword(ipv6addrp1, i) < ipv6addr_getdword(ipv6addrp2, i)) {
				return(-1);
			};
		};
	};

	return(0);
};


/*
 * create/verify checksum for anonymized qword
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * in:  flag       = ANON_CHECKSUM_FLAG_CREATE|ANON_CHECKSUM_FLAG_VERIFY
 * in:  qword      = qword (64-bit selection)
 * out: ANON_CHECKSUM_FLAG_CREATE: 4-bit checksum   ANON_CHECKSUM_FLAG_VERIFY: 0:ok/1:not-ok
 *
 * using same calculation as for ISAN: ISO 7064, MOD 17,16
 * http://www.pruefziffernberechnung.de/I/ISAN.shtml
 */
uint32_t ipv6addr_checksum_anonymized_qword(const ipv6calc_ipv6addr *ipv6addrp, const int flag, const int qword) {
	uint32_t dword[2];
	uint32_t checksum = 0;
	int i, n, index, i_max = 16;
	unsigned int s;
	uint32_t a, b, c = 0;

	DEBUGPRINT_NA(DEBUG_libipv6addr, "Called");

	dword[0] = ipv6addr_getdword(ipv6addrp, (qword << 1)); // 00-31 (8 nibbles)
	dword[1] = ipv6addr_getdword(ipv6addrp, (qword << 1) + 1); // 32-63 (8 nibbles, only 7 nibbles are used for calculation)

	if (flag == ANON_CHECKSUM_FLAG_CREATE) {
		i_max = 15;
	};

	for (i = 1; i <= i_max; i++) {
		index = (i - 1) / 8; // 0-1
		n = (i - 1) % 8; // 0-7
		s = (7 - n) * 4;

		if (i == 1) {
			a = 16; // init
		} else {
			a = c * 2;
		};

		b = (a % 17) + ((dword[index] & (0xf << s)) >> s);
		c = b % 16;

		DEBUGPRINT_WA(DEBUG_libipv6addr, "checksum calculation of qword: %08x %08x  i=%02d a=%02d b=%02d c=%02d", (unsigned int) dword[0], (unsigned int) dword[1], i, a, b, c);
	};

	if (flag == ANON_CHECKSUM_FLAG_VERIFY) {
		// return code depending on result
		if (c == 1) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum verification OK");
			return(0);
		};

		DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum verification FAILED");
		return(1);
	};

	/* find checksum xdigit */
	a = c * 2;

	for (checksum = 0; checksum <= 0xf; checksum++) {
		if ( (((a % 17) + checksum) % 16) == 1) {
			break;
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "checksum of 64 bits: %08x %08x = %x", (unsigned int) dword[0], (unsigned int) dword[1], checksum);

	return(checksum);
};


/*
 * set checksum for anonymized prefix
 *
 * mod:  ipv6addrp  = pointer to IPv6 address structure
 */
void ipv6addr_set_checksum_anonymized_prefix(ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t checksum = ipv6addr_checksum_anonymized_qword(ipv6addrp, ANON_CHECKSUM_FLAG_CREATE, 0);

	/* checksum is stored in rightmost nibble */
	ipv6addr_setoctet(ipv6addrp, 7, (ipv6addr_getoctet(ipv6addrp, 7) & 0xf0) | checksum);
};


/*
 * verify checksum for anonymized prefix
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: 0=ok 1=not ok
 */
int ipv6addr_verify_checksum_anonymized_prefix(const ipv6calc_ipv6addr *ipv6addrp) {
	return (ipv6addr_checksum_anonymized_qword(ipv6addrp, ANON_CHECKSUM_FLAG_VERIFY, 0));
};


/*
 * set checksum for anonymized IID
 *
 * mod:  ipv6addrp  = pointer to IPv6 address structure
 */
void ipv6addr_set_checksum_anonymized_iid(ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t checksum = ipv6addr_checksum_anonymized_qword(ipv6addrp, ANON_CHECKSUM_FLAG_CREATE, 1);

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
	return (ipv6addr_checksum_anonymized_qword(ipv6addrp, ANON_CHECKSUM_FLAG_VERIFY, 1));
};


/*
 * fuzzy detection of IID is random generated (e.g. by privacy extension)
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * out: 0=probably random generated (e.g. by privacy extension), 1=manual set, -1=global, 2=unknown
 */
int ipv6addr_iidrandomdetection(const ipv6calc_ipv6addr *ipv6addrp, s_iid_statistics *iid_statisticsp) {
	uint32_t iid[2];

	iid[0] = ipv6addr_getdword(ipv6addrp, 2); // 00-31
	iid[1] = ipv6addr_getdword(ipv6addrp, 3); // 32-63

	int result = 2;

	float m, e;

	int iid_digit[16]; // digit of IID

	int b, i, c, v;

	// debug
	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "given IID: %08x%08x",(unsigned int) iid[0], (unsigned int) iid[1]);

	// blacklists
	if ((iid[0] & 0x02000000u) == 0x02000000u) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "universal/local bit set to: universal (no further random detection)");
		result = -1;
		goto END_ipv6addr_iidrandomdetection;
	};

	if (((iid[0] & 0x000000ffu) == 0x000000ffu) && ((iid[1] & 0xff000000u) == 0xfe000000u)) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "expanded EUI-48 (no further random detection)");
		result = -1;
		goto END_ipv6addr_iidrandomdetection;
	};

	// clear structure
	iid_statisticsp->hexdigit = 0;
	iid_statisticsp->lls_residual = 0;

	for (c = 0; c < 16; c++) {
		iid_statisticsp->digit_blocks[c] = 0;
		iid_statisticsp->digit_blocks_hexdigit[c] = 0;
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

		DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "analyze nibble %2d: %x", b, v);

		iid_statisticsp->digit_amount[v]++;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_iidrandomdetection) != 0 ) { // ipv6calc_debug usage ok
		DEBUGPRINT_NA(DEBUG_libipv6addr, "distribution");

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

		DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "hexdigit %x: amount=%.0f  exp.avg.=%.4f", b, e, m);

		e = e - m; /* substract related average */
		e = e * e; /* square */
		variance += e;
	};

	variance = sqrt(variance / c);

	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "variance for hexdigits: %0.5f", variance);

	iid_statisticsp->hexdigit = variance;


	/* calculate linear least square fit to detect sequences */
	float xm = 0, ym = 0, x2 = 0, xy = 0, a0, a1, r, r2 = 0;
	for (b = 0; b < 16; b++) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "linear least square calc: x=%0.5f y=%0.5f", (float) b, (float) iid_digit[b]);
		xm += (float) b;
		ym += (float) iid_digit[b];
		xy += (float) b * (float) iid_digit[b];
		x2 += (float) b * (float) b; 
	};
	xm /= 16.0; ym /= 16.0;

	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "linear least square calc: xm=%0.5f ym=%0.5f", xm, ym);
	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "linear least square calc: x2=%0.5f xy=%0.5f", x2, xy);

	a1 = (xy - 16 * xm * ym) / (x2 - 16 * xm * xm);
	a0 = ym - a1 * xm;
	
	for (b = 0; b < 16; b++) {
		r = a0 + a1 * (float) b - (float) iid_digit[b];
		r2 += r * r;
	};

	r = sqrt(r2);

	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "linear least square result: a0=%0.5f a1=%0.5f r=%05f", a0, a1, r);

	iid_statisticsp->lls_residual = r;


	/* check for repeating digits (digit blocks) */
	c = 0;
	i = iid_digit[0];
	for (b = 1; b < 16; b++) {
		if (i == iid_digit[b]) {
			c++;
		} else {
			if (c > 0) {
				iid_statisticsp->digit_blocks_hexdigit[i]++;
			};
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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_iidrandomdetection) != 0 ) { // ipv6calc_debug usage ok
		fprintf(stderr, "%s/%s: digit blocks: ", __FILE__, __func__);
		for (c = 0; c < 16; c++) {
			fprintf(stderr, "%d:%d ", c+1, iid_statisticsp->digit_blocks[c]);
		};
		fprintf(stderr, "\n");

		fprintf(stderr, "%s/%s: hex distribution in blocks: digit   ", __FILE__, __func__);
		for (b = 0; b < 16; b++) {
			fprintf(stderr, "|%2x", b);
		};
		fprintf(stderr, "|\n");
		fprintf(stderr, "%s/%s: hex distribution in blocks: count   ", __FILE__, __func__);
		for (b = 0; b < 16; b++) {
			fprintf(stderr, "|%2d", iid_statisticsp->digit_blocks_hexdigit[b]);
		};
		fprintf(stderr, "|\n");
	};

	/* check against limits */
	if (iid_statisticsp->hexdigit < s_iid_statistics_ok_min.hexdigit || iid_statisticsp->hexdigit > s_iid_statistics_ok_max.hexdigit) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "min/max hexdigit variance limit reached: %f min=%f max=%f", iid_statisticsp->hexdigit, s_iid_statistics_ok_min.hexdigit, s_iid_statistics_ok_max.hexdigit);
		result = 1;
		goto END_ipv6addr_iidrandomdetection;

	} else if (iid_statisticsp->lls_residual < s_iid_statistics_ok_min.lls_residual || iid_statisticsp->lls_residual > s_iid_statistics_ok_max.lls_residual) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max lls_residual limit reached");
		result = 1;
		goto END_ipv6addr_iidrandomdetection;

	} else if (iid_statisticsp->digit_delta_amount < s_iid_statistics_ok_min.digit_delta_amount || iid_statisticsp->digit_delta_amount > s_iid_statistics_ok_max.digit_delta_amount) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max digit_delta_amount reached");
		result = 1;
		goto END_ipv6addr_iidrandomdetection;

	} else {
		for (c = 0; c < 16; c++) {
			// digit blocks
			if (iid_statisticsp->digit_blocks[c] < s_iid_statistics_ok_min.digit_blocks[c] || iid_statisticsp->digit_blocks[c] > s_iid_statistics_ok_max.digit_blocks[c]) {
				DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max digit_blocks reached");
				result = 1;
				goto END_ipv6addr_iidrandomdetection;
			};

			// digit blocks hexdigits
			if (iid_statisticsp->digit_blocks_hexdigit[c] < s_iid_statistics_ok_min.digit_blocks_hexdigit[c] || iid_statisticsp->digit_blocks_hexdigit[c] > s_iid_statistics_ok_max.digit_blocks_hexdigit[c]) {
				DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max digit_blocks_hexdigit reached");
				result = 1;
				goto END_ipv6addr_iidrandomdetection;
			};

			// digit amount
			if (iid_statisticsp->digit_amount[c] < s_iid_statistics_ok_min.digit_amount[c] || iid_statisticsp->digit_amount[c] > s_iid_statistics_ok_max.digit_amount[c]) {
				DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max digit_amount reached");
				result = 1;
				goto END_ipv6addr_iidrandomdetection;
			};
		};
		for (c = 0; c < 31; c++) {
			// digit delta
			if (iid_statisticsp->digit_delta[c] < s_iid_statistics_ok_min.digit_delta[c] || iid_statisticsp->digit_delta[c] > s_iid_statistics_ok_max.digit_delta[c]) {
				DEBUGPRINT_NA(DEBUG_libipv6addr_iidrandomdetection, "min/max digit_delta reached");
				result = 1;
				goto END_ipv6addr_iidrandomdetection;
			};
		};

	};

	result = 0;

END_ipv6addr_iidrandomdetection:
	DEBUGPRINT_WA(DEBUG_libipv6addr_iidrandomdetection, "result=%d", result);
	return (result);
};


/*
 * Set type of an IPv6 address
 *
 * with credits to kernel and USAGI developer team
 * basic code was taken from "kernel/net/ipv6/addrconf.c"
 *
 * in : ipv6addrp = pointer to IPv6 address structure
 * mod: typeinfo, typeinfo2, prefix2length
 */
void ipv6addr_settype(ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t type = 0, r, type2 = 0;
	uint32_t st, st1, st2, st3;
	s_iid_statistics variances;
	int p;
	uint32_t mask_0_15, mask_16_31;

	uint32_t as_num32, cc_index;
	int r1, r2;

	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	st =  ipv6addr_getdword(ipv6addrp, 0); /* 32 MSB */
	st1 = ipv6addr_getdword(ipv6addrp, 1);
	st2 = ipv6addr_getdword(ipv6addrp, 2);
	st3 = ipv6addr_getdword(ipv6addrp, 3); /* 32 LSB */

	DEBUGPRINT_NA(DEBUG_libipv6addr, "Called");

	/* unspecified address */
	if ( (st == 0) && (st1 == 0) && (st2 == 0) && (st3 == 0) ) {
		type |= IPV6_NEW_ADDR_UNSPECIFIED;
		goto END_ipv6addr_gettype;
	};

	/* address space information  */
	if ((st & 0xFE000000u) == 0xFC000000u) {
		/* FC00::/7 -> Unique Local IPv6 Unicast Address */
		type |= IPV6_ADDR_ULUA;
	};

	if (st == 0x261000d0) {
		/* 2610:00d0::/32 -> LISP (RFC 6830) */
		type2 |= IPV6_ADDR_TYPE2_LISP;
	};

	if ((st == 0x2001067c) && ((st1 & 0xffff0000u) == 0x01980000u)) {
		/* 2001:67c:198::/48 -> LISP PETR (RFC 6830) */
		type2 |= IPV6_ADDR_TYPE2_LISP | IPV6_ADDR_TYPE2_LISP_PETR;
		type |= IPV6_ADDR_ANYCAST;
	};

	if ((st == 0x2001067c) && ((st1 & 0xffff0000u) == 0x00280000u)) {
		/* 2001:67c:28::/48 -> LISP Map Resolver (RFC 6830) */
		type2 |= IPV6_ADDR_TYPE2_LISP | IPV6_ADDR_TYPE2_LISP_MAP_RESOLVER;
		type |= IPV6_ADDR_ANYCAST;
	};

	if (st == 0x20010db8) {
		/* 2001:db8::/32 -> prefix for documentation (RFC 3849) */
		type |= IPV6_ADDR_RESERVED;
	};

	if (UNPACK_XMS(st, ANON_PREFIX_TOKEN_XOR, ANON_PREFIX_TOKEN_MASK, ANON_PREFIX_TOKEN_SHIFT) == ANON_PREFIX_TOKEN_VALUE) {
		// anonymized prefix ?
		DEBUGPRINT_WA(DEBUG_libipv6addr, " probably anonymized prefix found: %04x:%04x:%04x:%04x", U32_MSB16(st), U32_LSB16(st), U32_MSB16(st1), U32_LSB16(st1));

		/* verify now checksum */
		if (ipv6addr_verify_checksum_anonymized_prefix(ipv6addrp) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum ok - anonymized prefix found");

			type |= IPV6_NEW_ADDR_AGU | IPV6_ADDR_UNICAST | IPV6_ADDR_ANONYMIZED_PREFIX;

			r1 = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_CCINDEX, &cc_index);
			r2 = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_ASN32, &as_num32);

			if ((r1 == 0) && (r2 == 0) && (cc_index == COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN + REGISTRY_6BONE)) {
				type |= IPV6_NEW_ADDR_6BONE;
			} else if ((r1 == 0) && (r2 == 0) && (cc_index == 0x3fd)) {
				type |= IPV6_NEW_ADDR_PRODUCTIVE;
				type2 |= IPV6_ADDR_TYPE2_LISP;
			} else {
				type |= IPV6_NEW_ADDR_PRODUCTIVE;
			};
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum NOT ok - no anonymized prefix found");
		};
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
			type |= IPV6_ADDR_IID_32_63_HAS_IPV4;
		};
	};

	if (st == (uint32_t) 0x3FFE831Fu || st == (uint32_t) 0x20010000u) {
		/* 3ffe:831f::/32 -> Teredo (6bone, older draft) */
		/* 2001:0000::/32 -> Teredo (RFC 4380) */
		type |= IPV6_NEW_ADDR_TEREDO;
		if (ipv6addr_getword(ipv6addrp, 5) == 0xffffu) {
			// port=0, done by anonymization
			type |= IPV6_ADDR_ANONYMIZED_IID;
		};
	};

	if (((st & 0xFFFFFFF0u) == 0x20010010u) || ((st & 0xFFFFFFF0u) == 0x20010020u)) {
		/* 2001:0010::/28 -> ORCHID (RFC 4843) */
		/* 2001:0020::/28 -> ORCHIDv2 (RFC 7343) */
		type |= IPV6_NEW_ADDR_ORCHID;
		/* ORCHID has no IID, only a 100 bit encoded hash */

		/* check for anonymized hash */
		if ((st2 & ANON_TOKEN_MASK_00_31) == (ANON_TOKEN_VALUE_00_31 & ANON_TOKEN_MASK_00_31)) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "probably anonymized ORCHID found");

			/* verify now checksum */
			if (ipv6addr_verify_checksum_anonymized_iid(ipv6addrp) == 0) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum ok - anonymized ORCHID found");

				type |= IPV6_ADDR_ANONYMIZED_IID;

			} else {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum NOT ok - no anonymized ORCHID found");
			};
		};
		goto END_ipv6addr_gettype;
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

	if (((st2 & 0xFDFFFF00u) == (uint32_t) 0x00005E00u) && ((type & IPV6_NEW_ADDR_TEREDO) == 0)) {
		/* ISATAP (RFC 4214/5214), but not if TEREDO */
		/* ..:0x00:5EFE:xx.xx.xx.xx ISATAP IID with private IPv4 address */
		/* ..:0x00:5EFE:xx.xx.xx.xx ISATAP IID with public IPv4 address */
		/* ..:0x00:5EFF:FExx:xxxx   ISATAP IID with vendor ID */
		/* ..:0x00:5Exx:xxxx:xxxx   ISATAP IID with extension ID */
		/* x & 0x2 == 0x2:global, x & 0x02 == 0:local */
		type |= IPV6_NEW_ADDR_IID_ISATAP;

		if ((st2 & 0x02000000u) == 0x02000000u) {
			type |= IPV6_NEW_ADDR_IID_GLOBAL;
		} else {
			type |= IPV6_NEW_ADDR_IID_LOCAL;
		};

		if ((st2 & 0x000000ffu) == 0x000000feu) {
			type |= IPV6_ADDR_IID_32_63_HAS_IPV4;
		};
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
		goto END_ipv6addr_gettype;
	};

	/* special */
	if ((st | st1) == 0) {
		if (st2 == 0) {
			if (st3 == 0) {
				type |= IPV6_ADDR_ANY;
				goto END_ipv6addr_gettype;
			};

			if (st3 == (uint32_t) 0x00000001u) {
				type |= IPV6_ADDR_LOOPBACK | IPV6_ADDR_UNICAST;
				goto END_ipv6addr_gettype;
			};

			type |= IPV6_ADDR_COMPATv4 | IPV6_ADDR_UNICAST;
		}

		if (st2 == (uint32_t) 0x0000ffffu)
			type |= IPV6_ADDR_MAPPED;
	};

	// check for included anonymized IPv4 address
	if ((type & (IPV6_ADDR_COMPATv4| IPV6_ADDR_MAPPED | IPV6_NEW_ADDR_NAT64 | IPV6_NEW_ADDR_6TO4)) != 0) {
		ipv6addr_copy(&ipv6addr, ipv6addrp);
		ipv6addr.typeinfo = type; // store what we already have

		r = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
		if (r == 0) {
			if ((ipv4addr.typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
				if ((type & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) != 0) {
					type |= IPV6_ADDR_ANONYMIZED_PREFIX;
				};
				if ((type & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_IID) != 0) {
					type |= IPV6_ADDR_ANONYMIZED_IID;
				};
			};
		};
	} else if ((type & (IPV6_NEW_ADDR_TEREDO)) != 0) {
		// check client IP
		ipv6addr_copy(&ipv6addr, ipv6addrp);
		ipv6addr.typeinfo = type; // store what we already have

		r = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
		if (r == 0) {
			if ((ipv4addr.typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
				type |= IPV6_ADDR_ANONYMIZED_IID;
			};
		};

		// check server IP
		ipv6addr_copy(&ipv6addr, ipv6addrp);
		ipv6addr.typeinfo = type; // store what we already have

		r = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, IPV6_ADDR_SELECT_IPV4_TEREDO_SERVER);
		if (r == 0) {
			if ((ipv4addr.typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
				type |= IPV6_ADDR_ANONYMIZED_PREFIX;
			};
		};
	};

	
	/* Consider all addresses with the first three bits different of
	   000 and 111 as unicasts.
	   also link-local,site-local,ULULA
	   except LISP anycast
	 */
	if ((((st & 0xE0000000u) != 0x00000000u) && ((st & 0xE0000000u) != 0xE0000000u)) || ((st & 0xFC000000u) == 0xFC000000u)) {
		if ((type2 & (IPV6_ADDR_TYPE2_LISP_PETR | IPV6_ADDR_TYPE2_LISP_MAP_RESOLVER)) == 0) {
			type |= IPV6_ADDR_UNICAST;
		};

		if ((type & IPV6_NEW_ADDR_TEREDO) != 0) {
			/* teredo has no IID */
			goto END_ipv6addr_gettype;
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

				if ((type & IPV6_NEW_ADDR_IID_ISATAP) != 0) {
					/* ISATAP is handled above */
				} else {
					if (((st2 & (uint32_t) 0x000000FFu) == (uint32_t) 0x000000FFu) && ((st3 & (uint32_t) 0xFE000000u) == (uint32_t) 0xFE000000u)) {
						type |= IPV6_NEW_ADDR_IID_EUI48;
					} else {
						type |= IPV6_NEW_ADDR_IID_EUI64;
					};
				};
			} else {
				if ((type & IPV6_NEW_ADDR_IID_ISATAP) != 0) {
					/* ISATAP is handled above */
				} else if (((st2 & (uint32_t) 0x000000FFu) == (uint32_t) 0x000000FFu) && ((st3 & (uint32_t) 0xFE000000u) == (uint32_t) 0xFE000000u)) {
					/* EUI-48 local scope based */
					type |= IPV6_NEW_ADDR_IID_EUI48 | IPV6_NEW_ADDR_IID_LOCAL;
				};

				DEBUGPRINT_WA(DEBUG_libipv6addr, "check for anonymized IID: %04x:%04x:%04x:%04x", U32_MSB16(st2), U32_LSB16(st2), U32_MSB16(st3), U32_LSB16(st3));

				/* check for anonymized IID */
				if ((st2 & ANON_TOKEN_MASK_00_31) == (ANON_TOKEN_VALUE_00_31 & ANON_TOKEN_MASK_00_31)) {
					DEBUGPRINT_NA(DEBUG_libipv6addr, "perhaps anonymized IID found (ANON token match)");

					/* verify now checksum */
 					if (ipv6addr_verify_checksum_anonymized_iid(ipv6addrp) == 0) {
						p = UNPACK_XMS(st2, 0, ANON_IID_PREFIX_NIBBLES_MASK, ANON_IID_PREFIX_NIBBLES_SHIFT);

						DEBUGPRINT_WA(DEBUG_libipv6addr, "checksum ok - probably anonymized IID found, p=%d", p);

						if (p == 0) {
							// no additional check
						} else if (p == 0xf) {
							if ((type & IPV6_ADDR_ANONYMIZED_PREFIX) == 0) {
								DEBUGPRINT_NA(DEBUG_libipv6addr, "no anonymized prefix found, but p=f -> no anonymized IID");
								goto END_ANON_IID;
							};
						} else {
							// check anonymized nibbles in prefix
							DEBUGPRINT_WA(DEBUG_libipv6addr, "check now for %d anonymized nibbles in prefix: %04x:%04x:%04x:%04x", p, U32_MSB16(st), U32_LSB16(st), U32_MSB16(st1), U32_LSB16(st1));

							if (p >= 8) {
								mask_0_15 = 0xffffffff >> ((16 - p) * 4);
								mask_16_31 = 0xffffffff;
							} else {
								mask_0_15 = 0x0;
								mask_16_31 = 0xffffffff >> ((8 - p) * 4);
							};

							// check 1st 32-bit block
							if ((st & mask_0_15) != ((ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16)) & mask_0_15)) {
								DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized parts of prefix doesn't match amount of given nibbles: 0-15=%08x mask=%08x", st, mask_0_15);
								goto END_ANON_IID;
							};

							// check 2nd 32-bit block
							if ((st1 & mask_16_31) != ((ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16)) & mask_16_31)) {
								DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized parts of prefix doesn't match amount of given nibbles: 16-31=%08x mask=%08x", st1, mask_16_31);
								goto END_ANON_IID;
							};

							type2 |= IPV6_ADDR_TYPE2_ANON_MASKED_PREFIX;
							ipv6addrp->prefix2length = 64 - 4 * p;
							DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized masked prefix verified, usable prefix length is %u", ipv6addrp->prefix2length);
						};

						if (((st2 & ANON_IID_RANDOM_MASK_00_31) == ANON_IID_RANDOM_VALUE_00_31) && ((st3 & ANON_IID_RANDOM_MASK_32_63) == ANON_IID_RANDOM_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_RANDOM | IPV6_ADDR_ANONYMIZED_IID | IPV6_NEW_ADDR_IID_LOCAL;
							goto END_ipv6addr_gettype;

						} else if (((st2 & ANON_IID_STATIC_MASK_00_31) == ANON_IID_STATIC_VALUE_00_31) && ((st3 & ANON_IID_STATIC_MASK_32_63) == ANON_IID_STATIC_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_LOCAL | IPV6_ADDR_ANONYMIZED_IID;
							goto END_ipv6addr_gettype;

						} else if (((st2 & ANON_IID_EUI48_MASK_00_31) == ANON_IID_EUI48_VALUE_00_31) && ((st3 & ANON_IID_EUI48_MASK_32_63) == ANON_IID_EUI48_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_EUI48 | IPV6_ADDR_ANONYMIZED_IID;

							/* retrieve inverted local/global bit */
							if ( (st3 & ANON_IID_EUIxx_SCOPE_MASK) == ANON_IID_EUIxx_SCOPE_GLOBAL) {
								type |= IPV6_NEW_ADDR_IID_GLOBAL;
							} else {
								type |= IPV6_NEW_ADDR_IID_LOCAL;
							};
							goto END_ipv6addr_gettype;

						} else if (((st2 & ANON_IID_EUI64_MASK_00_31) == ANON_IID_EUI64_VALUE_00_31) && ((st3 & ANON_IID_EUI64_MASK_32_63) == ANON_IID_EUI64_VALUE_32_63)) {
							type |= IPV6_NEW_ADDR_IID_EUI64 | IPV6_ADDR_ANONYMIZED_IID;

							/* retrieve local/global bit */
							if ( (st3 & ANON_IID_EUIxx_SCOPE_MASK) == ANON_IID_EUIxx_SCOPE_GLOBAL) {
								type |= IPV6_NEW_ADDR_IID_GLOBAL;
							} else {
								type |= IPV6_NEW_ADDR_IID_LOCAL;
							};
							goto END_ipv6addr_gettype;

						} else if (((st2 & ANON_IID_IPV4_MASK_00_31) == ANON_IID_IPV4_VALUE_00_31) && ((st3 & ANON_IID_IPV4_MASK_32_63) == ANON_IID_IPV4_VALUE_32_63)) {
							type |= IPV6_ADDR_IID_32_63_HAS_IPV4 | IPV6_ADDR_ANONYMIZED_IID;
							if ((type & IPV6_NEW_ADDR_6TO4) != 0) {
								// anonymized 6to4 microsoft address
								type |= IPV6_NEW_ADDR_6TO4_MICROSOFT | IPV6_NEW_ADDR_IID_LOCAL;
							};
							goto END_ipv6addr_gettype;

						} else if (((st2 & ANON_IID_ISATAP_MASK_00_31) == ANON_IID_ISATAP_VALUE_00_31)) {
							type |= IPV6_NEW_ADDR_IID_ISATAP | IPV6_ADDR_ANONYMIZED_IID;

							if ((st3 & ANON_IID_ISATAP_TYPE_MASK_32_63) == ANON_IID_ISATAP_TYPE_IPV4_VALUE_32_63) {
								type |= IPV6_ADDR_IID_32_63_HAS_IPV4;
							};

							if (((st3 & ANON_IID_ISATAP_SCOPE_MASK) == ANON_IID_ISATAP_SCOPE_GLOBAL)) {
								type |= IPV6_NEW_ADDR_IID_GLOBAL;
							} else {
								type |= IPV6_NEW_ADDR_IID_LOCAL;
							};
							goto END_ipv6addr_gettype;
						};


						if ((ipv6calc_debug & DEBUG_libipv6addr_anonymization_unknown_break) != 0) { // ipv6calc_debug usage ok
							DEBUGPRINT_WA(DEBUG_libipv6addr_anonymization_unknown_break, "unhandled probably anonymized IID found, STOP because of debug level: %08x %08x", st2, st3);
							exit(1);
						} else {
							DEBUGPRINT_NA(DEBUG_libipv6addr, "unhandled probably anonymized IID found (this can really happen), proceed further on");
						};;
					} else {
						DEBUGPRINT_NA(DEBUG_libipv6addr, "checksum WRONG - no anonymized IID found, proceed further on");
					};
				};

END_ANON_IID:
				type |= IPV6_NEW_ADDR_IID_LOCAL;

				if ((type & (IPV6_ADDR_IID_32_63_HAS_IPV4 | IPV6_NEW_ADDR_LINKLOCAL_TEREDO | IPV6_NEW_ADDR_IID_ISATAP | IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_SOLICITED_NODE)) == 0) {
					DEBUGPRINT_WA(DEBUG_libipv6addr, "call IID random detection, typeinfo=%08x", type);

					/* fuzzy detection of random IID (e.g. privacy extension) */
					r = ipv6addr_iidrandomdetection(ipv6addrp, &variances);
					if (r == 0) {
						type |= IPV6_NEW_ADDR_IID_RANDOM;
					} else if (r == 2) {
						type |= IPV6_NEW_ADDR_IID_RANDOM | IPV6_ADDR_ANONYMIZED_IID;
					} else if (r == 3) {
						type |= IPV6_NEW_ADDR_IID_LOCAL | IPV6_ADDR_ANONYMIZED_IID;
					};
				};
			};
		};
	} else {
		type |= IPV6_ADDR_RESERVED;
	};

END_ipv6addr_gettype:
	ipv6addrp->typeinfo  = type;
	ipv6addrp->typeinfo2 = type2;
	ipv6addrp->flag_typeinfo = 1;
};


/*
 * function stores an IPv6 literal address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int addrliteral_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1;
	unsigned int s;
	char tempstring[NI_MAXHOST], tempstring2[NI_MAXHOST], *cptr;
	const char *literalstring = ".ipv6-literal.net";

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Got input '%s'", addrstring);

	/* lowercase string */
	for (s = 0; s <= strlen(addrstring); s++) {
		/* including trailing \0 */
		tempstring2[s] = tolower(addrstring[s]);
	}

	/* search for literal string */
	cptr = strstr(tempstring2, literalstring);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "String lengths addrstring=%d strstr=%d literal=%d", (unsigned int) strlen(addrstring), (unsigned int) strlen(cptr), (unsigned int) strlen(literalstring));

	if (cptr == NULL) {
		snprintf(resultstring, resultstring_length, "Error in given IPv6 literal address, has no 'ipv6-literal.net' included!");
		return (1);
	};

	if (strlen(cptr) != strlen(literalstring)) {
		snprintf(resultstring, resultstring_length, "Error in given IPv6 literal address, ends not with 'ipv6-literal.net'!");
		return (1);
	};

	/* copy without literal */
	snprintf(tempstring, strlen(addrstring) - strlen(literalstring) + 1, "%s", addrstring);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "String without literal suffix: %s", tempstring);

	/* replace - with : */
	for (s = 0; s < strlen(tempstring); s++) {
		if (tempstring[s] == '-') {
			tempstring[s] = ':';
		} else if (tempstring[s] == 's') {
			tempstring[s] = '%';
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "String converted to non-literal format: %s", tempstring);

	/* call normal IPv6 parsing function */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, resultstring_length, ipv6addrp);

	return (retval);
};

/*
 * function stores an IPv6 address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, i, cpoints = 0, ccolons = 0, cxdigits = 0;
	char *addronlystring, *cp, tempstring[NI_MAXHOST], tempstring2[NI_MAXHOST], *cptr, **ptrptr;
	int expecteditems = 0;
	int temp[8];
	unsigned int compat[4];

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Got input '%s' (resultstring_length=%u)", addrstring, (unsigned int) resultstring_length);

	if (strlen(addrstring) < 2) {
		fprintf(stderr, "Error in given IPv6 address, has less than 2 chars!\n");
		return (1);
	};

	if (strlen(addrstring) >= sizeof(tempstring)) {
		fprintf(stderr, "Error in given IPv6 address, has too much chars: %s\n", addrstring);
		return (1);
	};

	ipv6addr_clearall(ipv6addrp);

	snprintf(tempstring, sizeof(tempstring), "%s", addrstring);
	
	/* save prefix length first, if available */
	DEBUGPRINT_WA(DEBUG_libipv6addr, "Call strtok_r, searching for / in %s", tempstring);

	addronlystring = strtok_r(tempstring, "/", ptrptr);
	
	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addrstring);
		return (1);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Got address only string: %s", addronlystring);

	cp = strtok_r (NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 128 ) {
			snprintf(resultstring, resultstring_length, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) i;
		
		DEBUGPRINT_WA(DEBUG_libipv6addr, "prefix length %u", (unsigned int) ipv6addrp->prefixlength);
		DEBUGPRINT_WA(DEBUG_libipv6addr, "flag_prefixuse %d", ipv6addrp->flag_prefixuse);
	};

	snprintf(tempstring2, sizeof(tempstring2), "%s", addronlystring);

	/* save scope ID, if available */
	DEBUGPRINT_WA(DEBUG_libipv6addr, "Call strtok_r, searching for %% in %s", tempstring2);

	addronlystring = strtok_r(tempstring2, "%%", ptrptr);
	
	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addrstring);
		return (1);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Got address only string: %s", addronlystring);

	cp = strtok_r (NULL, "%", ptrptr);
	if ( cp != NULL ) {
		ipv6addrp->flag_scopeid = 1;
		snprintf(ipv6addrp->scopeid, sizeof(ipv6addrp->scopeid), "%s", cp);
		
		DEBUGPRINT_WA(DEBUG_libipv6addr, "scope ID    : %s", ipv6addrp->scopeid);
		DEBUGPRINT_WA(DEBUG_libipv6addr, "flag_scopeid: %d", ipv6addrp->flag_scopeid);
	};

	if ((strlen(addronlystring) < 2) || (strlen(addronlystring) > 45)) {
		/* min: :: */
		/* max: ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128 */
		/* max: ffff:ffff:ffff:ffff:ffff:ffff:123.123.123.123 */
		snprintf(resultstring, resultstring_length, "Error in given IPv6 address, has not 2 to 45 chars!");
		return (1);
	};

	/* uncompress string, if necessary */
	if (strstr(addronlystring, "::") != NULL) {
		result = compaddr_to_uncompaddr(addronlystring, tempstring, sizeof(tempstring));
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Result of uncompressed string: '%s'", tempstring);
		if ( result != 0 ) {
			snprintf(resultstring, resultstring_length, "%s", tempstring);
			retval = 1;
			return (retval);
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Copy string: '%s'", addronlystring);
		snprintf(tempstring, sizeof(tempstring), "%s", addronlystring);
	};
	
	DEBUGPRINT_WA(DEBUG_libipv6addr, "Check string: '%s'", tempstring);

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
			snprintf(resultstring, resultstring_length, "Error in given address expanded to '%s' is not valid!", tempstring);
		} else {
			snprintf(resultstring, resultstring_length, "Error in given address '%s' is not valid!", addrstring);
		};
		retval = 1;
		return (retval);
	};

	/* amount of ":" + "." + xdigits must be length */
	if (ccolons + cpoints + cxdigits != (int) strlen(tempstring)) {
		snprintf(resultstring, resultstring_length, "Error in given address '%s' is not valid!", tempstring);
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
				snprintf(resultstring, resultstring_length, "Error in given compatv4/mapped IPv6 address, '%s' is not valid on position %d!", addrstring, i);
				retval = 1;
				return (retval);
			};
		};
		temp[6] = (int) (( compat[0] << 8 ) | compat[1]);
		temp[7] = (int) (( compat[2] << 8 ) | compat[3]);
	} else {
		/* normal format */
		expecteditems = 8;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	};
	
	DEBUGPRINT_WA(DEBUG_libipv6addr, "reading into array, got items: %d", result);

	if ( result != expecteditems ) {
		snprintf(resultstring, resultstring_length, "Error in given IPv6 address, splitting of '%s' returns %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 7; i++ ) {
		if ( (temp[i] < 0) || (temp[i] > 0xffff) )	{
			snprintf(resultstring, resultstring_length, "Error in given IPv6 address, '%s' is not valid on position %d!", addronlystring, i);
			retval = 1;
			return (retval);
		};
	};
	
	/* copy into structure */
	for ( i = 0; i <= 7; i++ ) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Push word %u: %04x", (unsigned int) i, (unsigned int) temp[i]);
		ipv6addr_setword(ipv6addrp, (unsigned int) i, (unsigned int) temp[i]);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "In structure %08x %08x %08x %08x", (unsigned int) ipv6addr_getdword(ipv6addrp, 0), (unsigned int) ipv6addr_getdword(ipv6addrp, 1), (unsigned int) ipv6addr_getdword(ipv6addrp, 2), (unsigned int) ipv6addr_getdword(ipv6addrp, 3));
	DEBUGPRINT_WA(DEBUG_libipv6addr, "In structure %04x %04x %04x %04x %04x %04x %04x %04x", (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) ipv6addr_getword(ipv6addrp, 1), (unsigned int) ipv6addr_getword(ipv6addrp, 2), (unsigned int) ipv6addr_getword(ipv6addrp, 3), (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
	
	ipv6addr_settype(ipv6addrp);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "flag_prefixuse %d", ipv6addrp->flag_prefixuse);
	
	ipv6addrp->flag_valid = 1;
	retval = 0;
	return (retval);
};


/*
 * stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
static int ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int s;
	char tempstring[NI_MAXHOST], temp2string[NI_MAXHOST];
	
	/* print array */
	if ( ((ipv6addrp->typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED | IPV6_ADDR_IID_32_63_HAS_IPV4)) != 0) && ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) == 0)) {
		if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "%04x:%04x:%04x:%04x:%04x:%04x:%u.%u.%u.%u", \
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
			snprintf(tempstring, sizeof(tempstring), "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", \
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
			snprintf(tempstring, sizeof(tempstring), "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", \
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
			snprintf(tempstring, sizeof(tempstring), "%x:%x:%x:%x:%x:%x:%x:%x", \
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

	if ((ipv6addrp->flag_prefixuse == 1) && ((formatoptions & (FORMATOPTION_literal | FORMATOPTION_no_prefixlength)) == 0))  {
		/* append prefix length */
		snprintf(resultstring, resultstring_length, "%s/%u", tempstring, (unsigned int) ipv6addrp->prefixlength);
	} else {
		if ((formatoptions & FORMATOPTION_literal) != 0) {
			/* replace : by - */
			for (s = 0; s < strlen(tempstring); s++) {
				if (tempstring[s] == ':') {
					tempstring[s] = '-';
				};
			};

			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, resultstring_length, "%ss%s.ipv6-literal.net", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, resultstring_length, "%s.ipv6-literal.net", tempstring);
			}
		} else {
			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, resultstring_length, "%s%%%s", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, resultstring_length, "%s", tempstring);
			};
		};
	};

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(temp2string, sizeof(temp2string), "IPV6=%s", resultstring);
		snprintf(resultstring, resultstring_length, "%s", temp2string);
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
static int ipv6addrstruct_to_uncompaddrprefix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];
	
	DEBUGPRINT_NA(DEBUG_libipv6addr, "called");

	/* test for misuse */
	if ( ((ipv6addrp->typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && (ipv6addrp->prefixlength > 96) ) {
		snprintf(resultstring, resultstring_length, "Error, cannot print prefix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 0 ) {
		snprintf(resultstring, resultstring_length, "Error, cannot print prefix of a address with prefix length 0!");
		retval = 1;
		return (retval);
	};

	max = ( (unsigned int) ipv6addrp->prefixlength - 1 ) / 16u;
	i = 0;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1), "%s", tempstring2);
	};
	snprintf(resultstring, resultstring_length, "%s", tempstring1);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "result string: %s", resultstring);

	retval = 0;
	return (retval);
};


/*
 * function stores the suffix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
static int ipv6addrstruct_to_uncompaddrsuffix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];

	DEBUGPRINT_NA(DEBUG_libipv6addr, "called");

	/* test for misuse */
	if ( ( (ipv6addrp->typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0) && ( ipv6addrp->prefixlength > 96 ) ) {
		snprintf(resultstring, resultstring_length, "Error, cannot print suffix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 128 ) {
		snprintf(resultstring, resultstring_length, "Error, cannot print suffix of a address with prefix length 128!");
		retval = 1;
		return (retval);
	};

	max = 7;
	i = (unsigned int) ipv6addrp->prefixlength / 16u;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( ( ( ipv6addrp->typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && ( i == 6 ) ) {
			snprintf(tempstring2, sizeof(tempstring2), "%s%u.%u.%u.%u", tempstring1, \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
			);
			i = max;
		} else if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x:", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x", tempstring1, (unsigned int) ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1), "%s", tempstring2);
	};
	snprintf(resultstring, resultstring_length, "%s", tempstring1);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "result string: %s", resultstring);
	
	retval = 0;
	return (retval);
};


int libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	
	DEBUGPRINT_WA(DEBUG_libipv6addr, "get format option: %08x", (unsigned int) formatoptions);

	if ( (formatoptions & FORMATOPTION_printprefix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrprefix(ipv6addrp, resultstring, resultstring_length, formatoptions);
	} else if ( (formatoptions & FORMATOPTION_printsuffix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrsuffix(ipv6addrp, resultstring, resultstring_length, formatoptions);		
	} else {
		retval = ipv6addrstruct_to_uncompaddr(ipv6addrp, resultstring, resultstring_length, formatoptions);
	};

	if (retval == 0) {
		/* don't modify case on error messages */
		if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
			/* nothing to do */
		} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
			string_to_upcase(resultstring);
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "result string: %s", resultstring);
	
	retval = 0;	
	return (retval);
};


/*
 * mask prefix bits (set suffix bits to 0)
 * 
 * in:  structure via reference
 * out: modified structure
 */
void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6addr, "called");
   
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
			
			DEBUGPRINT_WA(DEBUG_libipv6addr, "bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x", nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};


/*
 * mask suffix bits (set prefix bits to 0) 
 *
 * in:  structure via reference
 * out: modified structure
 */
void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6addr, "called");
   
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

			DEBUGPRINT_WA(DEBUG_libipv6addr, "%u = nword: %u, mask: %04x, word: %04x newword: %04x", nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};


/*
 * function stores an 16 char token into a structure
 *
 * in : *addrstring = 16 char token
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result;
	int temp[4];
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv6addr, "got input '%s'", addrstring);
	
	if ( strlen(addrstring) != 16 ) {
		snprintf(resultstring, resultstring_length, "Error in given token '%s' is not valid (length != 16)!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	result = sscanf(addrstring, "%04x%04x%04x%04x", &temp[0], &temp[1], &temp[2], &temp[3]);
	if ( result != 4 ) {
		snprintf(resultstring, resultstring_length, "Error splitting address '%s', got %d items instead of 4!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring),  "0:0:0:0:%04x:%04x:%04x:%04x", \
		(unsigned int) temp[0] ,\
		(unsigned int) temp[1], \
		(unsigned int) temp[2], \
		(unsigned int) temp[3]  \
	);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, sizeof(resultstring), ipv6addrp);

	return (retval);
};

/*
 * function stores an interface identifier into a structure
 *
 * in : *addrstring = interface identifier
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, i, ccolons = 0;
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Got input '%s'", addrstring);
	
	if ((strlen(addrstring) < 2) || (strlen(addrstring) > 19)) {
		/* min: :: */
		/* max: ffff:ffff:ffff:ffff */
		snprintf(resultstring, resultstring_length, "Error in given identifier identifier, has not 2 to 19 chars!");
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
		snprintf(resultstring, resultstring_length, "Error in given identifier '%s' is not valid!", addrstring);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring), "0:0:0:0:%s", addrstring);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, sizeof(resultstring), ipv6addrp);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "result string '%s'", resultstring);

	return (retval);
};

/*
 * function stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, uint32_t formatoptions) {
	int retval = 1;
	
	/* print array */
	snprintf(resultstring, resultstring_length, "%04x%04x%04x%04x", \
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

	DEBUGPRINT_WA(DEBUG_libipv6addr, "result string '%s'", resultstring);
	
	retval = 0;	
	return (retval);
};


/*
 * function prints an IPv6 address in native octal format
 *
 * in:  ipv6addr = IPv6 address structure
 * formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv6addr_to_octal(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
		snprintf(tempstring, sizeof(tempstring), "\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o\\0%03o",
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
		snprintf(tempstring, sizeof(tempstring), "\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o\\0%o",
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

	snprintf(resultstring, resultstring_length, "%s", tempstring);
	retval = 0;	
	return (retval);
};


/*
 * function prints an IPv6 address in native hex format
 *
 * in:  ipv6addr = IPv6 address structure
 * formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv6addr_to_hex(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	int i;
	unsigned int s;
	char tempstring[NI_MAXHOST];

	snprintf(tempstring, sizeof(tempstring), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
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

	snprintf(resultstring, resultstring_length, "%s", tempstring);

	if ((formatoptions & FORMATOPTION_printprefix) && (ipv6addrp->flag_prefixuse == 1)) {
		// shorten string
		resultstring[ipv6addrp->prefixlength / 4] = '\0';
	} else if ((formatoptions & FORMATOPTION_printsuffix) && (ipv6addrp->flag_prefixuse == 1)) {
		// move string
		for (i = 0; i < 32 - (ipv6addrp->prefixlength / 4); i++) {
			resultstring[i] = resultstring[i + (ipv6addrp->prefixlength / 4)];
		};
		resultstring[32 - (ipv6addrp->prefixlength / 4)] = '\0';
	};

	if (formatoptions & FORMATOPTION_printuppercase) {
		for (s = 0; s < strlen(resultstring); s++) {
			resultstring[s] = toupper(resultstring[s]);
		};
	};

	retval = 0;	
	return (retval);
};


/*
 * retrieve payload of anonymized prefix
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 *      payload_selector: payload which should be retrieved
 *      result_ptr = pointer to a 32-bit result
 * out: 0 = OK, !=0: not ok
 */
int ipv6addr_get_payload_anonymized_prefix(const ipv6calc_ipv6addr *ipv6addrp, const int payload_selector, uint32_t *result_ptr) {
	uint32_t prefix[2], flags;

	prefix[0] = ipv6addr_getdword(ipv6addrp, 0);
	prefix[1] = ipv6addr_getdword(ipv6addrp, 1);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Get payload %d from %08x%08x", payload_selector, prefix[0], prefix[1]);

	// retrieve flags
	flags = UNPACK_XMS(prefix[ANON_PREFIX_FLAGS_DWORD], ANON_PREFIX_FLAGS_XOR, ANON_PREFIX_FLAGS_MASK, ANON_PREFIX_FLAGS_SHIFT);

	if (flags != 0) {
		// currently only flags=0 is supported
		return(1);
	};

	if (payload_selector == ANON_PREFIX_PAYLOAD_CCINDEX) {
		*result_ptr = UNPACK_XMS(prefix[ANON_PREFIX_CCINDEX_DWORD], ANON_PREFIX_CCINDEX_XOR, ANON_PREFIX_CCINDEX_MASK, ANON_PREFIX_CCINDEX_SHIFT);
	};

	if (payload_selector == ANON_PREFIX_PAYLOAD_ASN32) {
		*result_ptr = (UNPACK_XMS(prefix[ANON_PREFIX_ASN32_MSB_DWORD], ANON_PREFIX_ASN32_MSB_XOR, ANON_PREFIX_ASN32_MSB_MASK, ANON_PREFIX_ASN32_MSB_SHIFT) << ANON_PREFIX_ASN32_LSB_AMOUNT)| (UNPACK_XMS(prefix[ANON_PREFIX_ASN32_LSB_DWORD], ANON_PREFIX_ASN32_LSB_XOR, ANON_PREFIX_ASN32_LSB_MASK, ANON_PREFIX_ASN32_LSB_SHIFT));
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Extracted payload %d from %08x%08x: %08x", payload_selector, prefix[0], prefix[1], *result_ptr);

	return(0);
};


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

	if ((typeinfo & IPV6_NEW_ADDR_IID_EUI48) != 0) {
		payload = (iid[1] >> ANON_IID_EUI48_PAYLOAD_SHIFT) & ((2 << ANON_IID_EUI48_PAYLOAD_LENGTH) - 1);
	} else if ((typeinfo & IPV6_NEW_ADDR_IID_EUI64) != 0) {
		payload = (iid[1] >> ANON_IID_EUI64_PAYLOAD_SHIFT) & ((2 << ANON_IID_EUI64_PAYLOAD_LENGTH) - 1);
	} else if ((typeinfo & (IPV6_NEW_ADDR_IID_ISATAP|IPV6_ADDR_IID_32_63_HAS_IPV4)) != 0) {
		payload = (iid[1] >> ANON_IID_ISATAP_PAYLOAD_SHIFT) & ((2 << ANON_IID_ISATAP_PAYLOAD_LENGTH) - 1);
	} else if ((typeinfo & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0) {
		payload = (iid[1] >> ANON_IID_IPV4_PAYLOAD_SHIFT) & ((2 << ANON_IID_IPV4_PAYLOAD_LENGTH) - 1);
	};

	return(payload);
};


/*
 * anonymize IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 *      *ipv6calc_anon_set = anonymization set structure
 * ret: 0:anonymization ok
 *      1:anonymization method not supported
 */
int libipv6addr_anonymize(ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_anon_set *ipv6calc_anon_set) {
	/* anonymize IPv4 address according to settings */
	uint32_t iid[2];
	char tempstring[NI_MAXHOST];
	char helpstring[NI_MAXHOST];
	int i, j, r;
	int calculate_checksum = 0;
	int calculate_checksum_prefix = 0;
	int zeroize_prefix = 0;
	int zeroize_iid = 0;
	int anonymized_prefix_nibbles = 0;

	ipv6calc_macaddr   macaddr;
	ipv6calc_eui64addr eui64addr;
	ipv6calc_ipv4addr  ipv4addr;
	ipv6calc_ipaddr    ipaddr;
	uint32_t map_value;

	uint16_t cc_index, flags;
	uint32_t as_num32, ipv6_prefix[2];

	int mask_eui64  = ipv6calc_anon_set->mask_eui64;
	// int mask_mac  = ipv6calc_anon_set->mask_mac; // currently not used
	int mask_ipv6 = ipv6calc_anon_set->mask_ipv6;
	int mask_ipv4 = ipv6calc_anon_set->mask_ipv4;
	int method    = ipv6calc_anon_set->method;

	uint8_t bit_ul = 0;

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Called: addr=%08x %08x %08x %08x", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1), ipv6addr_getdword(ipv6addrp, 2), ipv6addr_getdword(ipv6addrp, 3));

	ipv6addr_settype(ipv6addrp);

	if (method == ANON_METHOD_ZEROIZE) {
		zeroize_prefix = 1;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {	// ipv6calc_debug usage ok
		libipv6calc_anon_infostring(tempstring, sizeof(tempstring), ipv6calc_anon_set);
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Anonymize IPv6 address flags: %s", tempstring);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {	// ipv6calc_debug usage ok
		DEBUGPRINT_WA(DEBUG_libipv6addr, "IPv6 address flags: 0x%08x", ipv6addrp->typeinfo);

		j = 0;
		snprintf(tempstring, sizeof(tempstring), "TYPE=");
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
			if ((ipv6addrp->typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring), "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring), "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				j = 1;
			};
		};
		fprintf(stderr, "%s\n", tempstring);
	};

	if ((ipv6addrp->typeinfo & (IPV6_ADDR_ANONYMIZED_IID | IPV6_ADDR_ANONYMIZED_PREFIX)) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv6addr, "Already anonymized IPv6 address - skip");
		return(0);
	};

	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4) != 0) {
		/* extract IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 2 + i));
		};

		ipv4addr_settype(&ipv4addr, 1); /* Set typeinfo */
		ipv4addr.flag_valid = 1;
		libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 2 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
		};

	};

	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_TEREDO) != 0) {
		/* extract Teredo client IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 12 + i) ^ 0xff);
		};

		ipv4addr_settype(&ipv4addr, 1);
		ipv4addr.flag_valid = 1;
		libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i) ^ 0xff);
		};

		/* clear client port */
		ipv6addr_setword(ipv6addrp, 5, 0 ^ 0xffff);
	};

	if ((ipv6addrp->typeinfo & (IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4 | IPV6_NEW_ADDR_NAT64)) != 0) {
		/* extract IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) 12 + i));
		};

		ipv4addr_settype(&ipv4addr, 1);
		ipv4addr.flag_valid = 1;
		libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

		/* store back */
		for (i = 0; i <= 3; i++) {
			ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
		};
	};

	/* prefix handling */
	if ( ((ipv6addrp->typeinfo & (IPV6_ADDR_SITELOCAL | IPV6_ADDR_ULUA | IPV6_NEW_ADDR_AGU)) != 0) && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		/* prefix included */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Prefix: pref=%08x %08x", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1));

		if (((ipv6addrp->typeinfo & IPV6_NEW_ADDR_AGU) != 0) && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_6TO4)) == 0) && (method == ANON_METHOD_KEEPTYPEASNCC)) {
			if (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 0) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "anonymization method not supported, db_wrapper reports too less features");
				return(1);
			};

			// check whether IPv6 address is anycast
			if (((ipv6addrp->typeinfo & IPV6_ADDR_ANYCAST) != 0) && ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_LISP) != 0)) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "IPv6 is LISP anycast, skip prefix anonymization");
				goto InterfaceIdentifier;
			};

			// check whether IPv6 address is reserved
			r = libipv6calc_db_wrapper_registry_string_by_ipv6addr(ipv6addrp, helpstring, sizeof(helpstring));
			if (r == 2) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "IPv6 registry of prefix contains reserved, skip anonymization");
				goto InterfaceIdentifier;
			};

			// switch to prefix anonymization
			if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6BONE) != 0) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "IPv6 is 6bone unicast, special prefix anonymization");
				cc_index = COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN + IPV6_ADDR_REGISTRY_6BONE;
				as_num32 = 0;
			} else if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_LISP) != 0) {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "IPv6 is LISP unicast, special prefix anonymization");
				cc_index = 0x3fd;
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);
				as_num32 = libipv6calc_db_wrapper_cc_index_by_addr(&ipaddr, NULL) << 20;
			} else {
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);

				cc_index = libipv6calc_db_wrapper_cc_index_by_addr(&ipaddr, NULL);
				as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(&ipaddr, NULL);

				if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
					// on unknown country, map registry value
					cc_index = COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN + libipv6calc_db_wrapper_registry_num_by_ipv6addr(ipv6addrp);
				};
			};

			DEBUGPRINT_WA(DEBUG_libipv6addr, "cc_index=%d (0x%03x) as_num32=%d (0x%08x)", cc_index, cc_index, as_num32, as_num32);

			flags = 0x0;

			ipv6_prefix[0] = 0; ipv6_prefix[1] = 0;

			// store prefix
			ipv6_prefix[ANON_PREFIX_TOKEN_DWORD] |= PACK_XMS(ANON_PREFIX_TOKEN_VALUE, ANON_PREFIX_TOKEN_XOR, ANON_PREFIX_TOKEN_MASK, ANON_PREFIX_TOKEN_SHIFT);

			// store cc_index
			ipv6_prefix[ANON_PREFIX_CCINDEX_DWORD] |= PACK_XMS(cc_index, ANON_PREFIX_CCINDEX_XOR, ANON_PREFIX_CCINDEX_MASK, ANON_PREFIX_CCINDEX_SHIFT);

			// store as_num32
			ipv6_prefix[ANON_PREFIX_ASN32_MSB_DWORD] |= PACK_XMS(as_num32 >> ANON_PREFIX_ASN32_LSB_AMOUNT, ANON_PREFIX_ASN32_MSB_XOR, ANON_PREFIX_ASN32_MSB_MASK, ANON_PREFIX_ASN32_MSB_SHIFT);
			ipv6_prefix[ANON_PREFIX_ASN32_LSB_DWORD] |= PACK_XMS(as_num32 & ANON_PREFIX_ASN32_LSB_MASK, ANON_PREFIX_ASN32_LSB_XOR, ANON_PREFIX_ASN32_LSB_MASK, ANON_PREFIX_ASN32_LSB_SHIFT);

			// store flags
			ipv6_prefix[ANON_PREFIX_FLAGS_DWORD] |= PACK_XMS(flags, ANON_PREFIX_FLAGS_XOR, ANON_PREFIX_FLAGS_MASK, ANON_PREFIX_FLAGS_SHIFT);

			DEBUGPRINT_WA(DEBUG_libipv6addr, "anonmized prefix for method=%d: %08x%08x", method, ipv6_prefix[0], ipv6_prefix[1]);

			anonymized_prefix_nibbles = 0;

			ipv6addr_setdword(ipv6addrp, 0, ipv6_prefix[0]);
			ipv6addr_setdword(ipv6addrp, 1, ipv6_prefix[1]);
			calculate_checksum_prefix = 1;

		} else if (mask_ipv6 == 64) {
			/* nothing to do */
		} else if (mask_ipv6 < 16 || mask_ipv6 > 64) {
			/* should not happen here */
			fprintf(stderr, "%s/%s: 'mask_ipv6' has an unexpected illegal value: %d\n", __FILE__, __func__, mask_ipv6);
			exit(EXIT_FAILURE);
		} else {
			if (((ipv6addrp->typeinfo & IPV6_ADDR_ANYCAST) != 0) && ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_LISP) != 0)) {
				if (mask_ipv6 < 48) {
					mask_ipv6 = 48; // keeping address type
				};
				DEBUGPRINT_WA(DEBUG_libipv6addr, "Mask adjusted to: %d", mask_ipv6);
			};

			DEBUGPRINT_WA(DEBUG_libipv6addr, "Mask prefix with mask: %d", mask_ipv6);

			if (mask_ipv6 < 64 && mask_ipv6 > 32) {
				if (zeroize_prefix != 0) {
					ipv6addr_setdword(ipv6addrp, 1, ipv6addr_getdword(ipv6addrp, 1) & (0xffffffffu << ((unsigned int) 64 - mask_ipv6)));
				} else {
					ipv6addr_setdword(ipv6addrp, 1, (ipv6addr_getdword(ipv6addrp, 1) & (0xffffffffu << ((unsigned int) 64 - mask_ipv6))) | ((ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16)) & ((0xffffffffu >> ((unsigned int) mask_ipv6 - 32)))));
					anonymized_prefix_nibbles = (64 - mask_ipv6) / 4;
				};
			} else if (mask_ipv6 == 32) {
				if (zeroize_prefix != 0) {
					ipv6addr_setdword(ipv6addrp, 1, 0u);
				} else {
					ipv6addr_setdword(ipv6addrp, 1, ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16));
					anonymized_prefix_nibbles = 8;
				};
			} else if (mask_ipv6 < 32 && mask_ipv6 >= 16) {
				if (zeroize_prefix != 0) {
					ipv6addr_setdword(ipv6addrp, 1, 0u);
					ipv6addr_setdword(ipv6addrp, 0, ipv6addr_getdword(ipv6addrp, 0) & (0xffffffffu << ((unsigned int) 32 - mask_ipv6)));
				} else {
					ipv6addr_setdword(ipv6addrp, 1, ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16));
					ipv6addr_setdword(ipv6addrp, 0, (ipv6addr_getdword(ipv6addrp, 0) & (0xffffffffu << ((unsigned int) 32 - mask_ipv6))) | ((ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16)) & ((0xffffffffu >> ((unsigned int) mask_ipv6)))));
					anonymized_prefix_nibbles = (64 - mask_ipv6) / 4;
				};
			};
		};

		/* restore prefix in special cases */
		if ( ((ipv6addrp->typeinfo & IPV6_ADDR_SITELOCAL) != 0) && (mask_ipv6 < 10) ) { 
			ipv6addr_setword(ipv6addrp, 0, ipv6addr_getword(ipv6addrp, 1) | 0xfec0u);
		} else if ( ((ipv6addrp->typeinfo & IPV6_ADDR_ULUA) != 0) && (mask_ipv6 < 7) ) {
			ipv6addr_setoctet(ipv6addrp, 0, ipv6addr_getoctet(ipv6addrp, 0) | 0xfdu);
		};

		DEBUGPRINT_WA(DEBUG_libipv6addr, "Prefix: pref=%08x %08x (anonymized)", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1));
	};

InterfaceIdentifier:
	/* interface identifier handling */
	if ( ( ((ipv6addrp->typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) || ((ipv6addrp->typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		/* Interface identifier included */
		if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_EUI48) != 0) {
			/* EUI-48 */

			DEBUGPRINT_NA(DEBUG_libipv6addr, "EUI-48 identifier found");

			if (method == ANON_METHOD_ZEROIZE) {
				zeroize_iid = 1;
			} else {
				/* set anon interface ID, include shifted OUI bytes */
				DEBUGPRINT_WA(DEBUG_libipv6addr, "Anonymize IPv6 address: OUI=%02x:%02x:%02x", ipv6addr_getoctet(ipv6addrp, 8) & 0xfc, ipv6addr_getoctet(ipv6addrp, 9), ipv6addr_getoctet(ipv6addrp, 10));

				mac_clearall(&macaddr);
				macaddr.addr[0] = ipv6addr_getoctet(ipv6addrp,  8) ^ 0x2;
				macaddr.addr[1] = ipv6addr_getoctet(ipv6addrp,  9);
				macaddr.addr[2] = ipv6addr_getoctet(ipv6addrp, 10);
				macaddr.addr[3] = ipv6addr_getoctet(ipv6addrp, 13);
				macaddr.addr[4] = ipv6addr_getoctet(ipv6addrp, 14);
				macaddr.addr[5] = ipv6addr_getoctet(ipv6addrp, 15);
				macaddr.flag_valid = 1;

				map_value = libieee_map_oui_macaddr(&macaddr) ^ 0x00020000;

				iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_EUI48_VALUE_00_31;
				iid[1] = ANON_IID_EUI48_VALUE_32_63 | ((map_value & 0x1ffffff) << ANON_IID_EUI48_PAYLOAD_SHIFT);

				ipv6addr_setdword(ipv6addrp, 2, iid[0]);
				ipv6addr_setdword(ipv6addrp, 3, iid[1]);
				calculate_checksum = 1;
			};
		} else {
			if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0 ) {
				if (method == ANON_METHOD_ZEROIZE) {
					/* mask ID according to mask_eui64 */
					zeroize_iid = 1;
				} else {
					/* replace IID with special value */
					iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_RANDOM_VALUE_00_31;
					iid[1] = ANON_IID_RANDOM_VALUE_32_63;

					ipv6addr_setdword(ipv6addrp, 2, iid[0]);
					ipv6addr_setdword(ipv6addrp, 3, iid[1]);
					calculate_checksum = 1;
				};
			} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_EUI64) == IPV6_NEW_ADDR_IID_EUI64) {
				/* Check for global EUI-64 */
				if (method == ANON_METHOD_ZEROIZE) {
					/* mask ID according to mask_eui64 */
					zeroize_iid = 1;
				} else {
					libeui64_clearall(&eui64addr);
					eui64addr.addr[0] = ipv6addr_getoctet(ipv6addrp,  8) ^ 0x2;
					eui64addr.addr[1] = ipv6addr_getoctet(ipv6addrp,  9);
					eui64addr.addr[2] = ipv6addr_getoctet(ipv6addrp, 10);
					eui64addr.addr[3] = ipv6addr_getoctet(ipv6addrp, 11);
					eui64addr.addr[4] = ipv6addr_getoctet(ipv6addrp, 12);
					eui64addr.addr[5] = ipv6addr_getoctet(ipv6addrp, 13);
					eui64addr.addr[6] = ipv6addr_getoctet(ipv6addrp, 14);
					eui64addr.addr[7] = ipv6addr_getoctet(ipv6addrp, 15);
					eui64addr.flag_valid = 1;

					map_value = libieee_map_oui_eui64addr(&eui64addr) ^ 0x00020000;

					iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_EUI64_VALUE_00_31;
					iid[1] = ANON_IID_EUI64_VALUE_32_63 | ((map_value & 0x1ffffff) << ANON_IID_EUI64_PAYLOAD_SHIFT);

					ipv6addr_setdword(ipv6addrp, 2, iid[0]);
					ipv6addr_setdword(ipv6addrp, 3, iid[1]);
					calculate_checksum = 1;
				};
			} else {
				if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
					/* zero'ise unique ID */
					ipv6addr_setoctet(ipv6addrp, 13, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 14, 0x0u);
					ipv6addr_setoctet(ipv6addrp, 15, 0x0u);
				} else if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_ISATAP) != 0 )  {
					/* ISATAP address */
					if ( (ipv6addrp->typeinfo & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0 ) {
						/* IPv4 address included */

						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
						};
						libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

						DEBUGPRINT_WA(DEBUG_libipv6addr, "ISATAP includes IPv4 address: IPv4=%d.%d.%d.%d, anonymized: %d.%d.%d.%d", ipv6addr_getoctet(ipv6addrp, 12), ipv6addr_getoctet(ipv6addrp, 13), ipv6addr_getoctet(ipv6addrp, 14), ipv6addr_getoctet(ipv6addrp, 15), ipv4addr_getoctet(&ipv4addr, 0), ipv4addr_getoctet(&ipv4addr, 1), ipv4addr_getoctet(&ipv4addr, 2), ipv4addr_getoctet(&ipv4addr, 3));

						/* store back */
						for (i = 0; i <= 3; i++) {
							ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
						};

						if (method == ANON_METHOD_ZEROIZE) {
							// nothing to do, IPv4 address already anonymized
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_ISATAP_VALUE_00_31;
							iid[1] = (ipv6addr_getoctet(ipv6addrp, 12) << (ANON_IID_ISATAP_PAYLOAD_SHIFT + 16)) | (ipv6addr_getoctet(ipv6addrp, 13) << (ANON_IID_ISATAP_PAYLOAD_SHIFT + 8)) | (ipv6addr_getoctet(ipv6addrp, 14) << (ANON_IID_ISATAP_PAYLOAD_SHIFT));
							iid[1] |= ANON_IID_ISATAP_TYPE_IPV4_VALUE_32_63;
							calculate_checksum = 1;
						};
					} else if ((ipv6addr_getoctet(ipv6addrp, 11) == 0xff) && (ipv6addr_getoctet(ipv6addrp, 12) == 0xfe)) {
						/* vendor ID included */
						if (method == ANON_METHOD_ZEROIZE) {
							/* zero'ise unique ID */
							/* TODO: honor mask_eui64 */
							ipv6addr_setoctet(ipv6addrp, 13, 0x0u);
							ipv6addr_setoctet(ipv6addrp, 14, 0x0u);
							ipv6addr_setoctet(ipv6addrp, 15, 0x0u);
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_ISATAP_VALUE_00_31;
							iid[1] = (ipv6addr_getoctet(ipv6addrp, 13) << (ANON_IID_ISATAP_PAYLOAD_SHIFT + 16));
							iid[1] |= ANON_IID_ISATAP_TYPE_VENDOR_VALUE_32_63;
							calculate_checksum = 1;
						};
					} else {
						/* extension ID included */
						if (method == ANON_METHOD_ZEROIZE) {
							/* mask ID according to mask_eui64 */
							zeroize_iid = 1;
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_ISATAP_VALUE_00_31;
							iid[1] = (ipv6addr_getoctet(ipv6addrp, 11) << (ANON_IID_ISATAP_PAYLOAD_SHIFT + 16)) | (ipv6addr_getoctet(ipv6addrp, 12) << (ANON_IID_ISATAP_PAYLOAD_SHIFT + 8)) | (ipv6addr_getoctet(ipv6addrp, 13) << (ANON_IID_ISATAP_PAYLOAD_SHIFT));
							iid[1] |= ANON_IID_ISATAP_TYPE_EXTID_VALUE_32_63;
							calculate_checksum = 1;
						};
					};

					if (calculate_checksum == 1) {
						/* store local/global bit */
						DEBUGPRINT_NA(DEBUG_libipv6addr, "ISATAP: store local/global bit");
						if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_GLOBAL) != 0 )  {
							DEBUGPRINT_NA(DEBUG_libipv6addr, "ISATAP: store global bit");
							iid[1] |= ANON_IID_ISATAP_SCOPE_GLOBAL;
						} else if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_LOCAL) != 0 ) {
							DEBUGPRINT_NA(DEBUG_libipv6addr, "ISATAP: store local bit");
							iid[1] |= ANON_IID_ISATAP_SCOPE_LOCAL;
						};
						ipv6addr_setdword(ipv6addrp, 2, iid[0]);
						ipv6addr_setdword(ipv6addrp, 3, iid[1]);
					};

				} else if ( ( ( (ipv6addrp->typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0 && ipv6addr_getword(ipv6addrp, 6) != 0)) )   {
					/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
					};

					libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

					/* store back */
					for (i = 0; i <= 3; i++) {
						ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
					};
				} else {
					if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
						/* extract IPv4 address */
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
						};

						libipv4addr_anonymize(&ipv4addr, mask_ipv4, method);

						if (method == ANON_METHOD_ZEROIZE) {
							/* store back */
							for (i = 0; i <= 3; i++) {
								ipv6addr_setoctet(ipv6addrp, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctet(&ipv4addr, (unsigned int) i));
							};
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_IPV4_VALUE_00_31;
							iid[1] = ANON_IID_IPV4_VALUE_32_63 | (ipv4addr_getoctet(&ipv4addr, 0) << (ANON_IID_IPV4_PAYLOAD_SHIFT + 16)) | (ipv4addr_getoctet(&ipv4addr, 1) << (ANON_IID_IPV4_PAYLOAD_SHIFT + 8)) | (ipv4addr_getoctet(&ipv4addr, 2) << (ANON_IID_IPV4_PAYLOAD_SHIFT));

							ipv6addr_setdword(ipv6addrp, 2, iid[0]);
							ipv6addr_setdword(ipv6addrp, 3, iid[1]);
							calculate_checksum = 1;
						};
					} else {
						/* Identifier has local scope */
						if (method == ANON_METHOD_ZEROIZE) {
							/* mask ID according to mask_eui64 */
							zeroize_iid = 1;
						} else {
							iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_IID_STATIC_VALUE_00_31;
							iid[1] = ANON_IID_STATIC_VALUE_32_63;

							ipv6addr_setdword(ipv6addrp, 2, iid[0]);
							ipv6addr_setdword(ipv6addrp, 3, iid[1]);
							calculate_checksum = 1;
						};
					};
				};
			};
		};
	};

	/* ORCHID hash */
	if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_ORCHID) != 0 ) {
		DEBUGPRINT_NA(DEBUG_libipv6addr, "ORCHID address found");

		if (method == ANON_METHOD_ZEROIZE) {
			/* mask 100 LSBs */
			ipv6addr_setword(ipv6addrp, 7, 0x0u);
			ipv6addr_setword(ipv6addrp, 6, 0x0u);
			ipv6addr_setword(ipv6addrp, 5, 0x0u);
			ipv6addr_setword(ipv6addrp, 4, 0x0u);
		} else {
			iid[0] = ANON_TOKEN_VALUE_00_31 | ANON_ORCHID_VALUE_00_31;
			iid[1] = ANON_ORCHID_VALUE_32_63;
			ipv6addr_setdword(ipv6addrp, 2, iid[0]);
			ipv6addr_setdword(ipv6addrp, 3, iid[1]);
			calculate_checksum = 1;
		};

		if (zeroize_prefix != 0) {
			ipv6addr_setword(ipv6addrp, 3, 0x0u);
			ipv6addr_setword(ipv6addrp, 2, 0x0u);
			ipv6addr_setword(ipv6addrp, 1, ipv6addr_getword(ipv6addrp, 1) & 0xFFF0);
		} else {
			ipv6addr_setdword(ipv6addrp, 1, (ANON_TOKEN_VALUE_00_31 | (ANON_TOKEN_VALUE_00_31 >> 16)));
			ipv6addr_setword(ipv6addrp, 1, (ipv6addr_getword(ipv6addrp, 1) & 0xFFF0) | ((ANON_TOKEN_VALUE_00_31 >> 16) & 0xF));
			anonymized_prefix_nibbles = (64 - 28) / 4;
		};
	};

	/* switch prefix anonymization if IID is not anonymizied in reliable way */
	if (calculate_checksum == 0) {
		zeroize_prefix = 1;
	} else {
		ipv6addrp->typeinfo |= IPV6_ADDR_ANONYMIZED_IID;
	};

	if (zeroize_iid == 1) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "Zeroize IID with mask: %d", mask_eui64);

		libeui64_clearall(&eui64addr);

		eui64addr.addr[0] = ipv6addr_getoctet(ipv6addrp,  8) ^ 0x2;
		eui64addr.addr[1] = ipv6addr_getoctet(ipv6addrp,  9);
		eui64addr.addr[2] = ipv6addr_getoctet(ipv6addrp, 10);
		eui64addr.addr[3] = ipv6addr_getoctet(ipv6addrp, 11);
		eui64addr.addr[4] = ipv6addr_getoctet(ipv6addrp, 12);
		eui64addr.addr[5] = ipv6addr_getoctet(ipv6addrp, 13);
		eui64addr.addr[6] = ipv6addr_getoctet(ipv6addrp, 14);
		eui64addr.addr[7] = ipv6addr_getoctet(ipv6addrp, 15);
		eui64addr.flag_valid = 1;

		bit_ul = eui64addr.addr[0] & 0x02;

		libeui64_anonymize(&eui64addr, ipv6calc_anon_set); // covers also EUI-48

		if ((mask_eui64 < 7) && (ipv6calc_anon_set->mask_autoadjust == 0) && (bit_ul == 0x0)) {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "mask_eui64 < 7 AND EUI-64 was %s AND mask_autoadjust not set, do not invert universal/local bit for IID (for backwards compatibility reason", (bit_ul == 2) ? "local" : "universal");

			eui64addr.addr[0] |= 0x02; // will be inverted and therefore cleared next
		};

		ipv6addr_setoctet(ipv6addrp,  8, eui64addr.addr[0] ^ 0x2);
		ipv6addr_setoctet(ipv6addrp,  9, eui64addr.addr[1]);
		ipv6addr_setoctet(ipv6addrp, 10, eui64addr.addr[2]);
		ipv6addr_setoctet(ipv6addrp, 11, eui64addr.addr[3]);
		ipv6addr_setoctet(ipv6addrp, 12, eui64addr.addr[4]);
		ipv6addr_setoctet(ipv6addrp, 13, eui64addr.addr[5]);
		ipv6addr_setoctet(ipv6addrp, 14, eui64addr.addr[6]);
		ipv6addr_setoctet(ipv6addrp, 15, eui64addr.addr[7]);
	};

	// checksumming
	if (calculate_checksum == 1) {
		if (anonymized_prefix_nibbles > 0) {
			/* fill amount of nibbles into IID lead token */
			DEBUGPRINT_WA(DEBUG_libipv6addr, "Store amount of anonymized nibbles to IID lead token: %d", anonymized_prefix_nibbles);

			ipv6addr_setoctet(ipv6addrp, 9, ipv6addr_getoctet(ipv6addrp, 9) | (anonymized_prefix_nibbles << 4));
		};

		ipv6addr_set_checksum_anonymized_iid(ipv6addrp);
	};

	if (calculate_checksum_prefix == 1) {
		ipv6addr_set_checksum_anonymized_prefix(ipv6addrp);
		ipv6addrp->typeinfo |= IPV6_ADDR_ANONYMIZED_PREFIX;
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Result: addr=%08x %08x %08x %08x (anonymized)", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1), ipv6addr_getdword(ipv6addrp, 2), ipv6addr_getdword(ipv6addrp, 3));

	return(0);
};


/*
 * clear filter IPv6 address
 *
 * in : *filter    = filter structure
 */
void ipv6addr_filter_clear(s_ipv6calc_filter_ipv6addr *filter) {
	filter->active = 0;

	filter->filter_typeinfo.active = 0;
	filter->filter_typeinfo.typeinfo_must_have = 0;
	filter->filter_typeinfo.typeinfo_may_not_have = 0;
	filter->filter_typeinfo2.typeinfo_must_have = 0;
	filter->filter_typeinfo2.typeinfo_may_not_have = 0;

	libipv6calc_filter_clear_db_cc(&filter->filter_db_cc);
	libipv6calc_filter_clear_db_asn(&filter->filter_db_asn);
	libipv6calc_filter_clear_db_registry(&filter->filter_db_registry);

	filter->filter_addr.active = 0;
	filter->filter_addr.addr_must_have_max = 0;
	filter->filter_addr.addr_may_not_have_max = 0;

	return;
};


/*
 * parse filter IPv6
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int ipv6addr_filter_parse(s_ipv6calc_filter_ipv6addr *filter, const char *token) {
	int i, result = 1, negate = 0, offset = 0, r;
	const char *prefix = "ipv6";
	const char *prefixdot = "ipv6.";
	const char *prefixdbdot = "db.";
	const char *prefixaddreq = "addr=";
	const char *prefixaddreq_le = "addr<=";
	const char *prefixaddreq_lt = "addr<";
	const char *prefixaddreq_ge = "addr>=";
	const char *prefixaddreq_gt = "addr>";
	const char *prefixaddreq_le2 = "addr=le=";
	const char *prefixaddreq_lt2 = "addr=lt=";
	const char *prefixaddreq_ge2 = "addr=ge=";
	const char *prefixaddreq_gt2 = "addr=gt=";
	ipv6calc_ipv6addr ipv6addr;
	char resultstring[NI_MAXHOST];
	int db = 0, addr = 0;
	int addr_test_method;

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	// typeinfo
	if (strcmp(token + offset, prefix) == 0) {
		/* any */
		if (negate == 1) {
			filter->filter_typeinfo.typeinfo_may_not_have = ~IPV6_ADDR_ANY;
		} else {
			filter->filter_typeinfo.typeinfo_must_have = IPV6_ADDR_ANY;
		};
		filter->filter_typeinfo.active = 1;
		filter->active = 1;
		result = 0;
		goto END_ipv6addr_filter_parse;

	} else if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		DEBUGPRINT_WA(DEBUG_libipv6addr, "token with prefix, suffix: %s", token + offset);
	};

	if (strncmp(token + offset, prefixaddreq_le, strlen(prefixaddreq_le)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr<=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_le);
		addr_test_method = IPV6CALC_TEST_LE;

	} else if (strncmp(token + offset, prefixaddreq_lt, strlen(prefixaddreq_lt)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr<' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_lt);
		addr_test_method = IPV6CALC_TEST_LT;

	} else if (strncmp(token + offset, prefixaddreq_ge, strlen(prefixaddreq_ge)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr>=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_ge);
		addr_test_method = IPV6CALC_TEST_GE;

	} else if (strncmp(token + offset, prefixaddreq_gt, strlen(prefixaddreq_gt)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr>' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_gt);
		addr_test_method = IPV6CALC_TEST_GT;

	} else if (strncmp(token + offset, prefixaddreq_le2, strlen(prefixaddreq_le2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr=le=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_le2);
		addr_test_method = IPV6CALC_TEST_LE;

	} else if (strncmp(token + offset, prefixaddreq_lt2, strlen(prefixaddreq_lt2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr=lt=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_lt2);
		addr_test_method = IPV6CALC_TEST_LT;

	} else if (strncmp(token + offset, prefixaddreq_ge2, strlen(prefixaddreq_ge2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr=ge=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_ge2);
		addr_test_method = IPV6CALC_TEST_GE;

	} else if (strncmp(token + offset, prefixaddreq_gt2, strlen(prefixaddreq_gt2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr=gt=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq_gt2);
		addr_test_method = IPV6CALC_TEST_GT;

	} else if (strncmp(token + offset, prefixaddreq, strlen(prefixaddreq)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'addr=' prefix in token: %s", token);
		addr = 1;
		offset += strlen(prefixaddreq);
		addr_test_method = IPV6CALC_TEST_PREFIX;


	} else if (strncmp(token + offset, prefixdbdot, strlen(prefixdbdot)) == 0) {
		/* prefixdb with dot found */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "found 'db.' prefix in token: %s", token);
		db = 1;

	} else if (strstr(token, ".") != NULL) {
		/* other prefix */
		DEBUGPRINT_WA(DEBUG_libipv6addr, "prefix did not match: %s", token + offset);
		return(1);
	};


	if ((db == 0) && (addr == 0)) {
		// typeinfo token
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "check token against: %s", ipv6calc_ipv6addrtypestrings[i].token);

			if (strcmp(ipv6calc_ipv6addrtypestrings[i].token, token + offset) == 0) {
				DEBUGPRINT_WA(DEBUG_libipv6addr, "token match: %s", ipv6calc_ipv6addrtypestrings[i].token);

				if (negate == 1) {
					filter->filter_typeinfo.typeinfo_may_not_have |= ipv6calc_ipv6addrtypestrings[i].number;
				} else {
					filter->filter_typeinfo.typeinfo_must_have |= ipv6calc_ipv6addrtypestrings[i].number;
				};
				filter->filter_typeinfo.active = 1;
				filter->active = 1;
				result = 0;
				break;
			};
		};

		// typeinfo2 token
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addr_type2_strings); i++ ) {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "check token against: %s", ipv6calc_ipv6addr_type2_strings[i].token);

			if (strcmp(ipv6calc_ipv6addr_type2_strings[i].token, token + offset) == 0) {
				DEBUGPRINT_WA(DEBUG_libipv6addr, "token match: %s", ipv6calc_ipv6addr_type2_strings[i].token);

				if (negate == 1) {
					filter->filter_typeinfo2.typeinfo_may_not_have |= ipv6calc_ipv6addr_type2_strings[i].number;
				} else {
					filter->filter_typeinfo2.typeinfo_must_have |= ipv6calc_ipv6addr_type2_strings[i].number;
				};
				filter->filter_typeinfo2.active = 1;
				filter->active = 1;
				result = 0;
				break;
			};
		};
	};

	if (db == 1) {
		// DB CC filter
		r = libipv6calc_db_cc_filter_parse(&filter->filter_db_cc, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};

		// DB ASN filter
		r = libipv6calc_db_asn_filter_parse(&filter->filter_db_asn, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};

		// DB registry filter
		r = libipv6calc_db_registry_filter_parse(&filter->filter_db_registry, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};
	};


	if (addr == 1) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "try to parse IPv6 address: %s", token + offset);
		r = addr_to_ipv6addrstruct(token + offset, resultstring, sizeof(resultstring), &ipv6addr);

		if (r == 0) {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "successfully parsed IPv6 address: %s", token + offset);

			// store address test method in 'flag_startend_use'
			ipv6addr.test_mode = addr_test_method;

			if (negate == 1) {
				if (filter->filter_addr.addr_may_not_have_max < IPV6CALC_FILTER_IPV6ADDR) {
					ipv6addr_copy(&filter->filter_addr.ipv6addr_may_not_have[filter->filter_addr.addr_may_not_have_max], &ipv6addr);
        				filter->filter_addr.addr_may_not_have_max++;
					filter->filter_addr.active = 1;
					filter->active = 1;
					result = 0;
				} else {
					ERRORPRINT_WA("filter token 'addr=' maxmimum reached for 'may not have': %d", filter->filter_addr.addr_may_not_have_max);
				};
			} else {
				if (filter->filter_addr.addr_must_have_max < IPV6CALC_FILTER_IPV6ADDR) {
					ipv6addr_copy(&filter->filter_addr.ipv6addr_must_have[filter->filter_addr.addr_must_have_max], &ipv6addr); 
        				filter->filter_addr.addr_must_have_max++;
					filter->filter_addr.active = 1;
					filter->active = 1;
					result = 0;
				} else {
					ERRORPRINT_WA("filter token 'addr=' maxmimum reached for 'must have': %d", filter->filter_addr.addr_must_have_max);
				};
			};
		};
	};

	if (result != 0) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "token not supported: %s", token);
		return (result);
	};

END_ipv6addr_filter_parse:
	return (result);
};


/*
 * check filter IPv6
 *
 * in : *filter    = filter structure
 * ret: 0:ok 1:problem
 */
int ipv6addr_filter_check(const s_ipv6calc_filter_ipv6addr *filter) {
	int result = 0, r, i;
	char resultstring[NI_MAXHOST];

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter general active          : %d", filter->active);

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo' active       : %d", filter->filter_typeinfo.active);
	if (filter->filter_typeinfo.active > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo/must_have'    : 0x%08x", filter->filter_typeinfo.typeinfo_must_have);
		DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo/may_not_have' : 0x%08x", filter->filter_typeinfo.typeinfo_may_not_have);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo2' active      : %d", filter->filter_typeinfo2.active);
	if (filter->filter_typeinfo2.active > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo2/must_have'   : 0x%08x", filter->filter_typeinfo2.typeinfo_must_have);
		DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'typeinfo2/may_not_have': 0x%08x", filter->filter_typeinfo2.typeinfo_may_not_have);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'addr' active           : %d", filter->filter_addr.active);
	if (filter->filter_addr.active > 0) {
		if (filter->filter_addr.addr_must_have_max > 0) {
			for (i = 0; i < filter->filter_addr.addr_must_have_max; i++) {
				ipv6addrstruct_to_compaddr(&filter->filter_addr.ipv6addr_must_have[i], resultstring, sizeof(resultstring));
				DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'addr/must_have'        : %s", resultstring);
			};
		};
		if (filter->filter_addr.addr_may_not_have_max > 0) {
			for (i = 0; i < filter->filter_addr.addr_may_not_have_max; i++) {
				ipv6addrstruct_to_compaddr(&filter->filter_addr.ipv6addr_may_not_have[i], resultstring, sizeof(resultstring));
				DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'addr/may_not_have'     : %s", resultstring);
			};
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'db.cc' active          : %d", filter->filter_db_cc.active);
	if (filter->filter_db_cc.active > 0) {
		r = libipv6calc_db_cc_filter_check(&filter->filter_db_cc, IPV6CALC_PROTO_IPV6);
		if (r > 0) { result = 1; };
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'db.asn' active         : %d", filter->filter_db_asn.active);
	if (filter->filter_db_asn.active > 0) {
		r = libipv6calc_db_asn_filter_check(&filter->filter_db_asn, IPV6CALC_PROTO_IPV6);
		if (r > 0) { result = 1; };
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "ipv6 filter 'db.registry' active    : %d", filter->filter_db_registry.active);
	if (filter->filter_db_registry.active > 0) {
		r = libipv6calc_db_registry_filter_check(&filter->filter_db_registry, IPV6CALC_PROTO_IPV6);
		if (r > 0) { result = 1; };
	};

//END_ipv6addr_filter_check:
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
	int result = 0, r, i, t;

	if (filter->active == 0) {
		DEBUGPRINT_NA(DEBUG_libipv6addr, "No filter active (SKIP)");
		return (1);
	};

	DEBUGPRINT_NA(DEBUG_libipv6addr, "start");

	if ((filter->filter_typeinfo.active > 0) || (filter->filter_typeinfo2.active > 0)) {
		if (ipv6addrp->flag_typeinfo == 0) {
			fprintf(stderr, "FATAL error, typeinfo not valid - FIX CODE of caller\n");
			exit(2);
		};
	};

	if (filter->filter_typeinfo.active > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "compare typeinfo against must_have: 0x%08x/0x%08x", ipv6addrp->typeinfo, filter->filter_typeinfo.typeinfo_must_have);

		if ((ipv6addrp->typeinfo & filter->filter_typeinfo.typeinfo_must_have) != filter->filter_typeinfo.typeinfo_must_have) {
			/* no match */
			result = 1;
		} else {
			if ((ipv6addrp->typeinfo & filter->filter_typeinfo.typeinfo_may_not_have) != 0) {
				/* no match */
				result = 1;
			};
		};
	};

	if (filter->filter_typeinfo2.active > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6addr, "compare typeinfo2 against must_have: 0x%08x/0x%08x", ipv6addrp->typeinfo2, filter->filter_typeinfo2.typeinfo_must_have);

		if ((ipv6addrp->typeinfo2 & filter->filter_typeinfo2.typeinfo_must_have) != filter->filter_typeinfo2.typeinfo_must_have) {
			/* no match */
			result = 1;
		} else {
			if ((ipv6addrp->typeinfo & filter->filter_typeinfo2.typeinfo_may_not_have) != 0) {
				/* no match */
				result = 1;
			};
		};
	};

	if (filter->filter_addr.active > 0) {
		if (filter->filter_addr.addr_must_have_max > 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "compare against ipv6addr/must_have");
			r = 1;
			for (i = 0; i < filter->filter_addr.addr_must_have_max; i++) {
				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare against ipv6addr/must_have filter number: %d", i);
				t = ipv6addr_compare(ipv6addrp, &filter->filter_addr.ipv6addr_must_have[i],
					(filter->filter_addr.ipv6addr_must_have[i].test_mode == IPV6CALC_TEST_PREFIX) ? 1 : 0);

				switch (filter->filter_addr.ipv6addr_must_have[i].test_mode) {
					case IPV6CALC_TEST_PREFIX:
						if (t != 0) { r = 0; }; break;

					case IPV6CALC_TEST_LE:
						if (t >  0) { r = 0; }; break;

					case IPV6CALC_TEST_LT:
						if (t >= 0) { r = 0; }; break;

					case IPV6CALC_TEST_GE:
						if (t <  0) { r = 0; }; break;

					case IPV6CALC_TEST_GT:
						if (t <= 0) { r = 0; }; break;

					default:
						ERRORPRINT_WA("unsupported test mode (FIX CODE): %d", filter->filter_addr.ipv6addr_must_have[i].test_mode);
						break;
				};

				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare against ipv6addr/must_have result filter number: %d r=%d", i, r);
			};
			if (r == 0) {
				/* no match */
				result = 1;
			};
			DEBUGPRINT_WA(DEBUG_libipv6addr, "compare against ipv6addr/must_have result: r=%d result=%d", r, result);
		};
		if (filter->filter_addr.addr_may_not_have_max > 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "compare against ipv6addr/may_not_have");
			r = 0;
			for (i = 0; i < filter->filter_addr.addr_may_not_have_max; i++) {
				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare against ipv6addr/may_not_have filter number: %d", i);
				t = ipv6addr_compare(ipv6addrp, &filter->filter_addr.ipv6addr_may_not_have[i],
					(filter->filter_addr.ipv6addr_may_not_have[i].test_mode == IPV6CALC_TEST_PREFIX) ? 1 : 0);

				switch (filter->filter_addr.ipv6addr_may_not_have[i].test_mode) {
					case IPV6CALC_TEST_PREFIX:
						if (t == 0) { r = 1; }; break;

					case IPV6CALC_TEST_LE:
						if (t <= 0) { r = 1; }; break;

					case IPV6CALC_TEST_LT:
						if (t <  0) { r = 1; }; break;

					case IPV6CALC_TEST_GE:
						if (t >= 0) { r = 1; }; break;

					case IPV6CALC_TEST_GT:
						if (t >  0) { r = 1; }; break;

					default:
						ERRORPRINT_WA("unsupported test mode (FIX CODE): %d", filter->filter_addr.ipv6addr_must_have[i].test_mode);
						break;
				};
				DEBUGPRINT_WA(DEBUG_libipv6addr, "compare against ipv6addr/may_not_have result filter number: %d r=%d", i, r);
			};
			if (r == 1) {
				/* match may_not_have*/
				result = 1;
			};
		};
	};

	if (filter->filter_db_cc.active > 0) {
		uint16_t cc_index = libipv6addr_cc_index_by_addr(ipv6addrp, NULL);

		if (libipv6calc_db_cc_filter(cc_index, &filter->filter_db_cc) > 0) {
			/* no match */
			result = 1;
		};
	};

	if (filter->filter_db_asn.active > 0) {
		uint32_t asn = libipv6addr_as_num32_by_addr(ipv6addrp, NULL);

		if (libipv6calc_db_asn_filter(asn, &filter->filter_db_asn) > 0) {
			/* no match */
			result = 1;
		};
	};

	if (filter->filter_db_registry.active > 0) {
		int registry = libipv6addr_registry_num_by_addr(ipv6addrp);

		if (libipv6calc_db_registry_filter(registry, &filter->filter_db_registry) > 0) {
			/* no match */
			result = 1;
		};
	};

	return (result);
};


/* get included IPv4 address from an IPv6 address */
/* in:	IPv6 address pointer (ro)
 *	selector: in case of Teredo (otherwise ignored):
 *	 IPV6_ADDR_SELECT_IPV4_DEFAULT (TEREDO_CLIENT)
 *	 IPV6_ADDR_SELECT_IPV4_TEREDO_SERVER
 *	 IPV6_ADDR_SELECT_IPV4_PREFIX2_LENGTH
 * mod:	IPv4 address pointer (rw)
 * ret: 0=ok, !=0: no IPv4 adress included
 */
int libipv6addr_get_included_ipv4addr(const ipv6calc_ipv6addr *ipv6addrp, ipv6calc_ipv4addr *ipv4addrp, const int selector) {
	int result = -1;
	uint32_t typeinfo, typeinfo2;
	int begin = -1, shift = 0;
	int i;
	uint8_t xor = 0;

	typeinfo = ipv6addrp->typeinfo;
	typeinfo2 = ipv6addrp->typeinfo2;

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Called with IPv6 address having typeinfo: 0x%08x-0x%08x", typeinfo, typeinfo2);

	if (selector == IPV6_ADDR_SELECT_IPV4_PREFIX2_LENGTH) {
		begin = ipv6addrp->prefix2length / 8;
		shift = ipv6addrp->prefix2length % 8;
	} else {
		if ((typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED | IPV6_NEW_ADDR_NAT64)) != 0) {
			begin = 12;
		} else if ((typeinfo & IPV6_NEW_ADDR_TEREDO) != 0) {
			if (selector == IPV6_ADDR_SELECT_IPV4_DEFAULT) {
				// Teredo client
				begin = 12;
				xor   = 0xff;
			} else if (selector == IPV6_ADDR_SELECT_IPV4_TEREDO_SERVER) {
				// Teredo server
				begin = 4;
			} else {
				fprintf(stderr, "libipv6addr_get_included_ipv4addr FAILED (unsupported value of selector: %d - FIX CALLING CODE)", selector);
				exit(1);
			};
		} else if ((typeinfo & IPV6_NEW_ADDR_6TO4) != 0) {
			begin = 2;
		};
	};

	if (begin > 0) {
		if (shift > 0) {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "IPv6 address contains IPv4 address in octets %d-%d and shift %d", begin, begin+3, shift);
		} else {
			DEBUGPRINT_WA(DEBUG_libipv6addr, "IPv6 address contains IPv4 address in octets %d-%d", begin, begin+3);
		};

		ipv4addr_clearall(ipv4addrp);

		if (shift > 0) {
			for (i = 0; i <= 4; i++) {
				int ipv6_p = ipv6addr_getoctet(ipv6addrp, i + begin);

				DEBUGPRINT_WA(DEBUG_libipv6addr, "IPv6[%d]=%02x", i + begin, ipv6_p);
				ipv6_p <<= shift;

				if (ipv6_p > 0xff) {
					if (i > 0) {
						// update higher octet
						ipv4addr_setoctet(ipv4addrp
							, (unsigned int) i - 1
							, (unsigned int) (ipv4addr_getoctet(ipv4addrp, i - 1) | (ipv6_p >> 8))
						);
					};
				};

				if (i <= 3) {
					ipv4addr_setoctet(ipv4addrp
						, (unsigned int) i
						, (unsigned int) (ipv6_p & 0xff)
					);
				};
			};
		} else {
			for (i = 0; i <= 3; i++) {
				ipv4addr_setoctet(ipv4addrp
					, (unsigned int) i
					, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + begin)) ^ xor
				);
			};
		};

		ipv4addr_settype(ipv4addrp, 1);
		ipv4addrp->flag_valid = 1;

		result = 0;
	};

	if (result == -1) {
		// fprintf(stderr, "libipv6addr_get_included_ipv4addr FAILED (this should not happen)");
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr, "Return with result: %d", result);

	return(result);
};


/*
 * country code index of IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: country code index
 */
uint16_t libipv6addr_cc_index_by_addr(const ipv6calc_ipv6addr *ipv6addrp, unsigned int *data_source_ptr) {
	uint32_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipaddr ipaddr;
	int retval;

	DEBUGPRINT_NA(DEBUG_libipv6addr, "start");

	if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv6addr, "anonymized prefix handling");
		if ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) == 0) {
			if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_LISP) != 0) {
				retval = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_ASN32, &cc_index);
				cc_index >>= 20;
			} else {
				/* retrieve CountryCodeIndex from anonymization value */
				retval = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_CCINDEX, &cc_index);
				if (retval != 0) {
					fprintf(stderr, "Error getting CountryCode index from anonymized IPv6 address\n");
					goto END_libipv6addr_cc_index_by_addr;
				};
			};
		} else {
			if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4) != 0) {
				retval = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
				if (retval != 0) {
					fprintf(stderr, "Error getting included IPv4 address from anonymized IPv6 address\n");
					goto END_libipv6addr_cc_index_by_addr;
				};

				cc_index = libipv4addr_cc_index_by_addr(&ipv4addr, NULL);
			};
		};
	} else if (((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) != 0) && ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_IID) != 0)) {
		retval = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
		if (retval != 0) {
			fprintf(stderr, "Error getting included IPv4 address from anonymized IPv6 address\n");
			goto END_libipv6addr_cc_index_by_addr;
		};

		cc_index = libipv4addr_cc_index_by_addr(&ipv4addr, data_source_ptr);
	} else {
		if ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_IID) != 0) {
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) == 1) {
				/* retrieve CountryCodeIndex from IPv4 address inside */
				retval = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
				if (retval != 0) {
					fprintf(stderr, "Error getting included IPv4 address from IPv6 address\n");
					goto END_libipv6addr_cc_index_by_addr;
				};

				cc_index = libipv4addr_cc_index_by_addr(&ipv4addr, data_source_ptr);
			} else {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "DB feature missing: DB_IPV4_TO_CC");
			};
		} else if ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) != 0) {
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) == 1) {
				retval = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
				if (retval != 0) {
					fprintf(stderr, "Error getting included IPv4 address from IPv6 address\n");
					goto END_libipv6addr_cc_index_by_addr;
				};

				cc_index = libipv4addr_cc_index_by_addr(&ipv4addr, data_source_ptr);
			} else {
				DEBUGPRINT_NA(DEBUG_libipv6addr, "DB feature missing: DB_IPV4_TO_CC");
			};
		} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6BONE) != 0) {
			cc_index = COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN + IPV6_ADDR_REGISTRY_6BONE;
		} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_ORCHID) != 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "ORCHID has no country");
			goto END_libipv6addr_cc_index_by_addr;
		} else if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_CC) == 1) {
			if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_ANON_MASKED_PREFIX) != 0) {
				if (ipv6addrp->prefix2length < 48) {
					DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized prefix prefix length to low for detecting country: %u", ipv6addrp->prefix2length);
					goto END_libipv6addr_cc_index_by_addr;
				};
				ipv6addr = *ipv6addrp;
				ipv6addr.flag_prefixuse = 1;
				ipv6addr.prefixlength = ipv6addrp->prefix2length;
				ipv6addrstruct_maskprefix(&ipv6addr);
				DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized prefix found, apply prefix length %u", ipv6addr.prefixlength);
				CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
			} else {
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);
			};

			cc_index = libipv6calc_db_wrapper_cc_index_by_addr(&ipaddr, data_source_ptr);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "DB feature missing: DB_IPV6_TO_CC");
		};
	};

END_libipv6addr_cc_index_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv6addr, "cc_index=%d (0x%03x)", cc_index, cc_index);
	return(cc_index);
};


/*
 * 32-bit AS number of IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: 32-bit AS number
 */
uint32_t libipv6addr_as_num32_by_addr(const ipv6calc_ipv6addr *ipv6addrp, unsigned int *data_source_ptr) {
	uint32_t as_num32 = ASNUM_AS_UNKNOWN;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipaddr ipaddr;
	int retval;

	DEBUGPRINT_NA(DEBUG_libipv6addr, "start");

	if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) != 0) {
		if ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) == 0) {
			/* retrieve ASN from anonymization value */
			retval = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_ASN32, &as_num32);
			if (retval != 0) {
				fprintf(stderr, "Error getting ASN32 from anonymized IPv6 address\n");
				goto END_libipv6addr_as_num32_by_addr;
			};
		} else {
			if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4) != 0) {
				retval = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);
				if (retval != 0) {
					fprintf(stderr, "Error getting included IPv4 address from anonymized IPv6 address\n");
					goto END_libipv6addr_as_num32_by_addr;
				};

				as_num32 = libipv4addr_as_num32_by_addr(&ipv4addr, data_source_ptr);
			};
		};
	} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6BONE) != 0) {
		as_num32 = ASNUM_AS_UNKNOWN;
		goto END_libipv6addr_as_num32_by_addr;
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_AS) == 1) {
			if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_ANON_MASKED_PREFIX) != 0) {
				if (ipv6addrp->prefix2length < 48) {
					goto END_libipv6addr_as_num32_by_addr;
				};
				ipv6addr = *ipv6addrp;
				ipv6addr.flag_prefixuse = 1;
				ipv6addr.prefixlength = ipv6addrp->prefix2length;
				ipv6addrstruct_maskprefix(&ipv6addr);
				DEBUGPRINT_WA(DEBUG_libipv6addr, "anonymized prefix found, apply prefix length %u", ipv6addr.prefixlength);
				CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
			} else {
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);
			};

			as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(&ipaddr, data_source_ptr);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6addr, "DB feature missing: DB_IPV6_TO_AS");
		};
	};

END_libipv6addr_as_num32_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv6addr, "as_num32=%d (0x%08x)", as_num32, as_num32);
	return(as_num32);
};


/*
 * registry number of IPv6 address
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: registry number
 */
int libipv6addr_registry_num_by_addr(const ipv6calc_ipv6addr *ipv6addrp) {
	int registry = IPV6_ADDR_REGISTRY_UNKNOWN;
	uint16_t cc_index;
	ipv6calc_ipv6addr ipv6addr;

	if (((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) != 0) \
		&& ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) == 0)) {
			/* retrieve registry via cc_index from anonymized address (simple) */
			cc_index = libipv6addr_cc_index_by_addr(ipv6addrp, NULL);
			registry = libipv6calc_db_wrapper_registry_num_by_cc_index(cc_index);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_REGISTRY) == 1) {
			if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_ANON_MASKED_PREFIX) != 0) {
				if (ipv6addrp->prefix2length < 12) {
					goto END_libipv6addr_registry_by_addr;
				};
				ipv6addr = *ipv6addrp;
				ipv6addr.flag_prefixuse = 1;
				ipv6addr.prefixlength = ipv6addrp->prefix2length;
				ipv6addrstruct_maskprefix(&ipv6addr);
				registry = libipv6calc_db_wrapper_registry_num_by_ipv6addr(&ipv6addr);
			} else {
				registry = libipv6calc_db_wrapper_registry_num_by_ipv6addr(ipv6addrp);
			};
		};
	};

END_libipv6addr_registry_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv6addr, "registry=%d (0x%x)", registry, registry);
	return(registry);
};
