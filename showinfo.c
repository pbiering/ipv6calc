/*
 * showinfo:
 *  Function to show information about a given IPv6 address
 *
 * Version:		$Id: showinfo.c,v 1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6addr.h"

void showinfo_printhelp() {
	fprintf(stderr, " %s --showinfo|-i  ipv6addr[/prefixlength]\n", PROGRAM_NAME);
}; 

void showinfo_printhelplong() {
	addr_to_ip6int_printhelp();
	fprintf(stderr, "  Show information about a given IPv6 address\n");
};

/* function shows information about a given IPv6 address
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int showinfo(char *addrstring, char *resultstring) {
#define DEBUG_function_name "showinfo"
	int retval = 1, i, typeinfo, result;
	ipv6calc_ipv6addr ipv6addr;
/*	char tempstring[NI_MAXHOST];*/

	if (ipv6calc_debug & DEBUG_showinfo) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);

	if (ipv6calc_debug & DEBUG_showinfo) {
		fprintf(stderr, "%s: result of 'addr_to_ipv6addrstruct': %d\n", DEBUG_function_name, result);
	};
			 
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	typeinfo = ipv6addr_gettype(&ipv6addr);

	fprintf(stderr, "%s: function is still incomplete\n", DEBUG_function_name);

	if (ipv6calc_debug & DEBUG_showinfo) {
		fprintf(stderr, "%s: result of 'ipv6addr_gettype': %x\n", DEBUG_function_name, typeinfo);
	};

	for (i = 0; i < IPV6INFO_NUM; i++ ) {
		if (ipv6calc_debug & DEBUG_showinfo) {
			fprintf(stderr, "%s: test: %x : %s\n", DEBUG_function_name, ipv6addr_typesnum[i], ipv6addr_typesstring[i]);
		};

	};	
	
	fprintf(stdout, "Address type: ");
	for (i = 0; i < IPV6INFO_NUM; i++ ) {
		if (typeinfo & ipv6addr_typesnum[i]) {
			fprintf(stdout, " %s", ipv6addr_typesstring[i]);
		};
	};	

	fprintf(stdout, "\n");
	if (typeinfo & IPV6_NEW_ADDR_6TO4 ) {
		fprintf(stdout, "Address type is 6to4, included IPv4 address is '%d.%d.%d.%d'\n", ipv6addr_getoctett(&ipv6addr, 2), ipv6addr_getoctett(&ipv6addr, 3), ipv6addr_getoctett(&ipv6addr, 4), ipv6addr_getoctett(&ipv6addr, 5));
	};
	
	retval = 0;
	return (retval);
};

