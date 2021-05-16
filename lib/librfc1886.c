/*
 * Project    : ipv6calc
 * File       : librfc1886.c
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  RFC 1886 conform reverse nibble format string
 *
 *  Function to format a given address to reverse nibble-by-nibble ip6.int|arpa format
 *
 * Intention from the Perl program "ip6_int" written by Keith Owens <kaos at ocs dot com dot au>
 * some hints taken from ifconfig.c (net-tools)
 * 
 * Credits to:
 *  Keith Owens <kaos at ocs dot com dot au>
 *	net-tools authors
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "libipv6addr.h"
#include "libipv6calc.h"
#include "librfc1886.h"


/*
 * converts IPv6addr_structure to a reverse nibble format string
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions, const char *domain) {
	int retval = 1;
	unsigned int nibble;
	int bit_start, bit_end, nbit;
	char tempstring[NI_MAXHOST];
	unsigned int nnibble, noctet;
	
	DEBUGPRINT_WA(DEBUG_librfc1886, "flag_prefixuse %d", (*ipv6addrp).flag_prefixuse);

	/* 20100909: take care of prefix length before printing the nibbles, but break old behavior */
	/* 20140331: mask prefix only in case of no printsuffix,printstart,printend */
	if ((*ipv6addrp).flag_prefixuse != 0 && ((formatoptions & (FORMATOPTION_printsuffix | FORMATOPTION_printstart | FORMATOPTION_printend)) == 0)) {
		ipv6addrstruct_maskprefix(ipv6addrp);
	};

	if ( ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_printstart | FORMATOPTION_printend)) == 0 ) && ((*ipv6addrp).flag_prefixuse != 0) ) {
		/* simulate old behavior */
		bit_start = 1;
		bit_end = (int) (*ipv6addrp).prefixlength;
		DEBUGPRINT_NA(DEBUG_librfc1886, "simulate old behavior");
	} else if ( (*ipv6addrp).flag_startend_use != 0 ) {
		/* check start and end */
		if ( (((*ipv6addrp).bit_start - 1) & 0x03) != 0 ) {
			snprintf(resultstring, resultstring_length, "Start bit number '%u' is not supported because of non-unique representation (value-1 must be dividable by 4)", (unsigned int) (*ipv6addrp).bit_start);
			retval = 1;
			return (retval);
		};
		if ( ((*ipv6addrp).bit_end & 0x03) != 0 ) {
			snprintf(resultstring, resultstring_length, "End bit number '%u' is not supported because of non-unique representation (value must be dividable by 4)", (unsigned int) (*ipv6addrp).bit_end);
			retval = 1;
			return (retval);
		};

		bit_start = (int) (*ipv6addrp).bit_start;
		bit_end = (int) (*ipv6addrp).bit_end;
	} else {
		bit_start = 1;
		bit_end = 128;
	};
	
	DEBUGPRINT_WA(DEBUG_librfc1886, "start bit %d  end bit %d", bit_start, bit_end);

	/* print out nibble format */
	/* 127 is lowest bit, 0 is highest bit */
	resultstring[0] = '\0';

	for (nbit = bit_end - 1; nbit >= bit_start - 1; nbit = nbit - 4) {
		/* calculate octet (8 bit) */
		noctet = ( ((unsigned int) nbit) & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = ( ((unsigned int) nbit) & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctet] & ( 0xf << (unsigned int) (4 * (1 - nnibble)) ) ) >> (unsigned int) ( 4 * (1 - nnibble));
		
		DEBUGPRINT_WA(DEBUG_librfc1886, "bit: %d = noctet: %u, nnibble: %u, octet: %02x, value: %x", nbit, noctet, nnibble, (unsigned int) (*ipv6addrp).in6_addr.s6_addr[noctet], nibble);

		snprintf(tempstring, sizeof(tempstring), "%s%x", resultstring, nibble);
		if ((nbit < bit_start) && (bit_start != 1)) {
			/* don't print trailing "." on middle part end */
			snprintf(resultstring, resultstring_length, "%s", tempstring);
		} else {
			snprintf(resultstring, resultstring_length, "%s.", tempstring);
		};
	};

	if (bit_start == 1) {
		snprintf(tempstring, sizeof(tempstring), "%s%s", resultstring, domain);
	};

	snprintf(resultstring, resultstring_length, "%s", tempstring);

	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};

	if ( (formatoptions & FORMATOPTION_printmirrored) != 0 ) {
		string_to_reverse_dotted(resultstring, resultstring_length);
	};
		
	DEBUGPRINT_WA(DEBUG_librfc1886, "Print out: %s", resultstring);

	retval = 0;
	return (retval);
};


