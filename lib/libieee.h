/*
 * Project    : ipv6calc
 * File       : libieee.h
 * Version    : $Id: libieee.h,v 1.9 2013/04/09 20:20:43 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libieee.c
 */

#include "ipv6calc_inttypes.h"
#include "libmac.h"


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

#endif

/* extern static const s_ieee_oui libieee_oui[];*/

extern int libieee_get_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp);
extern int libieee_get_short_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp);
