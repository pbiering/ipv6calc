/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id: showinfo.c,v 1.20 2005/10/21 13:42:33 peter Exp $
 * Copyright  : 2001-2005 by Peter Bieringer <pb (at) bieringer.de>
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
#include "libmac.h"
#include "libeui64.h"

/*
 * show available types on machine readable format
 */
void showinfo_availabletypes(void) {
	int i;

	fprintf(stderr, "\nAvailable tokens for machine-readable output (printed in one line):\n");
	fprintf(stderr, " TYPE=...                      : type of IPv6 address (commata separated)\n");
	fprintf(stderr, " ");
	for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
		fprintf(stderr, " %s", ipv6calc_ipv6addrtypestrings[i].token);
	};
	fprintf(stderr, "\n");
	fprintf(stderr, " IPV6=...                      : given IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV6_REGISTRY=...             : registry token of given IPv6 address\n");
	fprintf(stderr, " IPV6_PREFIXLENGTH=ddd         : given prefix length\n");
	fprintf(stderr, " IPV4=ddd.ddd.ddd.ddd          : an included IPv4 address in IID (e.g. ISATAP, TEREDO)\n");
	fprintf(stderr, " IPV4_REGISTRY=...             : registry token of IPv4 address in IID\n");
	fprintf(stderr, " IPV4_SOURCE=...               : source of IPv4 address\n");
	fprintf(stderr, "  ISATAP|TEREDO|6TO4|LINK-LOCAL-IID\n");
	fprintf(stderr, " IPV4_PREFIXLENGTH=...         : given prefix length of IPv4 address\n");
	fprintf(stderr, " SLA=xxxx                      : an included SLA\n");
	fprintf(stderr, " IID=xxxx:xxxx:xxxx:xxxx       : an included interface identifier\n");
	fprintf(stderr, " EUI48=xx:xx:xx:xx:xx:xx       : an included EUI-48 (MAC) identifier\n");
	fprintf(stderr, " EUI48_SCOPE=local|global      : scope of EUI-48 identifier\n");
	fprintf(stderr, " EUI48_TYPE=...                : type of EUI-48 identifier\n");
	fprintf(stderr, "  unicast|multicast|broadcast\n");
	fprintf(stderr, " EUI64=xx:xx:xx:xx:xx:xx:xx:xx : an included EUI-64 identifier\n");
	fprintf(stderr, " EUI64_SCOPE=local|global      : scope of EUI-64 identifier\n");
	fprintf(stderr, " OUI=\"...\"                     : OUI string, if available\n");
	fprintf(stderr, " TEREDO_IPV4_SERVER=...        : IPv4 address of Teredo server\n");
	fprintf(stderr, " TEREDO_PORT_CLIENT=...        : port of Teredo client (NAT outside)\n");
	fprintf(stderr, " TEREDO_IPV4_SERVER_REGISTRY=..: registry token of IPv4 address of Teredo server\n");
	fprintf(stderr, " IPV6CALC_VERSION=x.y          : Version of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_COPYRIGHT=\"...\"      : Copyright string\n");
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

static void printfooter(const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_NAME=%s", PROGRAM_NAME);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_VERSION=%s", PROGRAM_VERSION);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_COPYRIGHT=\"%s\"", PROGRAM_COPYRIGHT);
		printout(tempstring);
	};
};


/* print IPv4 address */
#define DEBUG_function_name "showinfo/print_ipv4addr"
static void print_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	char tempipv4string[NI_MAXHOST] = "";
	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);
	int retval;
	uint32_t typeinfo;

	typeinfo = ipv4addr_gettype(ipv4addrp);

	if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
		fprintf(stderr, "%s: result of 'ipv4addr_gettype': %x\n", DEBUG_function_name, (unsigned int) typeinfo);
	};

	retval = libipv4addr_ipv4addrstruct_to_string(ipv4addrp, tempipv4string, 0);
	if ( retval != 0 ) {
		fprintf(stderr, "Error converting IPv4 address: %s\n", tempipv4string);
	};	
	
	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV4=%s", tempipv4string);
		printout(tempstring);
	
		if (ipv4addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_PREFIXLENGTH=%d", (int) ipv4addrp->prefixlength);
			printout(tempstring);
		};
	} else {
		fprintf(stderr, "IPv4 address: %s\n", tempipv4string);
	};	

	/* get registry string */
	retval = libipv4addr_get_registry_string(ipv4addrp, helpstring);
	if ( retval != 0  && machinereadable == 0 ) {
		fprintf(stderr, "Error getting registry string for IPv4 address: %s (%s)\n", helpstring, tempipv4string);
		return;
	};
	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_REGISTRY=%s", helpstring);
		printout(tempstring);
	} else {
		fprintf(stderr, "IPv4 registry: %s\n", helpstring);
	};

	return;
};
#undef DEBUG_function_name


