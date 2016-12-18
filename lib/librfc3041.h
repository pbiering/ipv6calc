/*
 * Project    : ipv6calc
 * File       : librfc3041.h
 * Version    : $Id$
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

#endif



/* prototypes */
extern int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken);
