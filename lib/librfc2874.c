/*
 * Project    : ipv6calc
 * File       : librfc2874.c
 * Version    : $Id: librfc2874.c,v 1.13 2011/09/16 18:05:13 peter Exp $
 * Copyright  : 2002-2009 by Peter Bieringer <pb (at) bieringer.de>
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
#define DEBUG_function_name "librfc2874/addr_to_bitstring"
int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int nibble;
	unsigned int noctet, nbit, nnibble, prefixlength, bit_start, bit_end;
	char tempstring[NI_MAXHOST];

	if ( (*ipv6addrp).flag_startend_use != 0 ) {
		/* check start and end */
		if ( (((*ipv6addrp).bit_start - 1) & 0x03) != 0 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "Start bit number '%u' not dividable by 4 aren't supported because of non unique representation", (unsigned int) (*ipv6addrp).bit_start);
			retval = 1;
			return (retval);
		};
		if ( ((*ipv6addrp).bit_end & 0x03 ) != 0 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "End bit number '%u' not dividable by 4 aren't supported because of non unique representation", (unsigned int) (*ipv6addrp).bit_end);
			retval = 1;
			return (retval);
		};

		bit_start = (unsigned int) (*ipv6addrp).bit_start;
		bit_end = (unsigned int) (*ipv6addrp).bit_end;
	} else {
		bit_start = 1;
		bit_end = 128;
	};

	if ( (ipv6calc_debug & DEBUG_librfc2874) != 0 ) {
		fprintf(stderr, "%s: print from start bit to end bit: %u - %u\n", DEBUG_function_name, bit_start, bit_end);
	};

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
		
		if ( (ipv6calc_debug & DEBUG_librfc2874) != 0 ) {
			fprintf(stderr, "%s: bit: %u= noctet: %u, nnibble: %u, octet: %02x, value: %x\n", DEBUG_function_name, nbit, noctet, nnibble, (unsigned int) (*ipv6addrp).in6_addr.s6_addr[noctet], nibble);
		};

		snprintf(tempstring, sizeof(tempstring) - 1, "%s%x", resultstring, nibble);
		if ( (ipv6calc_debug & DEBUG_librfc2874) != 0 ) {
			fprintf(stderr, "%s: Result after step %u: %s\n", DEBUG_function_name, nbit, tempstring);
		};
		strncpy(resultstring, tempstring, NI_MAXHOST);
	};

	/* add begin and end of label */
	if ( (*ipv6addrp).flag_startend_use != 0 ) {
		prefixlength = bit_end - bit_start + 1;
	} else {
		prefixlength = 128;
	};
	
	if ( (ipv6calc_debug & DEBUG_librfc2874) != 0 ) {
		fprintf(stderr, "%s: Result after expanding: %s\n", DEBUG_function_name, tempstring);
	};
	
	if ( bit_start != 1 ) {
		snprintf(tempstring, sizeof(tempstring) - 1,  "%s/%u]", resultstring, prefixlength);
	} else {
		snprintf(tempstring, sizeof(tempstring) - 1, "%s/%u].ip6.arpa.", resultstring, prefixlength);
	};
	
	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(tempstring);
	};

	snprintf(resultstring, NI_MAXHOST - 1, "\\[x%s", tempstring);

	retval = 0;

	return (retval);
};
#undef DEBUG_function_name


