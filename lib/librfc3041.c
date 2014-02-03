/*
 * Project    : ipv6calc
 * File       : librfc3041.c
 * Version    : $Id: librfc3041.c,v 1.15 2014/02/03 20:48:04 ds6peter Exp $
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for host identifier privacy extension defined in RFC 3041 / RFC 4941
 *
 * Credits to YOSHIFUJI Hideaki <yoshfuji at linux-ipv6 dot org>
 *  for hints and sample code in Perl:
 *  % perl -e '@a=(0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef); foreach (@a){ print pack("C",$_); } foreach (@a){ print pack("C",$_); }' | md5sum 
 *  4662bdea8654776d486072ff7074945e
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

#ifdef ENABLE_BUNDLED_MD5
#include "../md5/md5.h"
#else
#include <openssl/md5.h>
#endif

#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "librfc3041.h"
#include "libipv6addr.h"


/* function 
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken) {
	int retval = 1, i;
	char tempstring[NI_MAXHOST],  tempstring2[NI_MAXHOST];
	unsigned char digest[MD5_DIGEST_LENGTH];

#ifdef ENABLE_BUNDLED_MD5
	struct md5_ctx md5hash;
#else
	MD5_CTX md5hash;
#endif

	DEBUGPRINT_WA(DEBUG_librfc3041, "Got identifier '%08x-%08x' and token '%08x-%08x'", (unsigned int) ipv6addr_getdword(identifier, 2), (unsigned int) ipv6addr_getdword(identifier, 3), (unsigned int) ipv6addr_getdword(token, 2), (unsigned int) ipv6addr_getdword(token, 3));

#ifdef ENABLE_BUNDLED_MD5
	md5_init_ctx(&md5hash);
#else
	MD5_Init(&md5hash);
#endif

#ifdef ENABLE_BUNDLED_MD5
	md5_process_bytes(&identifier->in6_addr.s6_addr[8], 8, &md5hash);
	md5_process_bytes(&token->in6_addr.s6_addr[8], 8, &md5hash);
#else
	MD5_Update(&md5hash, &identifier->in6_addr.s6_addr[8], 8);
	MD5_Update(&md5hash, &token->in6_addr.s6_addr[8], 8);
#endif

#ifdef ENABLE_BUNDLED_MD5
	md5_finish_ctx(&md5hash, digest);
#else
	MD5_Final(digest, &md5hash);
#endif

	tempstring[0] = '\0';

	for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s%02x", tempstring, (int) digest[i]);
		snprintf(tempstring, sizeof(tempstring) - 1, "%s", tempstring2);
	};
	
	DEBUGPRINT_WA(DEBUG_librfc3041, "MD5 hash '%s'", tempstring);

	ipv6addr_clear(newidentifier);
	ipv6addr_clear(newtoken);
	
	for (i = 0; i < 8; i++) {
		/* copy into */	
		newidentifier->in6_addr.s6_addr[i + 8] = (uint8_t) digest[i];
		newtoken->in6_addr.s6_addr[i + 8] = (uint8_t) digest[i + 8];
	};

	newidentifier->flag_valid = 1;
	newidentifier->prefixlength = 64;
	newtoken->flag_valid = 1;

	/* clear universal/local bit */
	ipv6addr_setoctet(newidentifier, 8, ipv6addr_getoctet(newidentifier, 8) & 0xfd);

   	retval = 0;	
	return (retval);
};

