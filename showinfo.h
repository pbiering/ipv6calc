/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.5 2002/03/03 18:21:34 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"

/* prototypes */
extern int  showinfo_ipv6addr(ipv6calc_ipv6addr *ipv6addrp, unsigned long formatoptions);
extern int showinfo_ipv4addr(ipv6calc_ipv4addr *ipv4addrp, unsigned long formatoptions);
extern void showinfo_availabletypes(void);
