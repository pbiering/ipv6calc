/*
 * Project    : ipv6calc
 * File       : addr_to_base85.h
 * Version    : $Id: addr_to_base85.h,v 1.1 2002/02/25 21:18:49 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for addr_to_base85.c
 */ 

/* prototypes */
extern int  addr_to_base85(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern void addr_to_base85_printhelp(void);
extern void addr_to_base85_printhelplong(void);
