/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id: showinfo.c,v 1.7 2002/03/03 14:54:03 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Function to show information about a given IPv6 address
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "libipv6addr.h"
#include "libipv6calc.h"

void showinfo_printhelp(void) {
	fprintf(stderr, " %s --showinfo|-i [--machine_readable|-m] <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
	fprintf(stderr, " %s --showinfo|-i --show_types\n", PROGRAM_NAME);
}; 

void showinfo_printhelplong(void) {
	showinfo_printhelp();
	fprintf(stderr, "  Show information about a given IPv6 address\n");
};

void showinfo_availabletypes(void) {
	int i;

	showinfo_printhelplong();
	fprintf(stderr, "\nAvailable tokens for machine-readable output (printed in one line):\n");
	fprintf(stderr, " TYPE (commata separated):\n");
	for (i = 0; i < sizeof(ipv6addr_typesnum) / sizeof(ipv6addr_typesnum[0]); i++ ) {
		fprintf(stderr, "  %s\n", ipv6addr_typesstring[i]);
	};
	fprintf(stderr, " IPV6=...                      : given IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV4=ddd.ddd.ddd.ddd          : an included IPv4 address\n");
	fprintf(stderr, " SLA=xxxx                      : an included SLA\n");
	fprintf(stderr, " IID=xxxx:xxxx:xxxx:xxxx       : an included interface identifier\n");
	fprintf(stderr, " IIDSCOPE=local|global         : scope of IID\n");
	fprintf(stderr, " EUI48=xx:xx:xx:xx:xx:xx       : an included EUI-48 (MAC) identifier\n");
	fprintf(stderr, " EUI64=xx:xx:xx:xx:xx:xx:xx:xx : an included EUI-64 identifier\n");
};

/* function shows information about a given IPv6 address
 *
 * in : *ipv4addrp = pointer to IPv6 address
 * ret: ==0: ok, !=0: error
 */
int showinfo_ipv6addr(ipv6calc_ipv6addr *ipv6addrp, unsigned long formatoptions) {
#define DEBUG_function_name "showinfo"
	int retval = 1, i, j, typeinfo;
	char tempstring[NI_MAXHOST] = "";

	typeinfo = ipv6addr_gettype(ipv6addrp);

	if (ipv6calc_debug & DEBUG_showinfo) {
		fprintf(stderr, "%s: result of 'ipv6addr_gettype': %x\n", DEBUG_function_name, typeinfo);
	};

	for (i = 0; i < sizeof(ipv6addr_typesnum) / sizeof(ipv6addr_typesnum[0]); i++ ) {
		if (ipv6calc_debug & DEBUG_showinfo) {
			fprintf(stderr, "%s: test: %x : %s\n", DEBUG_function_name, ipv6addr_typesnum[i], ipv6addr_typesstring[i]);
		};

	};	

	/* get full uncompressed IPv6 address */
	ipv6addrstruct_to_fulluncompaddr(ipv6addrp, tempstring);
	
	if (formatoptions & FORMATOPTION_machinereadable) {
		fprintf(stdout, "IPV6=%s", tempstring);

		j = 0;
		fprintf(stdout, " TYPE=");
		for (i = 0; i < sizeof(ipv6addr_typesnum) / sizeof(ipv6addr_typesnum[0]); i++ ) {
			if (typeinfo & ipv6addr_typesnum[i]) {
				if (j != 0) {
					fprintf(stdout, ",");
				};
				fprintf(stdout, "%s", ipv6addr_typesstring[i]);
				j = 1;
			};
		};
	} else {
		fprintf(stdout, "Address type: ");
		for (i = 0; i < sizeof(ipv6addr_typesnum) / sizeof(ipv6addr_typesnum[0]); i++ ) {
			if (typeinfo & ipv6addr_typesnum[i]) {
				fprintf(stdout, " %s", ipv6addr_typesstring[i]);
			};
		};
		fprintf(stdout, "\n");
	};	

	if (typeinfo & IPV6_NEW_ADDR_6TO4 ) {
		if (formatoptions & FORMATOPTION_machinereadable) {
			fprintf(stdout, " IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 2), ipv6addr_getoctett(ipv6addrp, 3), ipv6addr_getoctett(ipv6addrp, 4), ipv6addr_getoctett(ipv6addrp, 5));
		} else {
			fprintf(stdout, "Address type is 6to4 and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 2), ipv6addr_getoctett(ipv6addrp, 3), ipv6addr_getoctett(ipv6addrp, 4), ipv6addr_getoctett(ipv6addrp, 5));
		};
	};

	/* SLA prefix included? */
	if ( typeinfo & (IPV6_NEW_ADDR_6TO4 | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE) ) {
		if (formatoptions & FORMATOPTION_machinereadable) {
			fprintf(stdout, " SLA=%04x", ipv6addr_getword(ipv6addrp, 3));
		} else {
			fprintf(stdout, "Address type has SLA: %04x\n", ipv6addr_getword(ipv6addrp, 3));
		};
	};
	
	/* Proper solicited node link-local multicast address? */
	if (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) {
		if (typeinfo & IPV6_ADDR_LINKLOCAL && typeinfo & IPV6_ADDR_MULTICAST) {
			/* address is ok */
		} else {
			if (formatoptions & FORMATOPTION_machinereadable) {
			} else {
				fprintf(stdout, "Address is not a proper 'solicited-node link-local multicast' address!\n");
				retval = 1;
				goto END;
			};
		};
	};
	
	/* Compat or mapped */
	if ( typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED) ) {
		if (formatoptions & FORMATOPTION_machinereadable) {
			fprintf(stdout, " IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
		} else {
			fprintf(stdout, "Address type is compat/mapped and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
		};
	};

	/* Interface identifier included */
	if ( (typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4)) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) {
		if (formatoptions & FORMATOPTION_machinereadable) {
			fprintf(stdout, " IID=%04x:%04x:%04x:%04x", ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
		} else {
			fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
		};

		if (ipv6addr_getoctett(ipv6addrp, 11) == 0xff && ipv6addr_getoctett(ipv6addrp, 12) == 0xfe) {
			/* EUI-48 */
			if (formatoptions & FORMATOPTION_machinereadable) {
				fprintf(stdout, " EUI48=%02x:%02x:%02x:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
			} else {
				fprintf(stdout, "Interface identifier is an EUI-64 generated from EUI-48 (MAC): %02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
			};
			if (formatoptions & FORMATOPTION_machinereadable) {
				if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
					fprintf(stdout, " IIDSCOPE=global");
				} else {
					fprintf(stdout, " IIDSCOPE=local");
				};
			} else {
				if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
					fprintf(stdout, "MAC is a global unique one\n");
				} else {
					fprintf(stdout, "MAC is a local one\n");
				};
			};
		} else {
			/* Check for global EUI-64 */
			if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
				if (formatoptions & FORMATOPTION_machinereadable) {
					fprintf(stdout, " EUI64=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 11), ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					fprintf(stdout, " IIDSCOPE=global");
				} else {
					fprintf(stdout, "Interface identifier is probably EUI-64 based: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 11), ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
				};
			} else {
				if (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) {
					if (formatoptions & FORMATOPTION_machinereadable) {
						fprintf(stdout, " EUI64=??:??:??:??:??:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					} else {
						fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					};
				} else if (typeinfo & IPV6_NEW_ADDR_ISATAP) {
					if (formatoptions & FORMATOPTION_machinereadable) {
						fprintf(stdout, " IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					} else {
						fprintf(stdout, "Address type is ISATAP and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					};
				} else {
					if (formatoptions & FORMATOPTION_machinereadable) {
						fprintf(stdout, " EUI64=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 11), ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
						fprintf(stdout, " IIDSCOPE=local");
					} else {
						fprintf(stdout, "Interface identifier is probably manual set or based on a local EUI-64 identifier");
					};
				};
			};
		};
	};
END:	
	retval = 0;
	return (retval);
};