/*
 * function a reverse nibble format string into IPv6addr_structure
 *
 * in : inputstring
 * mod: *ipv6addrp = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int librfc1886_nibblestring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length) {
	int retval = 1;
	char tempstring[NI_MAXHOST], *token, *cptr, **ptrptr;
	int flag_tld = 0, flag_nld = 0, tokencounter = 0;
	unsigned int noctet, nibblecounter = 0;
	int xdigit;

	if ((strlen(inputstring) < 4) || (strlen(inputstring) > 73)) {
		/* min: .int */
		/* max: f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.ip6.arpa. */
		snprintf(resultstring, resultstring_length, "Error in given nibble string, has not 4 to 73 chars!");
		return (1);
	};

	ptrptr = &cptr;

	/* clear output structure */
	ipv6addr_clearall(ipv6addrp);

	/* reverse copy of string */
	snprintf(tempstring, sizeof(tempstring), "%s", inputstring);
	string_to_lowcase(tempstring);

	/* check string */
	retval = librfc1886_formatcheck(tempstring, resultstring, resultstring_length);
	if (retval != 0) {
		return (1);
	};
	
	string_to_reverse(tempstring);	
	
	DEBUGPRINT_WA(DEBUG_librfc1886, "reverse copied string: %s", tempstring);

	/* run through nibbles */
	token = strtok_r(tempstring, ".", ptrptr);

	while(token != NULL) {
		if (strcmp(token, "apra") == 0) {
			if (flag_tld == 0) {
				flag_tld = 1;
				goto NEXT_token_nibblestring_to_ipv6addrstruct;
			} else {
				snprintf(resultstring, resultstring_length, "Top level domain 'arpa' is in wrong place");
				return (1);
			};
		};
		if (strcmp(token, "tni") == 0) {
			if (flag_tld == 0) {
				flag_tld = 1;
				goto NEXT_token_nibblestring_to_ipv6addrstruct;
			} else {
				snprintf(resultstring, resultstring_length, "Top level domain 'int' is in wrong place");
				return (1);
			};
		};
		if (tokencounter == 1 && flag_tld == 1 && flag_nld == 0) {
			if (strcmp(token, "6pi") == 0) {
				flag_nld = 1;
				goto NEXT_token_nibblestring_to_ipv6addrstruct;
			} else {
				snprintf(resultstring, resultstring_length, "Next level domain 'ip6' is in wrong place or missing");
				return (1);
			};
		};

		/* now proceed nibbles */
		if (strlen(token) > 1) {
			string_to_reverse(token);
			snprintf(resultstring, resultstring_length, "Nibble '%s' on dot position %d (from right side) is longer than one char", token, tokencounter + 1);
			return (1);
		};
		
		if (! isxdigit((int) token[0])) {
			snprintf(resultstring, resultstring_length, "Nibble '%s' on dot position %d (from right side) is not a valid hexdigit", token, tokencounter + 1);
			return (1);
		};

		retval = sscanf(token, "%x", &xdigit);
		if (retval != 1) {
			snprintf(resultstring, resultstring_length, "Nibble '%s' on dot position %d (from right side) cannot be parsed", token, tokencounter + 1);
			return (1);
		};

		if ( xdigit < 0 || xdigit > 0xf ) {
			snprintf(resultstring, resultstring_length, "Nibble '%s' on dot position %d (from right side) is out of range", token, tokencounter + 1);
			return (1);
		};

		noctet = nibblecounter >> 1; /* divided by 2 */
		
		if (noctet > 15) {
			snprintf(resultstring, resultstring_length, "Too many nibbles");
			return (1);
		};

		if ( (nibblecounter & 0x01) != 0 ) {
			/* most significant bits */
			(*ipv6addrp).in6_addr.s6_addr[noctet] = ((*ipv6addrp).in6_addr.s6_addr[noctet] & 0xf0) | xdigit;
		} else {
			/* least significant bits */
			(*ipv6addrp).in6_addr.s6_addr[noctet] = ((*ipv6addrp).in6_addr.s6_addr[noctet] & 0x0f) | ((uint8_t) xdigit << 4);
		};

		nibblecounter++;
		
NEXT_token_nibblestring_to_ipv6addrstruct:
		token = strtok_r(NULL, ".", ptrptr);
		tokencounter++;
	};

	ipv6addrp->flag_valid = 1;
	ipv6addrp->flag_prefixuse = 1;
	ipv6addrp->prefixlength = (uint8_t) nibblecounter << 2;
	
	retval = 0;
	return (retval);
};

