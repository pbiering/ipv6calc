/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.7 2002/04/21 11:24:08 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "ipv6calc_inttypes.h"


/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string);

extern uint32_t libipv6calc_autodetectinput(const char *string);
