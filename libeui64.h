/*
 * Project    : ipv6calc
 * File       : libeui64.h
 * Version    : $Id: libeui64.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libeu64.c
 */ 

/* prototypes */
extern int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp);
