/*
 * Project    : ipv6calc
 * File       : libieee.h
 * Version    : $Id: libieee.h,v 1.4 2002/04/20 17:11:29 peter Exp $
 *
 * Information:
 *  Header file for libieee.c
 */

#if defined(__NetBSD__) || defined (__OpenBSD__) || defined (__FreeBSD__)
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#ifndef _libieee_h

#define _libieee_h

/* Structure */
typedef struct {
	const uint32_t id;
	const char *string_owner;
	const char *shortstring_owner;
} s_ieee_oui;

#endif

/* extern static const s_ieee_oui libieee_oui[];*/

extern int libieee_get_vendor_string(char *resultstring, const uint8_t o1, const uint8_t o2, const uint8_t o3);
extern int libieee_get_short_vendor_string(char *resultstring, const uint8_t, const uint8_t o2, const uint8_t o3);
