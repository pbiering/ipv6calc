/*
 * Project    : ipv6calc
 * File       : libieee.h
 * Version    : $Id: libieee.h,v 1.2 2002/03/16 19:40:29 peter Exp $
 *
 * Information:
 *  Header file for libieee.c
 */

#ifndef _libieee_h

#define _libieee_h

/* Structure */
typedef struct {
	const unsigned int id;
	const char *string_owner;
	const char *shortstring_owner;
} s_ieee_oui;

#endif

/* extern static const s_ieee_oui libieee_oui[];*/

extern int libieee_get_vendor_string(char *resultstring, const unsigned short o1,  const unsigned short o2,  const unsigned short o3);
extern int libieee_get_short_vendor_string(char *resultstring, const unsigned short o1,  const unsigned short o2,  const unsigned short o3);
