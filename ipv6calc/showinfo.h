/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.3 2002/03/24 17:00:39 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libmac.h"

#define DEBUG_showinfo		0x2

/* prototypes */
extern int  showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, const unsigned long formatoptions);
extern int  showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const unsigned long formatoptions);
extern int  showinfo_eui48(const ipv6calc_macaddr *macaddrp, const unsigned long formatoptions);
extern void showinfo_availabletypes(void);
