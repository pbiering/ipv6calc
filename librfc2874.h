/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.3 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

/* prototypes */
extern int librfc2874_addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
