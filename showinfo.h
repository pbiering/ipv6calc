/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.1 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

/* prototypes */
extern int  showinfo(ipv6calc_ipv6addr *ipv6addrp, unsigned long command);
extern void showinfo_printhelp(void);
extern void showinfo_printhelplong(void);
