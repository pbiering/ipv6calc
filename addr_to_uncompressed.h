/*
 * Project    : ipv6calc
 * File       : addr_to_uncompressed.h
 * Version    : $Id: addr_to_uncompressed.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for addr_to_uncompressed.h
 */ 

/* prototypes */
extern int  addr_to_uncompressed(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long command);
extern void addr_to_uncompressed_printhelp(void);
extern void addr_to_uncompressed_printhelplong(void);
