/*
 * Project    : ipv6calc
 * File       : libipv4addr.c
 * Version    : $Id: libipv4addr.c,v 1.8 2002/04/10 07:00:43 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Function library for IPv4 storage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libipv4addr.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "../databases/ipv4-assignment/dbipv4addr_assignment.h"


/*
 * function returns an octett of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * in: numoctett  = number of octett (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getoctett"
uint8_t ipv4addr_getoctett(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctett) {
	uint8_t retval;
	
	if ( numoctett > 3 ) {
		fprintf(stderr, "%s: given ocett number '%u' is out of range!\n", DEBUG_function_name, numoctett);
		exit(EXIT_FAILURE);
	};

	retval = (uint8_t) ( (ipv4addrp->in_addr.s_addr >> ( (3 - numoctett) << 3)) & 0xff );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a word of an IPv4 address
 *
 * in: ipv6addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getword"
uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 1 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};

	retval = (uint16_t) ( (ipv4addr_getoctett(ipv4addrp, (numword << 1)) << 8 ) | ipv4addr_getoctett(ipv4addrp, (numword << 1) + 1) );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a dword of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getdword"
uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t retval;
	
	retval = (uint32_t) ( (ipv4addr_getword(ipv4addrp, 0) << 16) | ipv4addr_getword(ipv4addrp, 1) );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function sets an octett of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numoctett   = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setoctett"
void ipv4addr_setoctett(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctett, const unsigned int value) {
	
	if ( numoctett > 3 ) {
		fprintf(stderr, "%s: given octett number '%u' is out of range!\n", DEBUG_function_name, numoctett);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000000ff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv4addrp->in_addr.s_addr &= ~ (0xff  << ((3 - numoctett) << 3) );
	ipv4addrp->in_addr.s_addr |= (value & 0xff) << ((3 - numoctett) << 3);

	return;
};
#undef DEBUG_function_name


/*
 * function sets a word of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setword"
void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const unsigned int value) {
	
	if ( numword > 1 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000ffff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv4addr_setoctett(ipv4addrp, numword << 1, (value & 0xff00  >> 8) );
	ipv4addr_setoctett(ipv4addrp, (numword << 1) + 1, (value & 0xff) );
	
	return;
};
#undef DEBUG_function_name


/*
 * function sets a dword of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setdword"
void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int value) {
	
	if ( value > 0xffffffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv4addr_setword(ipv4addrp, 0, ((value & 0xffff0000u) >> 16) );
	ipv4addr_setword(ipv4addrp, 1, (value & 0xffffu) );

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_clear"
void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addrp->in_addr.s_addr = 0;
	
	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_clearall"
void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addr_clear(ipv4addrp);
	(*ipv4addrp).prefixlength = 0;
	(*ipv4addrp).flag_prefixuse = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function gets type of an IPv4 address
 *
 *   to be implemented...
 */

uint32_t ipv4addr_gettype(/*@unused@*/ const ipv6calc_ipv4addr *ipv4addrp) {
	return (0);
};

/*
 * function stores an IPv4 address string into a structure
 *
 * in : *addrstring = IPv4 address
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4calc/addr_to_ipv4addrstruct"
int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1, result, i, cpoints = 0;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[NI_MAXHOST], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: got input %s\n", DEBUG_function_name,  addrstring);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "%s: input too long: %s\n", DEBUG_function_name, addrstring);
		return (1);
	};

	strncpy(tempstring, addrstring, sizeof(tempstring) - 1);
	
	ipv4addr_clearall(ipv4addrp);

	/* save prefix length first, if available */
	addronlystring = strtok_r(tempstring, "/", ptrptr);
	cp = strtok_r(NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 32 ) {
			snprintf(resultstring, NI_MAXHOST, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv4addrp->flag_prefixuse = 1;
		ipv4addrp->prefixlength = (uint8_t) i;
		
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: prefix length %u\n", DEBUG_function_name, (unsigned int) ipv4addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv4addrp->flag_prefixuse);
		};
	};

	/* count ".", must be 3 */
	for (i = 0; i < (int) strlen(addronlystring); i++) {
		if (addronlystring[i] == '.') {
			cpoints++;
		};
	};
	if ( cpoints != 3 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, given address '%s' is not valid (only %d dots)!", addronlystring, cpoints);
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 3; i++ ) {
		compat[i] = 0;
	};

	expecteditems = 4;
	result = sscanf(addronlystring, "%d.%d.%d.%d", &compat[0], &compat[1], &compat[2], &compat[3]);
	
	for ( i = 0; i <= 3; i++ ) {
		if ( ( compat[i] < 0 ) || ( compat[i] > 255 ) )	{
			snprintf(resultstring, NI_MAXHOST, "Error, given IPv4 address '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: Octett %d = %d\n", DEBUG_function_name, i, compat[i]);
		};
		ipv4addr_setoctett(ipv4addrp, (unsigned int) i, (unsigned int) compat[i]);
	};

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: In structure %03u %03u %03u %03u\n", DEBUG_function_name, (unsigned int) ipv4addr_getoctett(ipv4addrp, 0), (unsigned int) ipv4addr_getoctett(ipv4addrp, 1), (unsigned int) ipv4addr_getoctett(ipv4addrp, 2), (unsigned int) ipv4addr_getoctett(ipv4addrp, 3));
		fprintf(stderr, "%s: In structure %8x\n", DEBUG_function_name, (unsigned int) ipv4addr_getdword(ipv4addrp));
	};
	
	typeinfo = ipv4addr_gettype(ipv4addrp); 

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Got scope %02x\n", DEBUG_function_name, (unsigned int) typeinfo);
	};

	ipv4addrp->scope = typeinfo;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name

