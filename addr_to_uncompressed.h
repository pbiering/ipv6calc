/*
 * Project    : ipv6calc
 * File       : addr_to_uncompressed.h
 * Version    : $Id: addr_to_uncompressed.h,v 1.2 2002/02/27 23:07:14 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for addr_to_uncompressed.h
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  addr_to_uncompressed(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long command);
extern void addr_to_uncompressed_printhelp(void);
extern void addr_to_uncompressed_printhelplong(void);
