/*
 * Project    : ipv6calc
 * File       : libipv4addr.c
 * Version    : $Id: libipv4addr.c,v 1.4 2002/03/04 08:04:08 peter Exp $
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


/*
 * function returns an octett of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * in: numoctett  = number of octett (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getoctett"
unsigned int ipv4addr_getoctett(ipv6calc_ipv4addr *ipv4addrp, int numoctett) {
	unsigned int retval;
	
	if ( ( numoctett < 0 ) || ( numoctett > 3 ) ) {
		fprintf(stderr, "%s: given ocett number '%d' is out of range!\n", DEBUG_function_name, numoctett);
		exit(2);
	};

	retval = (ipv4addrp->in_addr.s_addr >> ( numoctett << 3)) & 0xff;

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
unsigned int ipv4addr_getword(ipv6calc_ipv4addr *ipv4addrp, int numword) {
	unsigned int retval;
	
	if ( ( numword < 0 ) || ( numword > 1 ) ) {
		fprintf(stderr, "%s: given word number '%d' is out of range!\n", DEBUG_function_name, numword);
		exit(2);
	};

	retval = (ipv4addrp->in_addr.s_addr >> (numword << 3) & 0xffff);

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
unsigned int ipv4addr_getdword(ipv6calc_ipv4addr *ipv4addrp) {
	unsigned int retval;
	
	retval = ipv4addrp->in_addr.s_addr;

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
void ipv4addr_setoctett(ipv6calc_ipv4addr *ipv4addrp, int numoctett, unsigned int value) {
	
	if ( ( numoctett < 0 ) || ( numoctett > 3 ) ) {
		fprintf(stderr, "%s: given octett number '%d' is out of range!\n", DEBUG_function_name, numoctett);
		exit(2);
	};
	
	if ( ( value < 0 ) || ( value > 0x0000000ff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv4addrp->in_addr.s_addr &= ~ (0xff  << (numoctett << 3) );
	ipv4addrp->in_addr.s_addr |= (value & 0xff) << (numoctett << 3);

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
void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, int numword, unsigned int value) {
	
	if ( ( numword < 0 ) || ( numword > 1 ) ) {
		fprintf(stderr, "%s: given word number '%d' is out of range!\n", DEBUG_function_name, numword);
		exit(2);
	};
	
	if ( ( value < 0 ) || ( value > 0x0000ffff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv4addrp->in_addr.s_addr &= ~ (0xffff  << (numword << 3) );
	ipv4addrp->in_addr.s_addr |= (value &0xffff)  << (numword << 3);
	
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
void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, unsigned int value) {
	
	if ( ( value < 0 ) || ( value > 0xffffffff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv4addrp->in_addr.s_addr = value;

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

	return;
};
#undef DEBUG_function_name


/*
 * function gets type of an IPv4 address
 *
 *   to be implemented...
 */

unsigned int ipv4addr_gettype(ipv6calc_ipv4addr *ipv4addrp) {
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
int addr_to_ipv4addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1, result, i, cpoints = 0;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];

	sprintf(resultstring, "%s", ""); /* clear result string */

	if ( ipv6calc_debug & DEBUG_libipv4addr ) {
		fprintf(stderr, "%s: got input %s\n", DEBUG_function_name,  addrstring);
	};
	
	ipv4addr_clearall(ipv4addrp);

	/* save prefix length first, if available */
	addronlystring = strtok (addrstring, "/");
	cp = strtok (NULL, "/");
	if ( cp != NULL ) {
		i = atol(cp);
		if (i < 0 || i > 32 ) {
			sprintf(resultstring, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv4addrp->flag_prefixuse = 1;
		ipv4addrp->prefixlength = i;
		
		if ( ipv6calc_debug & DEBUG_libipv4addr ) {
			fprintf(stderr, "%s: prefix length %d\n", DEBUG_function_name, ipv4addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv4addrp->flag_prefixuse);
		};
	};

	/* count ".", must be 3 */
	for (i = 0; i < strlen(addronlystring); i++) {
		if (addronlystring[i] == '.') {
			cpoints++;
		};
	};
	if ( cpoints != 3 ) {
		sprintf(resultstring, "Error, given address '%s' is not valid (only %d dots)!", addronlystring, cpoints);
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
			sprintf(resultstring, "Error, given IPv4 address '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	if ( ipv6calc_debug & DEBUG_libipv4addr ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		sprintf(resultstring, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		if ( ipv6calc_debug & DEBUG_libipv4addr ) {
			fprintf(stderr, "%s: Octett %d = %d\n", DEBUG_function_name, i, compat[i]);
		};
		ipv4addr_setoctett(ipv4addrp, i, compat[i]);
	};

	if ( ipv6calc_debug & DEBUG_libipv4addr ) {
		fprintf(stderr, "%s: In structure %03d %03d %03d %03d\n", DEBUG_function_name, ipv4addr_getoctett(ipv4addrp, 0), ipv4addr_getoctett(ipv4addrp, 1), ipv4addr_getoctett(ipv4addrp, 2), ipv4addr_getoctett(ipv4addrp, 3));
		fprintf(stderr, "%s: In structure %8x\n", DEBUG_function_name, ipv4addr_getdword(ipv4addrp));
	};
	
	result = ipv4addr_gettype(ipv4addrp); 

	if ( ipv6calc_debug & DEBUG_libipv4addr ) {
		fprintf(stderr, "%s: Got scope %02x\n", DEBUG_function_name, result);
	};

	ipv4addrp->scope = result;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores the ipv4addr in structure in a string
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = IPv4 address string
 * ret: ==0: ok, !=0: error
 */
int libipv4addr_ipv4addrstruct_to_string(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, unsigned long formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	sprintf(tempstring, "%d.%d.%d.%d", ipv4addr_getoctett(ipv4addrp, 0), ipv4addr_getoctett(ipv4addrp, 1), ipv4addr_getoctett(ipv4addrp, 2), ipv4addr_getoctett(ipv4addrp, 3));

	if (formatoptions & FORMATOPTION_machinereadable) {
		sprintf(resultstring, "IPV4=%s", tempstring);
	} else {
		sprintf(resultstring, "%s", tempstring);
	};

	/* netmaks */
	if (ipv4addrp->flag_prefixuse == 1) {
		/* to be filled */
		/* IPV4NETMASK= */
	};
	
	return(0);
};
