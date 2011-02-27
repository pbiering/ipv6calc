/*
 * Project    : ipv6calc
 * File       : libmac.h
 * Version    : $Id: libmac.h,v 1.7 2011/02/27 11:38:43 peter Exp $
 * Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libmac.c
 */ 


#include "ipv6calc_inttypes.h"


#ifndef _libmac_h

#define _libmac_h

/* typedefs */

typedef struct {
	uint8_t addr[6];	/* MAC address in 6x 8 bit */
	int flag_valid;		/* address structure filled */
} ipv6calc_macaddr;

#endif

/* prototypes */
extern int mac_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp);
extern int macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const uint32_t formatoptions);
extern void mac_clearall(ipv6calc_macaddr *macaddrp);