/*
 * checks for proper format of a nibble string
 *  suffix has to be ip6.int or ip6.arpa
 *
 * in : string
 * ret: ==0: ok, !=0: error
 */
int librfc1886_formatcheck(const char *string, char *infostring, const size_t infostring_length) {
	int nibblecounter = 0, flag_tld = 0, flag_nld = 0, tokencounter = 0;
	char tempstring[NI_MAXHOST], *token, *cptr, **ptrptr;

	ptrptr = &cptr;

	infostring[0] = '\0'; /* clear string */

        if (strlen(string) >= sizeof(tempstring)) {
		fprintf(stderr, "Input too long: %s\n", string);
		return (1);
	};

	snprintf(tempstring, sizeof(tempstring), "%s", string);
	
	DEBUGPRINT_WA(DEBUG_librfc1886, "check %s", tempstring);

	string_to_reverse(tempstring);	
	
	/* run through nibbles */
	token = strtok_r(tempstring, ".", ptrptr);

	while(token != NULL) {
		DEBUGPRINT_WA(DEBUG_librfc1886, "check token: %s (tld: %d, nld: %d, tokencounter: %d)", token, flag_tld, flag_nld, tokencounter + 1);

		if (strcmp(token, "apra") == 0) {
			/* arpa (reverse) */
			DEBUGPRINT_NA(DEBUG_librfc1886, "found: arpa");
			if (flag_tld == 0 && tokencounter == 0) {
				DEBUGPRINT_NA(DEBUG_librfc1886, "found TLD: arpa");
				flag_tld = 1;
				goto NEXT_librfc1886_formatcheck;
			} else {
				snprintf(infostring, infostring_length, "Top level domain 'arpa' is in wrong place");
				return (1);
			};
		};
		if (strcmp(token, "tni") == 0) {
			/* int (reverse) */
			DEBUGPRINT_NA(DEBUG_librfc1886, "found: int");
			if (flag_tld == 0 && tokencounter == 0) {
				DEBUGPRINT_NA(DEBUG_librfc1886, "found TLD: int");
				flag_tld = 1;
				goto NEXT_librfc1886_formatcheck;
			} else {
				snprintf(infostring, infostring_length, "Top level domain 'int' is in wrong place");
				return (1);
			};
		};
		if (strcmp(token, "6pi") == 0) {
			/* ip6 (reverse) */
			DEBUGPRINT_NA(DEBUG_librfc1886, "found: ip6");
			if (tokencounter == 1 && flag_tld == 1 && flag_nld == 0) {
				DEBUGPRINT_NA(DEBUG_librfc1886, "found NLD: ip6");
				flag_nld = 1;
				goto NEXT_librfc1886_formatcheck;
			} else {
				snprintf(infostring, infostring_length, "Next level domain 'ip6' is in wrong place or missing");
				return (1);
			};
		};

		if ((flag_tld != 1) && (flag_nld != 1)) {
			snprintf(infostring, infostring_length, "Nibble string misses suffix ip6.int or ip6.arpa");
			return (1);
		};

		/* now proceed nibbles */
		if (strlen(token) > 1) {
			string_to_reverse(token);
			snprintf(infostring, infostring_length, "Nibble '%s' on dot position %d (from right side) is longer than one char", token, tokencounter + 1);
			return (1);
		};
		
		if (! isxdigit((int) token[0])) {
			snprintf(infostring, infostring_length, "Nibble '%s' on dot position %d (from right side) is not a valid hexdigit", token, tokencounter + 1);
			return (1);
		};

		nibblecounter++;
		
		if (nibblecounter > 32) {
			snprintf(infostring, infostring_length, "Too many nibbles (more than 32)");
			return (1);
		};
		
NEXT_librfc1886_formatcheck:
		token = strtok_r(NULL, ".", ptrptr);
		tokencounter++;
	};

	DEBUGPRINT_WA(DEBUG_librfc1886, "check %s is ok", string);

	return (0);
};
