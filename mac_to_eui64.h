/*
 * Project    : ipv6calc
 * File       : mac_to_eui64.h
 * Version    : $Id: mac_to_eui64.h,v 1.1 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for mac_to_eui64.c
 */ 

/* prototypes */
extern int  mac_to_eui64(char *addrstring, char *resultstring);
extern void mac_to_eui64_printhelp(void);
extern void mac_to_eui64_printhelplong(void);
