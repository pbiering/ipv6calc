/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id: showinfo.c,v 1.3 2002/01/20 09:44:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Function to show information about a given IPv6 address
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

	/* printf(stderr, "%s: function is still incomplete\n", DEBUG_function_name); */

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
		fprintf(stdout, "Address type is 6to4 and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(&ipv6addr, 2), ipv6addr_getoctett(&ipv6addr, 3), ipv6addr_getoctett(&ipv6addr, 4), ipv6addr_getoctett(&ipv6addr, 5));
	};

	/* SLA prefix included? */
	if (typeinfo & IPV6_NEW_ADDR_6TO4 || typeinfo & IPV6_ADDR_SITELOCAL || typeinfo & IPV6_NEW_ADDR_AGU || typeinfo & IPV6_NEW_ADDR_6BONE) {
		fprintf(stdout, "Address type has SLA: %04x\n", ipv6addr_getword(&ipv6addr, 3));
	};
	
	/* Proper solicited node link-local multicast address? */
	if (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) {
		if (typeinfo & IPV6_ADDR_LINKLOCAL && typeinfo & IPV6_ADDR_MULTICAST) {
			/* address is ok */
		} else {
			fprintf(stdout, "Address is not a proper 'solicited-node link-local multicast' address!\n");
			retval = 1;
			goto END;
		};
	};

	/* Interface identifier included */
	if (typeinfo & IPV6_ADDR_LINKLOCAL || typeinfo & IPV6_ADDR_SITELOCAL || typeinfo & IPV6_NEW_ADDR_AGU || typeinfo & IPV6_NEW_ADDR_6BONE || typeinfo & IPV6_NEW_ADDR_6TO4) {
		fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", ipv6addr_getword(&ipv6addr, 4), ipv6addr_getword(&ipv6addr, 5), ipv6addr_getword(&ipv6addr, 6), ipv6addr_getword(&ipv6addr, 7));
		if (ipv6addr_getoctett(&ipv6addr, 11) == 0xff && ipv6addr_getoctett(&ipv6addr, 12) == 0xfe) {
			/* EUI-48 */
			fprintf(stdout, "Interface identifier is an EUI-64 generated from EUI-48 (MAC): %02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(&ipv6addr, 8) ^ 0x02, ipv6addr_getoctett(&ipv6addr, 9), ipv6addr_getoctett(&ipv6addr, 10), ipv6addr_getoctett(&ipv6addr, 13), ipv6addr_getoctett(&ipv6addr, 14), ipv6addr_getoctett(&ipv6addr, 15));
			if (ipv6addr_getoctett(&ipv6addr, 8) & 0x02) {
				fprintf(stdout, "MAC is a global unique one\n");
			} else {
				fprintf(stdout, "MAC is a local one\n");
			};
		} else {
			/* Check for global EUI-64 */
			if (ipv6addr_getoctett(&ipv6addr, 8) & 0x02) {
				fprintf(stdout, "Interface identifier is probably EUI-64 based: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(&ipv6addr, 8) ^ 0x02, ipv6addr_getoctett(&ipv6addr, 9), ipv6addr_getoctett(&ipv6addr, 10), ipv6addr_getoctett(&ipv6addr, 11), ipv6addr_getoctett(&ipv6addr, 12), ipv6addr_getoctett(&ipv6addr, 13), ipv6addr_getoctett(&ipv6addr, 14), ipv6addr_getoctett(&ipv6addr, 15));
			} else {
				if (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) {
					fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", ipv6addr_getoctett(&ipv6addr, 13), ipv6addr_getoctett(&ipv6addr, 14), ipv6addr_getoctett(&ipv6addr, 15));
				} else if (typeinfo & IPV6_NEW_ADDR_ISATAP) {
					fprintf(stdout, "Address type is ISATAP and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(&ipv6addr, 12), ipv6addr_getoctett(&ipv6addr, 13), ipv6addr_getoctett(&ipv6addr, 14), ipv6addr_getoctett(&ipv6addr, 15));
				} else {
					fprintf(stdout, "Interface identifier is probably manual set or based on a local EUI-64 identifier");
				};
			};
		};
	};
END:	
	retval = 0;
	return (retval);
};

