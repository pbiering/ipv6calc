/*
 * Version:     $Id: librfc3041.h,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 *
 * Header file for librfc3041.c libary
 *
 * Author:      Peter Bieringer <pb@bieringer.de>
 *
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int librfc3041_calc(ipv6calc_ipv6addr *identifier, ipv6calc_ipv6addr *token, ipv6calc_ipv6addr *newidentifier, ipv6calc_ipv6addr *newtoken);
