/*
 * Project    : ipv6calc
 * File       : librfc3041.h
 * Version    : $Id: librfc3041.h,v 1.2 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc3041.c libary
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken);