/*
 * converts a bitstring label into IPv6addr_structure
 *
 * in : inputstring
 * mod: *ipv6addrp = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc2874/bitstring_to_ipv6addrstruct"
int librfc2874_bitstring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1;
	char tempstring[NI_MAXHOST], tempstring2[NI_MAXHOST];
	unsigned int nibblecounter = 0;
	unsigned int noctet, startprefixlength, endprefixlength, index = 0, length, prefixlength;
	int xdigit;

	/* clear output structure */
	ipv6addr_clearall(ipv6addrp);

	/* reverse copy of string */
	snprintf(tempstring, sizeof(tempstring) - 1, "%s", inputstring);
	string_to_lowcase(tempstring);

	if ( (ipv6calc_debug & DEBUG_librfc2874) != 0 ) {
		fprintf(stderr, "%s: get string: %s\n", DEBUG_function_name, tempstring);
	};

	length = (unsigned int) strlen(tempstring);

	/* check content */
	retval = librfc2874_formatcheck(tempstring, resultstring);
	
	if (retval != 0) {
		return (1);
	}
	
	index = 3; /* start value */
	
	while(isxdigit((int) tempstring[index])) {
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%c", tempstring[index]);
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%x", &xdigit);

		if (retval != 1) {
			snprintf(resultstring, NI_MAXHOST - 1, "Nibble '%s' at position %u cannot be parsed", tempstring2, index + 1);
			return (1);
		};

		if (xdigit > 0xf) {
			snprintf(resultstring, NI_MAXHOST - 1, "Nibble '%s' at dot position %u is out of range", tempstring2, index + 1);
			return (1);
		};

		noctet = nibblecounter >> 1; /* divided by 2 */
		
		if (noctet > 15) {
			snprintf(resultstring, NI_MAXHOST - 1, "Too many nibbles");
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
		snprintf(resultstring, NI_MAXHOST - 1, "Unexpected end of string");
		return (1);
	};

	if (tempstring[index] == ']') {
		/* bitstring label closed */
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) nibblecounter << 2;
		goto END_bitstring_to_ipv6addrstruct;
	};
	index++;

	if (index > length) {
		snprintf(resultstring, NI_MAXHOST - 1, "Unexpected end of string");
		return (1);
	};

	/* proceed prefix length */
	if (tempstring[index] == '/') {
		snprintf(resultstring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", tempstring[index], index + 1);
		return (1);
	};
	index++;

	if (index > length) {
		snprintf(resultstring, NI_MAXHOST - 1, "Unexpected end of string");
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
			snprintf(resultstring, NI_MAXHOST - 1, "Invalid prefix length");
			return (1);
		};
		
		strncpy(tempstring2, tempstring + startprefixlength - 1, (size_t) (endprefixlength - startprefixlength + 1));
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%u", &prefixlength);

		if ( /*prefixlength < 0 || */ prefixlength > 128) {
			snprintf(resultstring, NI_MAXHOST - 1, "Given prefix length '%u' is out of range", prefixlength);
			return (1);
		};
		
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) prefixlength;
		goto END_bitstring_to_ipv6addrstruct;
	};

	snprintf(resultstring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", tempstring[index], index + 1);
	return (1);

END_bitstring_to_ipv6addrstruct:
	ipv6addrp->flag_valid = 1;
	
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * checks for proper format of a bitstring label
 *
 * in : string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc2874/formatcheck"
int librfc2874_formatcheck(const char *string, char *infostring) {
	unsigned int length, index = 0;
	unsigned int nibblecounter = 0, digitcounter = 0;

	infostring[0] = '\0'; /* clear string */
	
	length = (unsigned int) strlen(string);

	/* check length */
	if (length < 5) {
		snprintf(infostring, NI_MAXHOST - 1, "Error in given bitstring label, has less than 5 chars!");
		return (1);
	};
	
	/* check start */
	if (string[index] != '\\') {
		snprintf(infostring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;
	
	if (string[index] != '[') {
		snprintf(infostring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;
	
	if ( (char) tolower(string[index]) != 'x') {
		snprintf(infostring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;

	/* check nibble string */	
	while(isxdigit((int) string[index])) {
		nibblecounter++;

		if (nibblecounter > 32) {
			snprintf(infostring, NI_MAXHOST - 1, "More than 32 nibbles on position %u", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		snprintf(infostring, NI_MAXHOST - 1, "Unexpected end of string (missing '/' or ']')");
		return (1);
	};

	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};

	/* proceed prefix length */
	if (string[index] != '/') {
		snprintf(infostring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", string[index], index + 1);
		return (1);
	};
	index++;

	if (index >= length) {
		snprintf(infostring, NI_MAXHOST - 1, "Unexpected end of string (missing prefix length)");
		return (1);
	};
	
	/* check digit string */	
	while(isdigit((int) string[index])) {
		digitcounter++;

		if (digitcounter > 3) {
			snprintf(infostring, NI_MAXHOST - 1, "More than 3 digits on position %u", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		snprintf(infostring, NI_MAXHOST - 1, "Unexpected end of string (missing ']')");
		return (1);
	};
	
	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};
	
	snprintf(infostring, NI_MAXHOST - 1, "Char '%c' not expected on position %u", string[index], index + 1);

	return (1);
};
#undef DEBUG_function_name
