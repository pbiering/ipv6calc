/*
 * Project    : ipv6calc
 * File       : base85_to_addr.h
 * Version    : $Id: base85_to_addr.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for base85_to_addr.h
 */ 

/* prototypes */
extern int  base85_to_addr(char *addrstring, char *resultstring);
extern void base85_to_addr_printhelp(void);
extern void base85_to_addr_printhelplong(void);
