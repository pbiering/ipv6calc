/*
 * Project    : ipv6calc
 * File       : ipv4_to_6to4addr.h
 * Version    : $Id: ipv4_to_6to4addr.h,v 1.1 2002/02/27 23:07:14 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv4_to_6to4addr.c
 */ 

/* prototypes */
extern int  ipv4_to_6to4addr(char *addrstring, char *resultstring);
extern void ipv4_to_6to4addr_printhelp(void);
extern void ipv4_to_6to4addr_printhelplong(void);
