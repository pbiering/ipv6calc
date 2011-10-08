/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id: showinfo.h,v 1.14 2011/10/08 16:08:33 peter Exp $
 * Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
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
// 3: 0.61.0cvs to 0.70.0
// 4: 0.71.0cvs to current
//	- IP2LOCATION_DATABASE_INFO
//	+ IP2LOCATION_DATABASE_INFO_IPV4
//	+ IP2LOCATION_DATABASE_INFO_IPV6 
// 5: 0.92.0 to current
//	+ IPV6CALC_FEATURES (see ipv6calc -v)
#define IPV6CALC_OUTPUT_VERSION	5

/* prototypes */
extern int  showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t formatoptions);
extern int  showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions);
extern int  showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions);
extern void showinfo_availabletypes(void);
