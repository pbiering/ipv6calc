/*
 * addr_to_base85:
 *  A function to format a given IPv6 address to a
 *   base85 one (RFC 1924)
 *
 * Version:		$Id: addr_to_base85.c,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "librfc1924.h"

void addr_to_base85_printhelp() {
	fprintf(stderr, " %s --addr_to_base85 ipv6addr[/prefixlength]\n", PROGRAM_NAME);
};

void addr_to_base85_printhelplong() {
	addr_to_base85_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a base85 one (RFC 1924)\n");
	fprintf(stderr, "   e.g. 1080:0:0:0:8:800:200c:417a -> 4)+k&C#VzJ4br>0wv%%Yp \n");
};

/* function formats an given IPv6 address to a base85 one
 *  
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_base85"
int addr_to_base85(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;

	if ( ipv6calc_debug & DEBUG_addr_to_base85 ) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name,addrstring);
	};

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
	
	if ( ipv6calc_debug & DEBUG_addr_to_base85 ) {
		fprintf(stderr, "%s: result of uncomp2ipv6calc_ipv6addr: %d\n", DEBUG_function_name, result);
	};

	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	result = ipv6addrstruct_to_base85(&ipv6addr, resultstring);
   
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
