/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.9 2012/03/18 17:15:41 peter Exp $
 * Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "ipv6calc_inttypes.h"


/* typedefs */

#ifndef _libipv6calc_h

#define _libipv6calc_h 1

/* text representations for defines */
typedef struct {
	const uint32_t number;
	const char *token;
} s_type;

#endif


/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string);

extern uint32_t libipv6calc_autodetectinput(const char *string);


