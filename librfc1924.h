/*
 * Version:     $Id: librfc1924.h,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 *
 * Header file for librfc1924.c libary
 *
 * Author:      Peter Bieringer <pb@bieringer.de>
 *
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int ipv6addrstruct_to_base85(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int base85_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
