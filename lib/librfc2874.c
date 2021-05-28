/*
 * Project    : ipv6calc
 * File       : librfc2874.c
 * Version    : $Id$
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  RFC 2874 conform addresses (A6/DNAME) bitstring labels
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "libipv6addr.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "librfc2874.h"
#include "ipv6calctypes.h"


/*
 * converts IPv6addr_structure to a bitstring label
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int nibble;
	unsigned int noctet, nbit, nnibble, prefixlength, bit_start, bit_end;
	char tempstring[IPV6CALC_STRING_MAX];

	if ( (*ipv6addrp).flag_startend_use != 0 ) {
		/* check start and end */
		if ( (((*ipv6addrp).bit_start - 1) & 0x03) != 0 ) {
			snprintf(resultstring, resultstring_length, "Start bit number '%u' not dividable by 4 aren't supported because of non unique representation", (unsigned int) (*ipv6addrp).bit_start);
			retval = 1;
			return (retval);
		};
		if ( ((*ipv6addrp).bit_end & 0x03 ) != 0 ) {
			snprintf(resultstring, resultstring_length, "End bit number '%u' not dividable by 4 aren't supported because of non unique representation", (unsigned int) (*ipv6addrp).bit_end);
			retval = 1;
			return (retval);
		};

		bit_start = (unsigned int) (*ipv6addrp).bit_start;
		bit_end = (unsigned int) (*ipv6addrp).bit_end;
	} else {
		bit_start = 1;
		bit_end = 128;
	};

	DEBUGPRINT_WA(DEBUG_librfc2874, "print from start bit to end bit: %u - %u", bit_start, bit_end);

	/* print out hex string format */
	/* 127 is lowest bit, 0 is highest bit */
	resultstring[0] = '\0';
	for (nbit = bit_start - 1; nbit <= bit_end - 1; nbit = nbit + 4) {
		/* calculate octet (8 bit) */
		noctet = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctet] & ( 0xf << (unsigned int) (4 * (1 - nnibble)) ) ) >> (unsigned int) ( 4 * (1 - nnibble));
		
		DEBUGPRINT_WA(DEBUG_librfc2874, "bit: %u= noctet: %u, nnibble: %u, octet: %02x, value: %x", nbit, noctet, nnibble, (unsigned int) (*ipv6addrp).in6_addr.s6_addr[noctet], nibble);

		snprintf(tempstring, sizeof(tempstring), "%s%x", resultstring, nibble);
		snprintf(resultstring, resultstring_length, "%s", tempstring);

		DEBUGPRINT_WA(DEBUG_librfc2874, "Result after step %u (temp): %s", nbit, tempstring);
		DEBUGPRINT_WA(DEBUG_librfc2874, "Result after step %u (resu): %s", nbit, resultstring);
	};

	/* add begin and end of label */
	if ( (*ipv6addrp).flag_startend_use != 0 ) {
		prefixlength = bit_end - bit_start + 1;
	} else {
		prefixlength = 128;
	};
	
	DEBUGPRINT_WA(DEBUG_librfc2874, "Result after expanding: %s", tempstring);
	
	if ( bit_start != 1 ) {
		snprintf(tempstring, sizeof(tempstring),  "%s/%u]", resultstring, prefixlength);
	} else {
		snprintf(tempstring, sizeof(tempstring), "%s/%u].ip6.arpa.", resultstring, prefixlength);
	};
	
	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(tempstring);
	};

	snprintf(resultstring, resultstring_length, "\\[x%s", tempstring);

	DEBUGPRINT_WA(DEBUG_librfc2874, "Final result: %s", resultstring);

	retval = 0;

	return (retval);
};


