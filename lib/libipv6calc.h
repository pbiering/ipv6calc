/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.3 2002/04/05 19:11:13 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#if defined(__NetBSD__)
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#define NI_MAXHOST	1024

/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);
extern void string_to_reverse(char *string);
extern uint32_t libipv6calc_autodetectinput(const char *string);
