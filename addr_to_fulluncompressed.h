/*
 * Project    : ipv6calc
 * File       : addr_to_fulluncompressed.h
 * Version    : $Id: addr_to_fulluncompressed.h,v 1.2 2002/03/01 23:26:45 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file addr_to_fulluncompressed.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  addr_to_fulluncompressed(ipv6calc_ipv6addr *ipv6addr, char *resultstring, unsigned long command);
extern void addr_to_fulluncompressed_printhelp(void);
extern void addr_to_fulluncompressed_printhelplong(void);
