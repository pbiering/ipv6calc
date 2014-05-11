/*
 * Project    : ipv6calc
 * File       : libmac.h
 * Version    : $Id: libmac.h,v 1.14 2014/05/11 09:49:38 ds6peter Exp $
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libmac.c
 */ 


#include "ipv6calc_inttypes.h"


#ifndef _libmac_h

#define _libmac_h 1

/* typedefs */

typedef struct {
	uint8_t addr[6];	/* MAC address in 6x 8 bit */
	int flag_valid;		/* address structure filled */
} ipv6calc_macaddr;


typedef struct {
	int active;
	uint32_t typeinfo_must_have;
	/* others coming next */
} s_ipv6calc_filter_macaddr;

#define EUI48_00_23(x)  ((x[0] << 16) | (x[1] << 8) | x[2])
#define EUI48_24_47(x)  ((x[3] << 16) | (x[4] << 8) | x[5])

#endif

/* prototypes */
extern int mac_to_macaddrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_macaddr *macaddrp);
extern int addr_to_macaddrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_macaddr *macaddrp); // compatible name
extern int macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int libmacaddr_macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions); // compatible name
extern void mac_clearall(ipv6calc_macaddr *macaddrp);

extern int macaddr_filter(const ipv6calc_macaddr *macaddrp, const s_ipv6calc_filter_macaddr *filter);
extern void macaddr_filter_clear(s_ipv6calc_filter_macaddr *filter);

extern void libmacaddr_anonymize(ipv6calc_macaddr *macaddrp, const s_ipv6calc_anon_set *ipv6calc_anon_set_p);