/*
 * stores the ipv4addr structure in a string
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = IPv4 address string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4calc/ipv4addrstruct_to_string"
int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	snprintf(tempstring, sizeof(tempstring), "%u.%u.%u.%u", (unsigned int) ipv4addr_getoctett(ipv4addrp, 0), (unsigned int) ipv4addr_getoctett(ipv4addrp, 1), (unsigned int) ipv4addr_getoctett(ipv4addrp, 2), (unsigned int) ipv4addr_getoctett(ipv4addrp, 3));

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(resultstring, NI_MAXHOST, "IPV4=%s", tempstring);
	} else {
		snprintf(resultstring, NI_MAXHOST, "%s", tempstring);
	};

	/* netmaks */
	if (ipv4addrp->flag_prefixuse == 1) {
		/* to be filled */
		/* IPV4NETMASK= */
	};
	
	return(0);
};
#undef DEBUG_function_name


/*
 * get registry string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4calc/get_registry_string"
int libipv4addr_get_registry_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring) {
	int i, match = -1;
	uint32_t match_mask = 0;

	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Given IPv4 address: %08x\n", DEBUG_function_name, (unsigned int) ipv4);
	};

	/* run through database array */
	for (i = 0; i < (int) ( sizeof(dbipv4addr_assignment) / sizeof(dbipv4addr_assignment[0])); i++) {
		if ( (ipv4 & dbipv4addr_assignment[i].ipv4mask) == dbipv4addr_assignment[i].ipv4addr ) {
			/* ok, entry matches */
			if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
				fprintf(stderr, "%s: Found match number: %d\n", DEBUG_function_name, i);
			};

			/* have already found one */
			if ( match != -1 ) {
				if ( dbipv4addr_assignment[i].ipv4mask > match_mask ) {
					/* wins */
					if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
						fprintf(stderr, "%s: Overwrite match number: %d (old: %d)\n", DEBUG_function_name, i, match);
					};
					match = i;
					match_mask = dbipv4addr_assignment[i].ipv4mask;
				} else {
					if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
						fprintf(stderr, "%s: Overwrite match number: %d (old: %d)\n", DEBUG_function_name, i, match);
					};
				};
			} else {
				match = i;
				match_mask = dbipv4addr_assignment[i].ipv4mask;
			};
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Final match number: %d\n", DEBUG_function_name, match);
	};
	
	/* result */
	if ( match > -1 ) {
		snprintf(resultstring, NI_MAXHOST, "%s", dbipv4addr_assignment[match].string_registry);
		return(0);
	} else {
		snprintf(resultstring, NI_MAXHOST, "%s", "unknown");
		return(1);
	};
};
#undef DEBUG_function_name


/*
 * converts IPv4addr_structure to a reverse decimal format string
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/addr_to_reversestring"
int libipv4addr_to_reversestring(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int octett;
	int bit_start, bit_end, nbit;
	char tempstring[NI_MAXHOST];
	unsigned int noctett;
	
	if ( ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_printstart | FORMATOPTION_printend)) == 0 ) && ((*ipv4addrp).flag_prefixuse != 0) ) {
		bit_start = 1;
		bit_end = (int) (*ipv4addrp).prefixlength;
	} else {
		bit_start = 1;
		bit_end = 32;
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: start bit %d  end bit %d\n", DEBUG_function_name, bit_start, bit_end);
	};

	/* print out nibble format */
	/* 31 is lowest bit, 0 is highest bit */
	resultstring[0] = '\0';

	for (nbit = bit_end - 1; nbit >= bit_start - 1; nbit = nbit - 8) {
		/* calculate octett (8 bit) */
		noctett = ( ((unsigned int) nbit) & 0x78) >> 3;
		
		/* extract octett */
		octett = ipv4addr_getoctett(ipv4addrp, noctett);
		
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: bit: %d = noctett: %u, value: %x\n", DEBUG_function_name, nbit, noctett, octett);
		};

		snprintf(tempstring, sizeof(tempstring), "%s%u", resultstring, octett);
		snprintf(resultstring, NI_MAXHOST, "%s.", tempstring);
	};

	if (bit_start == 1) {
		snprintf(tempstring, sizeof(tempstring), "%sin-addr.arpa.", resultstring);
	};

	snprintf(resultstring, NI_MAXHOST, "%s", tempstring);

	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};
	
	if ( (formatoptions & FORMATOPTION_printmirrored) != 0 ) {
		string_to_reverse_dotted(resultstring);
	};
		
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name

