/*
 * Project    : ipv6calc
 * File       : librfc3041.h
 * Version    : $Id: librfc3041.h,v 1.2 2004/10/30 12:35:52 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc3041.c
 */ 

#include "libipv6addr.h"

/* defines */

#ifndef MD5_DIGEST_LENGTH
#define MD5_DIGEST_LENGTH 16
#endif

/* prototypes */
extern int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken);
