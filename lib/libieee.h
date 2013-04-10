/*
 * Project    : ipv6calc
 * File       : libieee.h
 * Version    : $Id: libieee.h,v 1.11 2013/04/10 18:30:52 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libieee.c
 */

#include "ipv6calc_inttypes.h"
#include "libmac.h"
#include "libeui64.h"


#ifndef _libieee_h

#define _libieee_h

/* Structure */
typedef struct {
	const uint32_t id;
	const char *string_owner;
	const char *shortstring_owner;
} s_ieee_oui;

typedef struct {
	const uint32_t id;
	const uint32_t subid_begin;
	const uint32_t subid_end;
	const char *string_owner;
	const char *shortstring_owner;
} s_ieee_oui36;

typedef struct {
	const uint32_t id;
	const uint32_t subid_begin;
	const uint32_t subid_end;
	const char *string_owner;
	const char *shortstring_owner;
} s_ieee_iab;

typedef struct {
	const uint32_t bits_00_23;
	const uint32_t mapping;
} s_ieee_mapping;


// internal map for IAB / OUI-36 lists
static const s_ieee_mapping ieee_mapping[] = {
	// IAB
	{ 0x0050C2,	0x1 },
	{ 0x40D855,	0x2 },
	// OUI36
	{ 0x001BC5,	0x9 }
};

#endif

extern int libieee_get_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp);
extern int libieee_get_short_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp);

extern uint32_t libieee_map_oui_macaddr(const ipv6calc_macaddr *macaddrp);
extern uint32_t libieee_map_oui_eui64addr(const ipv6calc_eui64addr *eui64addrp);
extern int libieee_unmap_oui_macaddr(ipv6calc_macaddr *macaddrp, uint32_t map_value);
extern int libieee_unmap_oui_eui64addr(ipv6calc_eui64addr *eui64addrp, uint32_t map_value);
