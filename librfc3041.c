/*
 * Project    : ipv6calc
 * File       : librfc3041.c
 * Version    : $Id: librfc3041.c,v 1.2 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for host identifier privacy extension defined in RFC 3041
 *
 * Credits to YOSHIFUJI Hideaki <yoshfuji at linux-ipv6 dot org>
 *  for hints and sample code in Perl:
 *  % perl -e '@a=(0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef); foreach (@a){ print pack("C",$_); } foreach (@a){ print pack("C",$_); }' | md5sum 
 *  4662bdea8654776d486072ff7074945e
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include "ipv6calc.h"
#include "librfc3041.h"
#include "libipv6addr.h"

/* function 
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */


int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken) {
#define DEBUG_function_name "librfc3041/librfc3041_calc"
	int retval = 1, i;
	MD5_CTX md5hash;
	char tempstring[NI_MAXHOST],  tempstring2[NI_MAXHOST];
	unsigned char digest[MD5_DIGEST_LENGTH];

	if (ipv6calc_debug & DEBUG_librfc3041) {
		fprintf(stderr, "%s: Got identifier '%08x-%08x' and token '%08x-%08x'\n", DEBUG_function_name, ipv6addr_getdword(identifier, 2), ipv6addr_getdword(identifier, 3), ipv6addr_getdword(token, 2), ipv6addr_getdword(token, 3)); 
	};

	MD5_Init(&md5hash);

	MD5_Update(&md5hash, &identifier->in6_addr.s6_addr[8], 8);
	MD5_Update(&md5hash, &token->in6_addr.s6_addr[8], 8);

	MD5_Final(digest, &md5hash);

	sprintf(tempstring, "%s", "");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(tempstring2, "%s%02x", tempstring, digest[i]);
		strcpy(tempstring, tempstring2);
	};
	
	if (ipv6calc_debug & DEBUG_librfc3041) {
		fprintf(stderr, "%s: MD5 hash '%s'\n", DEBUG_function_name, tempstring);
	};

	ipv6addr_clear(newidentifier);
	ipv6addr_clear(newtoken);
	
	for (i = 0; i <= 7; i++) {
		/* copy into */	
		newidentifier->in6_addr.s6_addr[i + 8] = digest[i];
		newtoken->in6_addr.s6_addr[i + 8] = digest[i + 8];
	};

   	retval = 0;	
	return (retval);
};