/*
 * converts a bitstring label into IPv6addr_structure
 *
 * in : inputstring
 * mod: *ipv6addrp = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
int librfc2874_bitstring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length) {
	int retval = 1;
	char tempstring[IPV6CALC_STRING_MAX], tempstring2[IPV6CALC_STRING_MAX];
	unsigned int nibblecounter = 0;
	unsigned int noctet, startprefixlength, endprefixlength, index = 0, length, prefixlength;
	int xdigit;

	/* clear output structure */
	ipv6addr_clearall(ipv6addrp);

	/* reverse copy of string */
	snprintf(tempstring, sizeof(tempstring), "%s", inputstring);
	string_to_lowcase(tempstring);

	DEBUGPRINT_WA(DEBUG_librfc2874, "get string: %s", tempstring);

	length = (unsigned int) strlen(tempstring);

	/* check content */
	retval = librfc2874_formatcheck(tempstring, resultstring, resultstring_length);
	
	if (retval != 0) {
		return (1);
	}
	
	DEBUGPRINT_WA(DEBUG_librfc2874, "format is ok: %s", tempstring);

	index = 3; /* start value */
	
	while(isxdigit((int) tempstring[index])) {
		snprintf(tempstring2, sizeof(tempstring2), "%c", tempstring[index]);

		DEBUGPRINT_WA(DEBUG_librfc2874, "parse nibble: %s", tempstring2);
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%x", &xdigit);

		if (retval != 1) {
			snprintf(resultstring, resultstring_length, "Error in given bitstring label, nibble '%s' at position %u cannot be parsed", tempstring2, index + 1);
			return (1);
		};

		if (xdigit > 0xf) {
			snprintf(resultstring, resultstring_length, "Error in given bitstring label, nibble '%s' at dot position %u is out of range", tempstring2, index + 1);
			return (1);
		};

		noctet = nibblecounter >> 1; /* divided by 2 */
		
		if (noctet > 15) {
			snprintf(resultstring, resultstring_length, "Error in given bitstring label, too many nibbles");
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
		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index > length) {
		snprintf(resultstring, resultstring_length, "Error in given bitstring label, unexpected end of string");
		return (1);
	};

	if (tempstring[index] == ']') {
		/* bitstring label closed */
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) nibblecounter << 2;

		DEBUGPRINT_WA(DEBUG_librfc2874, "implicit prefix length: %d", ipv6addrp->prefixlength);

		goto END_bitstring_to_ipv6addrstruct;
	};
	index++;

	if (index > length) {
		snprintf(resultstring, resultstring_length, "Error in given bitstring label, unexpected end of string");
		return (1);
	};

	/* proceed prefix length */
	if (tempstring[index] == '/') {
		snprintf(resultstring, resultstring_length, "Error in given bitstring label, char '%c' not expected on position %u", tempstring[index], index + 1);
		return (1);
	};
	index++;

	if (index > length) {
		snprintf(resultstring, resultstring_length, "Error in given bitstring address, unexpected end of string");
		return (1);
	};

	startprefixlength = index;
	endprefixlength = 0;
	while(isdigit((int) tempstring[index])) {
		index++;
		endprefixlength = index;
		if (index > length) {
			break;
		};
	};

	if (tempstring[index] == ']') {
		/* bitstring label closed */

		if (endprefixlength == 0) {
			snprintf(resultstring, resultstring_length, "Invalid prefix length");
			return (1);
		};

		DEBUGPRINT_WA(DEBUG_librfc2874, "prefix length start=%d end=%d", startprefixlength, endprefixlength);

		snprintf(tempstring2, endprefixlength - startprefixlength + 2, "%s", tempstring + startprefixlength - 1);

		DEBUGPRINT_WA(DEBUG_librfc2874, "parse prefix length: %s", tempstring2);
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%u", &prefixlength);

		if ( /*prefixlength < 0 || */ prefixlength > 128) {
			snprintf(resultstring, resultstring_length, "Error in given bitstring label, given prefix length '%u' is out of range", prefixlength);
			return (1);
		};
		
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) prefixlength;

		DEBUGPRINT_WA(DEBUG_librfc2874, "explicit prefix length: %d", ipv6addrp->prefixlength);

		goto END_bitstring_to_ipv6addrstruct;
	};

	snprintf(resultstring, resultstring_length, "Error in given bitstring label, char '%c' not expected on position %u", tempstring[index], index + 1);
	return (1);

END_bitstring_to_ipv6addrstruct:
	ipv6addrp->flag_valid = 1;
	
	retval = 0;
	return (retval);
};


/*
 * checks for proper format of a bitstring label
 *
 * in : string
 * ret: ==0: ok, !=0: error
 */
int librfc2874_formatcheck(const char *string, char *infostring, const size_t infostring_length) {
	unsigned int length, index = 0;
	unsigned int nibblecounter = 0, digitcounter = 0;

	infostring[0] = '\0'; /* clear string */
	
	length = (unsigned int) strlen(string);

	/* check length */
	if (length < 5) {
		snprintf(infostring, infostring_length, "Error in given bitstring label, has less than 5 chars!");
		return (1);
	};
	
	/* check start */
	if (string[index] != '\\') {
		snprintf(infostring, infostring_length, "Error in given bitstring label, char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;
	
	if (string[index] != '[') {
		snprintf(infostring, infostring_length, "Error in given bitstring label, char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;
	
	if ( (char) tolower(string[index]) != 'x') {
		snprintf(infostring, infostring_length, "Error in given bitstring label, char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;

	/* check nibble string */	
	while(isxdigit((int) string[index])) {
		nibblecounter++;

		if (nibblecounter > 32) {
			snprintf(infostring, infostring_length, "Error in given bitstring label, more than 32 nibbles on position %u", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		snprintf(infostring, infostring_length, "Error in given bitstring label, unexpected end of string (missing '/' or ']')");
		return (1);
	};

	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};

	/* proceed prefix length */
	if (string[index] != '/') {
		snprintf(infostring, infostring_length, "Error in given bitstring label, char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;

	if (index >= length) {
		snprintf(infostring, infostring_length, "Error in given bitstring label, unexpected end of string (missing prefix length)");
		return (1);
	};
	
	/* check digit string */	
	while(isdigit((int) string[index])) {
		digitcounter++;

		if (digitcounter > 3) {
			snprintf(infostring, infostring_length, "Error in given bitstring label, more than 3 digits on position %u", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		snprintf(infostring, infostring_length, "Error in given bitstring label, unexpected end of string (missing ']')");
		return (1);
	};
	
	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};
	
	snprintf(infostring, infostring_length, "Error in given bitstring label, char '%c' not expected on position %u", string[index], index + 1);

	return (1);
};
