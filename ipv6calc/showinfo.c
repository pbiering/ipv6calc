/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id: showinfo.c,v 1.4 2002/03/19 23:14:42 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Function to show information about a given IPv6 address
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "showinfo.h"
#include "ipv6calc.h"
#include "version.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libipv6calc.h"
#include "libieee.h"

/*
 * show available types on machine readable format
 */
void showinfo_availabletypes(void) {
	int i;

	fprintf(stderr, "\nAvailable tokens for machine-readable output (printed in one line):\n");
	fprintf(stderr, " TYPE (commata separated):\n");
	for (i = 0; i < sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0]); i++ ) {
		fprintf(stderr, "  %s", ipv6calc_ipv6addrtypestrings[i].token);
	};
	fprintf(stderr, "\n");
	fprintf(stderr, " IPV6=...                      : given IPv6 address full uncompressed\n");
	fprintf(stderr, " PREFIXLENGTH=ddd              : given prefix length\n");
	fprintf(stderr, " IPV4=ddd.ddd.ddd.ddd          : an included IPv4 address\n");
	fprintf(stderr, " SLA=xxxx                      : an included SLA\n");
	fprintf(stderr, " IID=xxxx:xxxx:xxxx:xxxx       : an included interface identifier\n");
	fprintf(stderr, " IIDSCOPE=local|global         : scope of IID\n");
	fprintf(stderr, " EUI48=xx:xx:xx:xx:xx:xx       : an included EUI-48 (MAC) identifier\n");
	fprintf(stderr, " EUI64=xx:xx:xx:xx:xx:xx:xx:xx : an included EUI-64 identifier\n");
	fprintf(stderr, " OUI=\"...\"                   : OUI string, if available\n");
	fprintf(stderr, " IPV6CALC_VERSION=x.y          : Version of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_COPYRIGHT=\"...\"    : Copyright string\n");
};

/*
 * print one information
 */
static void printout(const char *string) {
	const char *prefix = "";
	/* const char *prefix = "ipv6calc_"; */
	const char *suffix = "\n";
	
	fprintf(stdout, "%s%s%s", prefix, string, suffix);
};

static void printfooter(const unsigned long formatoptions) {
	char tempstring[NI_MAXHOST];

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(tempstring, sizeof(tempstring), "IPV6CALC_NAME=%s", PROGRAM_NAME);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring), "IPV6CALC_VERSION=%s", PROGRAM_VERSION);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring), "IPV6CALC_COPYRIGHT=\"%s\"", PROGRAM_COPYRIGHT);
		printout(tempstring);
	};
};

