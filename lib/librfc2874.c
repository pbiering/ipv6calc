/*
 * Project    : ipv6calc
 * File       : librfc2874.c
 * Version    : $Id: librfc2874.c,v 1.1 2002/03/18 19:59:24 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
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
int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const unsigned long int formatoptions) {
	int retval = 1, result;
	unsigned int nibble;
	int noctett, nbit, nnibble, prefixlength, bit_start, bit_end;
	char tempstring[NI_MAXHOST];

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		result = ipv6addrstruct_to_uncompaddr(ipv6addrp, tempstring);
		fprintf(stderr, "%s: got address '%s'\n",  DEBUG_function_name, tempstring);
	};

	if ( (*ipv6addrp).flag_startend_use != 0 ) {
		/* check start and end */
		if ( ((*ipv6addrp).bit_start - 1) & 0x03 ) {
			sprintf(resultstring, "Start bit number '%d' not dividable by 4 aren't supported because of non unique representation", ((*ipv6addrp).bit_start));
			retval = 1;
			return (retval);
		};
		if ( (*ipv6addrp).bit_end & 0x03 ) {
			sprintf(resultstring, "End bit number '%d' not dividable by 4 aren't supported because of non unique representation", (*ipv6addrp).bit_end);
			retval = 1;
			return (retval);
		};

		bit_start = (*ipv6addrp).bit_start;
		bit_end = (*ipv6addrp).bit_end;
	} else {
		bit_start = 1;
		bit_end = 128;
	};

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		fprintf(stderr, "%s: print from start bit to end bit: %d - %d\n", DEBUG_function_name, bit_start, bit_end);
	};

	/* print out hex string format */
	/* 127 is lowest bit, 0 is highest bit */
	sprintf(resultstring, "%s", "");
	for (nbit = bit_start - 1; nbit <= bit_end - 1; nbit = nbit + 4) {
		/* calculate octett (8 bit) */
		noctett = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctett] & ( 0xf << (4 * (1 - nnibble)) ) ) >> ( 4 * (1 - nnibble));
		
		if ( ipv6calc_debug & DEBUG_librfc2874 ) {
			fprintf(stderr, "%s: bit: %d = noctett: %d, nnibble: %d, octett: %02x, value: %x\n", DEBUG_function_name, nbit, noctett, nnibble, (*ipv6addrp).in6_addr.s6_addr[noctett], nibble);
		};

		sprintf(tempstring, "%s%x", resultstring, nibble);
		sprintf(resultstring, "%s", tempstring);
	};

	/* add begin and end of label */
	if ((*ipv6addrp).flag_startend_use) {
		prefixlength = bit_end - bit_start + 1;
	} else {
		prefixlength = 128;
	};
	
	if ( bit_start != 1 ) {
		sprintf(tempstring, "%s/%d]", resultstring, prefixlength);
	} else {
		sprintf(tempstring, "%s/%d].ip6.arpa.", resultstring, prefixlength);
	};
	
	if (formatoptions & FORMATOPTION_printuppercase ) {
		string_to_upcase(tempstring);
	};

	sprintf(resultstring, "\\[x%s", tempstring);

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
	int nibblecounter = 0;
	int noctet, xdigit, startprefixlength, endprefixlength;
	int length, index = 0, prefixlength;

	/* clear output structure */
	ipv6addr_clearall(ipv6addrp);

	/* reverse copy of string */
	sprintf(tempstring, "%s", inputstring);
	string_to_lowcase(tempstring);

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		fprintf(stderr, "%s: get string: %s\n", DEBUG_function_name, tempstring);
	};

	length = strlen(tempstring);

	/* check content */
	retval = librfc2874_formatcheck(tempstring, resultstring);
	
	if (retval != 0) {
		return (1);
	}
	
	index = 3; /* start value */
	
	while(isxdigit(tempstring[index])) {
		sprintf(tempstring2, "%c", tempstring[index]);
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%x", &xdigit);

		if (retval != 1) {
			sprintf(resultstring, "Nibble '%s' at position %d cannot be parsed", tempstring2, index + 1);
			return (1);
		};

		if (xdigit < 0 || xdigit > 0xf) {
			sprintf(resultstring, "Nibble '%s' at dot position %d is out of range", tempstring2, index + 1);
			return (1);
		};

		noctet = nibblecounter >> 1; /* divided by 2 */
		
		if (noctet > 15) {
			sprintf(resultstring, "Too many nibbles");
			return (1);
		};

		if (nibblecounter & 0x01) {
			/* most significant bits */
			(*ipv6addrp).in6_addr.s6_addr[noctet] = ((*ipv6addrp).in6_addr.s6_addr[noctet] & 0xf0) | xdigit;
		} else {
			/* least significant bits */
			(*ipv6addrp).in6_addr.s6_addr[noctet] = ((*ipv6addrp).in6_addr.s6_addr[noctet] & 0x0f) | (xdigit << 4);
		};

		nibblecounter++;
		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index > length) {
		sprintf(resultstring, "Unexpected end of string");
		return (1);
	};

	if (tempstring[index] == ']') {
		/* bitstring label closed */
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = nibblecounter << 2;
		goto END_bitstring_to_ipv6addrstruct;
	};
	index++;

	if (index > length) {
		sprintf(resultstring, "Unexpected end of string");
		return (1);
	};

	/* proceed prefix length */
	if (tempstring[index] == '/') {
		sprintf(resultstring, "Char '%c' not expected on position %d", tempstring[index], index + 1);
		return (1);
	};
	index++;

	if (index > length) {
		sprintf(resultstring, "Unexpected end of string");
		return (1);
	};

	startprefixlength = index;
	endprefixlength = 0;
	while(isdigit(tempstring[index])) {
		index++;
		endprefixlength = index;
		if (index > length) {
			break;
		};
	};

	if (tempstring[index] == ']') {
		/* bitstring label closed */

		if (endprefixlength == 0) {
			sprintf(resultstring, "Invalid prefix length");
			return (1);
		};
		
		strncpy(tempstring2, tempstring + startprefixlength - 1, endprefixlength - startprefixlength + 1);
		
		/* now proceed nibbles */
		retval = sscanf(tempstring2, "%d", &prefixlength);

		if (prefixlength < 0 || prefixlength > 128) {
			sprintf(resultstring, "Given prefix length '%d' is out of range", prefixlength);
			return (1);
		};
		
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = prefixlength;
		goto END_bitstring_to_ipv6addrstruct;
	};

	sprintf(resultstring, "Char '%c' not expected on position %d", tempstring[index], index + 1);
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
	size_t length;
	int index = 0, nibblecounter = 0, digitcounter = 0;

	infostring[0] = '\0'; /* clear string */
	
	length = strlen(string);

	/* check length */
	if (length < 5) {
		sprintf(infostring, "Length %d too low", length);
		return (1);
	};
	
	/* check start */
	if (string[index] != '\\') {
		sprintf(infostring, "Char '%c' not expected on position %d", string[index], index + 1);
		return (1);
	};
	index++;
	
	if (string[index] != '[') {
		sprintf(infostring, "Char '%c' not expected on position %d", string[index], index + 1);
		return (1);
	};
	index++;
	
	if (tolower(string[index]) != 'x') {
		sprintf(infostring, "Char '%c' not expected on position %d", string[index], index + 1);
		return (1);
	};
	index++;

	/* check nibble string */	
	while(isxdigit(string[index])) {
		nibblecounter++;

		if (nibblecounter > 32) {
			sprintf(infostring, "More than 32 nibbles on position %d", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		sprintf(infostring, "Unexpected end of string (missing '/' or ']')");
		return (1);
	};

	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};

	/* proceed prefix length */
	if (string[index] != '/') {
		sprintf(infostring, "Char '%c' not expected on position %d", string[index], index + 1);
		return (1);
	};
	index++;

	if (index >= length) {
		sprintf(infostring, "Unexpected end of string (missing prefix length)");
		return (1);
	};
	
	/* check digit string */	
	while(isdigit(string[index])) {
		digitcounter++;

		if (digitcounter > 3) {
			sprintf(infostring, "More than 3 digits on position %d", index + 1);
			return (1);
		};

		index++;
		if (index > length) {
			break;
		};
	};
	
	if (index >= length) {
		sprintf(infostring, "Unexpected end of string (missing ']')");
		return (1);
	};
	
	if (string[index] == ']') {
		/* bitstring label closed */
		return (0);
	};
	
	sprintf(infostring, "Char '%c' not expected on position %d", string[index], index + 1);

	return (1);
};
#undef DEBUG_function_name
