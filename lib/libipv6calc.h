/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.2 2002/04/04 19:40:27 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include <stdint.h>
#define NI_MAXHOST	1024

/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);
extern void string_to_reverse(char *string);
extern uint32_t libipv6calc_autodetectinput(const char *string);
