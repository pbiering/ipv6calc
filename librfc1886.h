/*
 * Project    : ipv6calc
 * File       : librfc1886.h
 * Version    : $Id: librfc1886.h,v 1.1 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1886.c
 */ 

/* prototypes */
extern int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
