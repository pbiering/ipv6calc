/*
 * Project    : ipv6calc
 * File       : eui_to_privacy.c
 * Version    : $Id: eui64_to_privacy.c,v 1.2 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  A function to calculate a new host identifier for privacy
 *   defined in (RFC 3041)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipv6calc.h"
#include "librfc3041.h"
#include "libipv6addr.h"
#include "libipv6calc.h"
#include "libeui64.h"
#include "eui64_to_privacy.h"

void eui64_to_privacy_printhelp(void) {
	fprintf(stderr, " %s --eui64_to_privacy <eui64 token>\n", PROGRAM_NAME);
};

void eui64_to_privacy_printhelplong(void) {
	eui64_to_privacy_printhelp();
	fprintf(stderr, "  Calculate a new host identifier from a given one and a token (RFC 3041)\n");
	fprintf(stderr, "   also the next token (for history) is displayed\n");
	fprintf(stderr, "   --eui64_to_privacy 0123456789abcdef 0123456789abcdef -> 4462bdea8654776d 486072ff7074945e\n");
};

/* function which calculates a new identifier and token from a given set
 *  
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int eui64_to_privacy(char *hostidentifier, char *token, char *resultstring) {
#define DEBUG_function_name "eui64_to_privacy" 
	int retval = 1, result = 0, i;
    char tempstring[NI_MAXHOST],  tempstring2[NI_MAXHOST], tempstring3[NI_MAXHOST];
	ipv6calc_ipv6addr ipv6addr_identifier, ipv6addr_token, ipv6addr_newidentifier, ipv6addr_newtoken;

	if (ipv6calc_debug & DEBUG_eui64_to_privacy) {
		fprintf(stderr, "%s: Got hostidentifier '%s' and token '%s'\n", DEBUG_function_name, hostidentifier, token);
	};

	result = identifier_to_ipv6addrstruct(hostidentifier, resultstring, &ipv6addr_identifier);
	result = identifier_to_ipv6addrstruct(token, resultstring, &ipv6addr_token);

	result = librfc3041_calc(&ipv6addr_identifier, &ipv6addr_token, &ipv6addr_newidentifier, &ipv6addr_newtoken);

	/* clear bit 6 of new identifier "local scope" */
	ipv6addr_newidentifier.in6_addr.s6_addr[8] &= 0xfd;

    sprintf(tempstring, "%s", "");
    for (i = 0; i < 4; i++) {
        sprintf(tempstring2, "%s%04x", tempstring, ipv6addr_getword(&ipv6addr_newidentifier, i+4));
        strcpy(tempstring, tempstring2);
    };    
	
	if (ipv6calc_debug & DEBUG_eui64_to_privacy) {
		fprintf(stderr, "%s: New host identifier '%s'\n", DEBUG_function_name, tempstring);
	};

    sprintf(tempstring3, "%s", "");
    for (i = 0; i < 4; i++) {
        sprintf(tempstring2, "%s%04x", tempstring3, ipv6addr_getword(&ipv6addr_newtoken, i+4));
        strcpy(tempstring3, tempstring2);
    };    

	if (ipv6calc_debug & DEBUG_eui64_to_privacy) {
		fprintf(stderr, "%s: New token '%s'\n", DEBUG_function_name, tempstring3);
	};
	
    sprintf(resultstring, "%s %s", tempstring, tempstring3);
	
	retval = 0;
	return (retval);
};
