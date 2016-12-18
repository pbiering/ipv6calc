/*
 * Project    : ipv6calc
 * File       : libeui64.h
 * Version    : $Id$
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libeu64.c
 */ 

#include "libipv6addr.h"
#include "libmac.h"

#ifndef _libeui64_h

#define _libeui64_h

typedef struct {
	uint8_t addr[8];        /* EUI-64 address in 8x 8 bit */
	int flag_valid;         /* address structure filled */
} ipv6calc_eui64addr;


#define EUI64_00_23(x)	((x[0] << 16) | (x[1] << 8) | x[2])
#define EUI64_24_47(x)	((x[3] << 16) | (x[4] << 8) | x[5])
#define EUI64_48_63(x)	(               (x[6] << 8) | x[7])

#define EUI64_00_31(x)	((x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3])
#define EUI64_32_63(x)	((x[4] << 24) | (x[5] << 16) | (x[6] << 8) | x[7])


#endif

/* prototypes */
extern int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp);
extern int libeui64_addr_to_eui64addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_eui64addr *eui64addrp);
extern void libeui64_clear(ipv6calc_eui64addr *eui64addrp);
extern void libeui64_clearall(ipv6calc_eui64addr *eui64addrp);

extern void libeui64_anonymize(ipv6calc_eui64addr *eui64addrp, const s_ipv6calc_anon_set *ipv6calc_anon_set_p);
extern int libeui64_eui64addrstruct_to_string(const ipv6calc_eui64addr *eui64addr_p, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