/*
 * print EUI-48/MAC information
 */
#define DEBUG_function_name "showinfo/print_eui48"
static void print_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int i, result;
	ipv6calc_ipv4addr ipv4addr;

	/* EUI-48/MAC address */
	snprintf(helpstring, sizeof(helpstring) - 1, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) macaddrp->addr[0], (unsigned int) macaddrp->addr[1], (unsigned int) macaddrp->addr[2], (unsigned int) macaddrp->addr[3], (unsigned int) macaddrp->addr[4], (unsigned int) macaddrp->addr[5]);

	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "EUI48=%s", helpstring);
		printout(tempstring);
	} else {
		fprintf(stdout, "EUI-48/MAC address: %s\n", helpstring);
	};

	/* scope */	
	if ( (macaddrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE=global");
		} else {
			fprintf(stdout, "MAC is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE=local");
		} else {
			fprintf(stdout, "MAC is a local one\n");
		};
	};
	
	/* unicast/multicast/broadcast */	
	if ( (macaddrp->addr[0] & 0x01) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_TYPE=unicast");
		} else {
			fprintf(stdout, "MAC is an unicast one\n");
		};
	} else {
		if ( (macaddrp->addr[0] == 0xff) && (macaddrp->addr[1] == 0xff) && (macaddrp->addr[2] == 0xff) && (macaddrp->addr[3] == 0xff) && (macaddrp->addr[4] == 0xff) && (macaddrp->addr[5] == 0xff) ) {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE=broadcast");
			} else {
				fprintf(stdout, "MAC is a broadcast one\n");
			};
		} else {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE=multicast");
			} else {
				fprintf(stdout, "MAC is a multicast one\n");
			};

		};
	};
	
	/* vendor string */
	result = libieee_get_vendor_string(helpstring, macaddrp);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "OUI=\"%s\"", helpstring);
			printout(tempstring);
		} else {
			fprintf(stdout, "OUI is: %s\n", helpstring);
		};
	};

	/* check for Linux ISDN-NET/PLIP */
	if ( (macaddrp->addr[0] == 0xfc) && (macaddrp->addr[1] == 0xfc) ) {
		/* copy address */
		for ( i = 0; i <= 3; i++ ) {
			ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) macaddrp->addr[i + 2]);
		};

		if ( machinereadable != 0 ) {
			/* no additional hint */
		} else {
			fprintf(stdout, "Address type contains IPv4 address:\n");
		};
		print_ipv4addr(&ipv4addr, formatoptions);
	};

	return;
};
#undef DEBUG_function_name


/*
 * print EUI-64 information
 */
#define DEBUG_function_name "showinfo/print_eui64"
static void print_eui64(const ipv6calc_eui64addr *eui64addrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int result, i;
	ipv6calc_macaddr macaddr;

	/* EUI-64 address */
	snprintf(helpstring, sizeof(helpstring) - 1, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) eui64addrp->addr[0], (unsigned int) eui64addrp->addr[1], (unsigned int) eui64addrp->addr[2], (unsigned int) eui64addrp->addr[3], (unsigned int) eui64addrp->addr[4], (unsigned int) eui64addrp->addr[5], (unsigned int) eui64addrp->addr[6], (unsigned int) eui64addrp->addr[7]);

	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "EUI64=%s", helpstring);
		printout(tempstring);
	} else {
		fprintf(stdout, "EUI-64 identifier: %s\n", helpstring);
	};
	
	/* scope */	
	if ( (eui64addrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE=global");
		} else {
			fprintf(stdout, "EUI-64 identifier is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE=local");
		} else {
			fprintf(stdout, "EUI-64 identifier is a local one\n");
		};
	};

	/* get vendor string */
	for (i = 0; i < 6; i++) {	
		macaddr.addr[i] = eui64addrp->addr[i];
	};

	result = libieee_get_vendor_string(helpstring, &macaddr);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "OUI=\"%s\"", helpstring);
			printout(tempstring);
		} else {
			fprintf(stdout, "OUI is: %s\n", tempstring);
		};
	};
	
	return;
};
#undef DEBUG_function_name

