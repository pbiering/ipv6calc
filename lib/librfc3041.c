/*
 * Project    : ipv6calc
 * File       : librfc3041.c
 * Version    : $Id$
 * Copyright  : 2001-2021 by Peter Bieringer <pb (at) bieringer.de>
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
#ifdef ENABLE_OPENSSL_EVP_MD5
#include <openssl/evp.h>
#else
#include <openssl/md5.h>
#endif
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
	int retval = 1;
	unsigned int i;
	char tempstring[IPV6CALC_STRING_MAX],  tempstring2[IPV6CALC_STRING_MAX];

	DEBUGPRINT_WA(DEBUG_librfc3041, "Got identifier '%08x-%08x' and token '%08x-%08x'", (unsigned int) ipv6addr_getdword(identifier, 2), (unsigned int) ipv6addr_getdword(identifier, 3), (unsigned int) ipv6addr_getdword(token, 2), (unsigned int) ipv6addr_getdword(token, 3));

#ifdef ENABLE_OPENSSL_EVP_MD5

#ifdef HAVE_EVP_MD_CTX_NEW
	unsigned int digest_len; // will be set by EVP_DigestFinal_ex
	unsigned char digest[EVP_MAX_MD_SIZE];
	EVP_MD_CTX *md5hash = EVP_MD_CTX_new();

	EVP_DigestInit_ex(md5hash, EVP_md5(), NULL);
	EVP_DigestUpdate(md5hash, &identifier->in6_addr.s6_addr[8], 8);
	EVP_DigestUpdate(md5hash, &token->in6_addr.s6_addr[8], 8);
	EVP_DigestFinal_ex(md5hash, digest, &digest_len);
	EVP_MD_CTX_free(md5hash);
#else // HAVE_EVP_MD_CTX_NEW

#ifdef HAVE_EVP_MD_CTX_INIT
	unsigned int digest_len; // will be set by EVP_DigestFinal_ex
	unsigned char digest[EVP_MAX_MD_SIZE];
	EVP_MD_CTX md5hash;
	EVP_MD_CTX_init(&md5hash);

	EVP_DigestInit_ex(&md5hash, EVP_md5(), NULL);
	EVP_DigestUpdate(&md5hash, &identifier->in6_addr.s6_addr[8], 8);
	EVP_DigestUpdate(&md5hash, &token->in6_addr.s6_addr[8], 8);
	EVP_DigestFinal_ex(&md5hash, digest, &digest_len);
#else // HAVE_EVP_MD_CTX_INIT

#error No supported OpenSSL EVP MD5 method defined

#endif // HAVE_EVP_MD_CTX_INIT

#endif // HAVE_EVP_MD_CTX_NEW

#else // ENABLE_OPENSSL_EVP_MD5

#ifdef ENABLE_OPENSSL_MD5
	unsigned int digest_len = MD5_DIGEST_LENGTH;
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5_CTX md5hash;

	MD5_Init(&md5hash);
	MD5_Update(&md5hash, &identifier->in6_addr.s6_addr[8], 8);
	MD5_Update(&md5hash, &token->in6_addr.s6_addr[8], 8);
	MD5_Final(digest, &md5hash);
#else // ENABLE_OPENSSL_MD5
	// fallback to bundled MD5
	unsigned int digest_len = MD5_DIGEST_LENGTH;
	unsigned char digest[MD5_DIGEST_LENGTH];
	struct md5_ctx md5hash;

	md5_init_ctx(&md5hash);
	md5_process_bytes(&identifier->in6_addr.s6_addr[8], 8, &md5hash);
	md5_process_bytes(&token->in6_addr.s6_addr[8], 8, &md5hash);
	md5_finish_ctx(&md5hash, digest);
#endif // ENABLE_OPENSSL_MD5

#endif // ENABLE_OPENSSL_EVP_MD5

	tempstring[0] = '\0';

	for (i = 0; i < digest_len; i++) {
		snprintf(tempstring2, sizeof(tempstring2), "%s%02x", tempstring, (int) digest[i]);
		snprintf(tempstring, sizeof(tempstring), "%s", tempstring2);
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

