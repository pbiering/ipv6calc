/*
 * Project    : ipv6calc
 * File       : addr_to_base85.c
 * Version    : $Id: addr_to_base85.c,v 1.2 2002/02/25 21:18:49 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  A function to format a given IPv6 address to a
 *   base85 one (RFC 1924)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "librfc1924.h"
#include "addr_to_base85.h"

void addr_to_base85_printhelp(void) {
	fprintf(stderr, " %s --addr_to_base85 <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
};

void addr_to_base85_printhelplong(void) {
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
int addr_to_base85(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1, result;

	result = ipv6addrstruct_to_base85(ipv6addrp, resultstring);
   
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
