/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.11 2007/02/15 14:53:33 peter Exp $
 * Copyright  : 2001-2006 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libmac.h"

#define DEBUG_showinfo		0x2

/* output version of machine readable format */
// 1: never defined, until 0.51
// 2: 0.60.0cvs to 0.60.1
// 3: 0.61.0cvs to current
#define IPV6CALC_OUTPUT_VERSION	3

/* prototypes */
#ifndef SUPPORT_IP2LOCATION
// TODO: analyse reason (happen with at least 2.1.2)
extern int  showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t formatoptions);
extern int  showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions);
extern int  showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions);
#endif
extern void showinfo_availabletypes(void);
