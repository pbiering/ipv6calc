/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.4 2002/03/03 14:54:03 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  showinfo_ipv6addr(ipv6calc_ipv6addr *ipv6addrp, unsigned long formatoptions);
extern void showinfo_availabletypes(void);
extern void showinfo_printhelp(void);
extern void showinfo_printhelplong(void);
