/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.2 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  addr_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  ipv6addrstruct_to_uncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int  ipv6addrstruct_to_uncompaddrprefix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int  ipv6addrstruct_to_uncompaddrsuffix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  ipv6addrstruct_to_fulluncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern void string_to_upcase(char *string);