/*
 * function shows information about a given IPv6 address
 *
 * in : *ipv6addrp = pointer to IPv6 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_ipv6addr"
int showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp1, const unsigned long formatoptions) {
	int retval = 1, i, j, typeinfo, flag_prefixuse, result, registry;
	char tempstring[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	ipv6calc_ipv6addr ipv6addr, *ipv6addrp;

	int machinereadable = ( formatoptions & FORMATOPTION_machinereadable);

	ipv6addrp = &ipv6addr;
	ipv6addr_copy(ipv6addrp, ipv6addrp1);

	typeinfo = ipv6addr_gettype(ipv6addrp);
	registry = ipv6addr_getregistry(ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_showinfo)  != 0) {
		fprintf(stderr, "%s: result of 'ipv6addr_gettype': %x\n", DEBUG_function_name, typeinfo);
	};

	for (i = 0; i < sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0]); i++ ) {
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
			fprintf(stderr, "%s: test: %x : %s\n", DEBUG_function_name, ipv6calc_ipv6addrtypestrings[i].number, ipv6calc_ipv6addrtypestrings[i].token);
		};

	};	

	/* get full uncompressed IPv6 address */
	flag_prefixuse = ipv6addrp->flag_prefixuse;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrstruct_to_fulluncompaddr(ipv6addrp, helpstring);
	ipv6addrp->flag_prefixuse = flag_prefixuse;
	
	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring), "IPV6=%s", helpstring);
		printout(tempstring);
	
		if (ipv6addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring), "PREFIXLENGTH=%d", ipv6addrp->prefixlength);
			printout(tempstring);
		};

		j = 0;
		snprintf(tempstring, sizeof(tempstring), "TYPE=");
		for (i = 0; i < sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0]); i++ ) {
			if (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) {
				if (j != 0) {
					snprintf(helpstring, sizeof(tempstring), "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				};
				snprintf(helpstring, sizeof(tempstring), "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				j = 1;
			};
		};
		printout(tempstring);
	} else {
		fprintf(stdout, "Address type: ");
		j = 0;
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv6addrtypestrings[i].token);
				j = 1;
			};
		};
		fprintf(stdout, "\n");
	};	

	if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 2), ipv6addr_getoctett(ipv6addrp, 3), ipv6addr_getoctett(ipv6addrp, 4), ipv6addr_getoctett(ipv6addrp, 5));
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type is 6to4 and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 2), ipv6addr_getoctett(ipv6addrp, 3), ipv6addr_getoctett(ipv6addrp, 4), ipv6addr_getoctett(ipv6addrp, 5));
		};
	};

	/* SLA prefix included? */
	if ( typeinfo & ( IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU ) ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "SLA=%04x", ipv6addr_getword(ipv6addrp, 3));
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type has SLA: %04x\n", ipv6addr_getword(ipv6addrp, 3));
		};
	};
	
	/* Registry? */
	if ( registry != -1 ) {
		for ( i = 0; i < (int) (sizeof(ipv6calc_ipv6addrregistry) / sizeof(ipv6calc_ipv6addrregistry[0])); i++ ){
			if ( ipv6calc_ipv6addrregistry[i].number == registry ) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring), "REGISTRY=%s", ipv6calc_ipv6addrregistry[i].tokensimple);
					printout(tempstring);
				} else {
					fprintf(stdout, "Registry for address: %s\n", ipv6calc_ipv6addrregistry[i].token);
				};
			};
			break;
		};
	};
	
	/* Proper solicited node link-local multicast address? */
	if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
		if (typeinfo & IPV6_ADDR_LINKLOCAL && typeinfo & IPV6_ADDR_MULTICAST) {
			/* address is ok */
		} else {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Address is not a proper 'solicited-node link-local multicast' address!\n");
				retval = 1;
				goto END;
			};
		};
	};
	
	/* Compat or mapped */
	if ( (typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type is compat/mapped and included IPv4 address is: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
		};
	};

	/* Interface identifier included */
	if ( (typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU )) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "IID=%04x:%04x:%04x:%04x", ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
			printout(tempstring);
		} else {
			fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
		};

		if (ipv6addr_getoctett(ipv6addrp, 11) == 0xff && ipv6addr_getoctett(ipv6addrp, 12) == 0xfe) {
			/* EUI-48 */
			if ( machinereadable != 0 ) {
				snprintf(tempstring, sizeof(tempstring), "EUI48=%02x:%02x:%02x:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
				printout(tempstring);
			} else {
				fprintf(stdout, "Interface identifier is an EUI-64 generated from EUI-48 (MAC): %02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
			};

			if ( machinereadable != 0 ) {
				if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
					printout("IIDSCOPE=global");
				} else {
					printout("IIDSCOPE=local");
				};
			} else {
				if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
					fprintf(stdout, "MAC is a global unique one\n");
				} else {
					fprintf(stdout, "MAC is a local one\n");
				};
			};

			/* get vendor string */
			result = libieee_get_vendor_string(helpstring, ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10));
			if (result == 0) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring), "OUI=\"%s\"", helpstring);
					printout(tempstring);
				} else {
					fprintf(stdout, "OUI is: %s\n", tempstring);
				};
			};

			/* check for Linux ISDN-NET/PLIP */
			if (((ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02) == 0xfc) && (ipv6addr_getoctett(ipv6addrp, 9 == 0xfc))) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring), "IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					printout(tempstring);
				} else {
					fprintf(stdout, "Address type contains IPv4 address: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
				};
			};
			
		} else {
			/* Check for global EUI-64 */
			if (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring), "EUI64=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 11), ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					printout(tempstring);
					printout("IIDSCOPE=global");
				} else {
					fprintf(stdout, "Interface identifier is probably EUI-64 based: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10), ipv6addr_getoctett(ipv6addrp, 11), ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
				};
				
				/* get vendor string */
				result = libieee_get_vendor_string(helpstring, ipv6addr_getoctett(ipv6addrp, 8) ^ 0x02, ipv6addr_getoctett(ipv6addrp, 9), ipv6addr_getoctett(ipv6addrp, 10));
				if (result == 0) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring), "OUI=\"%s\"", helpstring);
						printout(tempstring);
					} else {
						fprintf(stdout, "OUI is: %s\n", tempstring);
					};
				};

			} else {
				if (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring), "EUI64=??:??:??:??:??:%02x:%02x:%02x", ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
						printout(tempstring);
					} else {
						fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					};
				} else if (typeinfo & IPV6_NEW_ADDR_ISATAP || (typeinfo & IPV6_ADDR_LINKLOCAL && ipv6addr_getdword(ipv6addrp, 2) == 0) ) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring), "IPV4=%d.%d.%d.%d", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
						printout(tempstring);
					} else {
						fprintf(stdout, "Address type contains IPv4 address: %d.%d.%d.%d\n", ipv6addr_getoctett(ipv6addrp, 12), ipv6addr_getoctett(ipv6addrp, 13), ipv6addr_getoctett(ipv6addrp, 14), ipv6addr_getoctett(ipv6addrp, 15));
					};
				} else {
					if ( machinereadable != 0 ) {
						printout("IIDSCOPE=local");
					} else {
						fprintf(stdout, "Interface identifier is probably manual set or based on a local EUI-64 identifier\n");
					};
				};
			};
		};
	};
END:	
	printfooter(formatoptions);
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function shows information about a given IPv4 address
 *
 * in : *ipv4addrp = pointer to IPv4 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_ipv4addr"
int showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const unsigned long formatoptions) {
	int retval = 1, typeinfo;
	char tempstring[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";

	typeinfo = ipv4addr_gettype(ipv4addrp);

	if (ipv6calc_debug & DEBUG_showinfo) {
		fprintf(stderr, "%s: result of 'ipv4addr_gettype': %x\n", DEBUG_function_name, typeinfo);
	};

	libipv4addr_ipv4addrstruct_to_string(ipv4addrp, helpstring, 0);
	
	if (formatoptions & FORMATOPTION_machinereadable) {
		snprintf(tempstring, sizeof(tempstring), "IPV4=%s", helpstring);
		printout(tempstring);
	
		if (ipv4addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring), "PREFIXLENGTH=%d", ipv4addrp->prefixlength);
			printout(tempstring);
		};
	} else {
		fprintf(stderr, "This printout is currently not supported\n");
	};	

	printfooter(formatoptions);
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
