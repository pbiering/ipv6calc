/*
 * Project    : ipv6calc
 * File       : librfc1924.h
 * Version    : $Id: librfc1924.h,v 1.2 2014/05/11 09:49:38 ds6peter Exp $
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1924.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int ipv6addrstruct_to_base85(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);
extern int base85_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);
extern int librfc1924_formatcheck(const char *string, char *infostring, const size_t infostring_length);