/*
 * function shows information about a given IPv6 address
 *
 * in : *ipv6addrp = pointer to IPv6 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_ipv6addr"
int showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp1, const uint32_t formatoptions) {
	int retval = 1, i, j, flag_prefixuse, registry;
	char tempstring[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	ipv6calc_ipv6addr ipv6addr, *ipv6addrp;
	ipv6calc_ipv4addr ipv4addr, ipv4addr2;
	ipv6calc_macaddr macaddr;
	ipv6calc_eui64addr eui64addr;
	uint16_t port;
	uint32_t typeinfo;
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);

	ipv6addrp = &ipv6addr;
	ipv6addr_copy(ipv6addrp, ipv6addrp1);

	typeinfo = ipv6addr_gettype(ipv6addrp);
	registry = ipv6addr_getregistry(ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
		fprintf(stderr, "%s: result of 'ipv6addr_gettype'    : %x\n", DEBUG_function_name, (unsigned int) typeinfo);
		fprintf(stderr, "%s: result of 'ipv6addr_getregistry': %d\n", DEBUG_function_name, registry);
	};

	for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
			fprintf(stderr, "%s: test: %x : %s\n", DEBUG_function_name, (unsigned int) ipv6calc_ipv6addrtypestrings[i].number, ipv6calc_ipv6addrtypestrings[i].token);
		};

	};	

	/* get full uncompressed IPv6 address */
	flag_prefixuse = ipv6addrp->flag_prefixuse;
	ipv6addrp->flag_prefixuse = 0;
	retval = libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6addrp, helpstring, FORMATOPTION_printfulluncompressed);
	if ( retval != 0 ) {
		fprintf(stderr, "Error uncompressing IPv6 address: %s\n", helpstring);
		retval = 1;
		goto END;
	};	

	ipv6addrp->flag_prefixuse = flag_prefixuse;
	
	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6=%s", helpstring);
		printout(tempstring);
	
		if (ipv6addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_PREFIXLENGTH=%d", (int) ipv6addrp->prefixlength);
			printout(tempstring);
		};

		j = 0;
		snprintf(tempstring, sizeof(tempstring) - 1, "TYPE=");
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring) - 1, "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring) - 1, "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
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
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) 2 + i));
		};

		retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, helpstring, 0);
		if ( retval != 0 ) {
			fprintf(stderr, "Error converting IPv4 address to string\n");
			retval = 1;
			goto END;
		};	

		if ( machinereadable != 0 ) {
			printout("IPV4_SOURCE=6TO4");
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4=%s", helpstring);
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type is 6to4 and included IPv4 address is: %s\n", helpstring);
		};

		/* get registry string */
		retval = libipv4addr_get_registry_string(&ipv4addr, helpstring);
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_REGISTRY=%s", helpstring);
			printout(tempstring);
		} else {
			fprintf(stderr, "IPv4 registry for 6to4 address: %s\n", helpstring);
		};
	};

	if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
		/* extract Teredo client IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) 12 + i) ^ 0xff);
		};

		/* extract Teredo server IPv4 address */
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctett(&ipv4addr2, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) 4 + i));
		};

		if ( machinereadable != 0 ) {
			printout("IPV4_SOURCE=ISATAP");
		};
		print_ipv4addr(&ipv4addr, formatoptions);

		retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, helpstring, 0);
		if ( retval != 0 ) {
			fprintf(stderr, "Error converting IPv4 address to string\n");
			retval = 1;
			goto END;
		};	

		/* extract Teredo client UDP port */
		port = ((unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) 10) << 8 | (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) 11)) ^ 0xffff;

		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "TEREDO_IPV4_SERVER=%s", helpstring);
			printout(tempstring);
			snprintf(tempstring, sizeof(tempstring) - 1, "TEREDO_PORT_CLIENT=%d", port);
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type is Teredo and included IPv4 server address is: %s and client port: %d\n", helpstring, port);
		};

		/* get registry string */
		retval = libipv4addr_get_registry_string(&ipv4addr2, helpstring);
		
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "TEREDO_IPV4_SERVER_REGISTRY=%s", helpstring);
			printout(tempstring);
		} else {
			fprintf(stderr, "IPv4 registry for Teredo server address: %s\n", helpstring);
		};
	};

	/* SLA prefix included? */
	if ( ((typeinfo & ( IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0)) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "SLA=%04x", (unsigned int) ipv6addr_getword(ipv6addrp, 3));
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type has SLA: %04x\n", (unsigned int) ipv6addr_getword(ipv6addrp, 3));
		};
	};
	
	/* IPv6 Registry? */
	if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
		fprintf(stderr, "%s: Check registry: %d\n", DEBUG_function_name, registry);
	};
	if ( registry != -1 ) {
		for ( i = 0; i < (int) (sizeof(ipv6calc_ipv6addrregistry) / sizeof(ipv6calc_ipv6addrregistry[0])); i++ ) {
			if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
				fprintf(stderr, "%s: Registry type check: %d\n", DEBUG_function_name, i);
			};
			if ( ipv6calc_ipv6addrregistry[i].number == registry ) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_REGISTRY=%s", ipv6calc_ipv6addrregistry[i].tokensimple);
					printout(tempstring);
				} else {
					fprintf(stdout, "Registry for address: %s\n", ipv6calc_ipv6addrregistry[i].token);
				};
				break;
			};
		};
	};
	
	/* Proper solicited node link-local multicast address? */
	if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
		if ( (typeinfo & (IPV6_ADDR_LINKLOCAL & IPV6_ADDR_MULTICAST)) != 0 ) {
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
		} else {
			fprintf(stdout, "Address type is compat/mapped and include an IPv4 address\n");
		};
		for (i = 0; i <= 3; i++) {
			ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) (i + 12)));
		};
		print_ipv4addr(&ipv4addr, formatoptions);
	};

	/* Interface identifier included */
	if ( ( ((typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0) ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IID=%04x:%04x:%04x:%04x", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
			printout(tempstring);
		} else {
			fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
		};

		if (ipv6addr_getoctett(ipv6addrp, 11) == 0xff && ipv6addr_getoctett(ipv6addrp, 12) == 0xfe) {
			/* EUI-48 */
			macaddr.addr[0] = ipv6addr_getoctett(ipv6addrp,  8) ^ 0x02;
			macaddr.addr[1] = ipv6addr_getoctett(ipv6addrp,  9);
			macaddr.addr[2] = ipv6addr_getoctett(ipv6addrp, 10);
			macaddr.addr[3] = ipv6addr_getoctett(ipv6addrp, 13);
			macaddr.addr[4] = ipv6addr_getoctett(ipv6addrp, 14);
			macaddr.addr[5] = ipv6addr_getoctett(ipv6addrp, 15);
			print_eui48(&macaddr, formatoptions);
		} else {
			/* Check for global EUI-64 */
			if ( (ipv6addr_getoctett(ipv6addrp, 8) & 0x02) != 0 ) {
				eui64addr.addr[0] = ipv6addr_getoctett(ipv6addrp,  8) ^ 0x02;
				eui64addr.addr[1] = ipv6addr_getoctett(ipv6addrp,  9);
				eui64addr.addr[2] = ipv6addr_getoctett(ipv6addrp, 10);
				eui64addr.addr[3] = ipv6addr_getoctett(ipv6addrp, 11);
				eui64addr.addr[4] = ipv6addr_getoctett(ipv6addrp, 12);
				eui64addr.addr[5] = ipv6addr_getoctett(ipv6addrp, 13);
				eui64addr.addr[6] = ipv6addr_getoctett(ipv6addrp, 14);
				eui64addr.addr[7] = ipv6addr_getoctett(ipv6addrp, 15);
				print_eui64(&eui64addr, formatoptions);
			} else {
				if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring) - 1, "EUI64=??:??:??:??:??:%02x:%02x:%02x", (unsigned int) ipv6addr_getoctett(ipv6addrp, 13), (unsigned int) ipv6addr_getoctett(ipv6addrp, 14), (unsigned int) ipv6addr_getoctett(ipv6addrp, 15));
						printout(tempstring);
					} else {
						fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", (unsigned int) ipv6addr_getoctett(ipv6addrp, 13), (unsigned int) ipv6addr_getoctett(ipv6addrp, 14), (unsigned int) ipv6addr_getoctett(ipv6addrp, 15));
					};
				} else if ( (typeinfo & IPV6_NEW_ADDR_ISATAP) != 0 )  {
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) (i + 12)));
					};

					if ( machinereadable != 0 ) {
						printout("IPV4_SOURCE=ISATAP");
					} else {
						fprintf(stderr, "IPv4 registry for ISATAP client address: %s\n", helpstring);
					};
					print_ipv4addr(&ipv4addr, formatoptions);
				} else if ( ( ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0 && ipv6addr_getword(ipv6addrp, 6) != 0)) )   {
					/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
					if ( machinereadable != 0 ) {
					} else {
						fprintf(stdout, "Address type contains IPv4 address:\n");
					};
					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(ipv6addrp, (unsigned int) (i + 12)));
					};
					if ( machinereadable != 0 ) {
						printout("IPV4_SOURCE=LINK-LOCAL-IID");
					};
					print_ipv4addr(&ipv4addr, formatoptions);
				} else {
					if ( machinereadable != 0 ) {
						if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							printout("EUI64_SCOPE=local-6to4-microsoft");
						} else {
							printout("EUI64_SCOPE=local");
						};
					} else {
						if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							fprintf(stdout, "Interface identifier contain only IPv4 address from 6to4 prefix, usually seen on Microsoft OS\n");
						} else {
							fprintf(stdout, "Interface identifier is probably manual set or based on a local EUI-64 identifier\n");
						};
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
 * shows information about a given IPv4 address
 *
 * in : *ipv4addrp = pointer to IPv4 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_ipv4addr"
int showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions) {
	int retval = 1;

	print_ipv4addr(ipv4addrp, formatoptions);

	printfooter(formatoptions);
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * shows information about a given EUI-48 identifier
 *
 * in : *macaddrp = pointer to MAC address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_eui48"
int showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	int retval = 1;

	print_eui48(macaddrp, formatoptions);
	printfooter(formatoptions);

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
