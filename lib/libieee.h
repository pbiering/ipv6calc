/*
 * Project    : ipv6calc
 * File       : libieee.h
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de>
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
} s_ieee_oui28;

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
	const uint8_t type;
	const uint32_t mapping;
} s_ieee_mapping;

#define IEEE_IAB	0x1
#define IEEE_OUI36	0x2
#define IEEE_OUI28	0x3

#endif

extern uint32_t libieee_map_oui_macaddr(const ipv6calc_macaddr *macaddrp);
extern uint32_t libieee_map_oui_eui64addr(const ipv6calc_eui64addr *eui64addrp);
extern int libieee_unmap_oui_macaddr(ipv6calc_macaddr *macaddrp, uint32_t map_value);
extern int libieee_unmap_oui_eui64addr(ipv6calc_eui64addr *eui64addrp, uint32_t map_value);

extern int libieee_check_oui36_iab(const uint32_t bits_00_23);
extern int libieee_check_oui28(const uint32_t bits_00_23);
