/*
 * Project    : ipv6calc
 * File       : librfc1924.h
 * Version    : $Id: librfc1924.h,v 1.2 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1924.c libary
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int ipv6addrstruct_to_base85(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int base85_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
