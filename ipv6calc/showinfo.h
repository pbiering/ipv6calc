/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.8 2002/04/21 11:23:23 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libmac.h"

#define DEBUG_showinfo		0x2

/* prototypes */
extern int  showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t formatoptions);
extern int  showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions);
extern int  showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions);
extern void showinfo_availabletypes(void);
