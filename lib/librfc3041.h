/*
 * Project    : ipv6calc
 * File       : librfc3041.h
 * Version    : $Id: librfc3041.h,v 1.4 2012/05/09 17:08:10 peter Exp $
 * Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc3041.c (RFC 3041 / RFC 4941)
 */ 

#include "libipv6addr.h"

/* defines */

#ifndef MD5_DIGEST_LENGTH
#define MD5_DIGEST_LENGTH 16
#endif

#ifndef _librfc3041_h

#define _librfc3041_h

#define RFC3041_ANON_IID_00_31	(uint32_t) 0xa0fc4941u		// RFC 4941
#define RFC3041_ANON_IID_32_63	(uint32_t) 0xa0fc3041u		// RFC 3041

#endif



/* prototypes */
extern int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken);
