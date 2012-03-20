/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.10 2012/03/20 06:36:30 peter Exp $
 * Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"


/* typedefs */

#ifndef _libipv6calc_h

#define _libipv6calc_h 1

/* master filter structure */
typedef struct {
	s_ipv6calc_filter_ipv4addr filter_ipv4addr;
	s_ipv6calc_filter_ipv6addr filter_ipv6addr;
	s_ipv6calc_filter_macaddr  filter_macaddr;
} s_ipv6calc_filter_master;

#endif


/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string);

extern uint32_t libipv6calc_autodetectinput(const char *string);

int libipv6calc_filter_parse(const char *expression, s_ipv6calc_filter_master *filter_master);
void libipv6calc_filter_clear(s_ipv6calc_filter_master *filter_master);


