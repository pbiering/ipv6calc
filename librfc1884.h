/*
 * Project    : ipv6calc
 * File       : librfc1884.h
 * Version    : $Id: librfc1884.h,v 1.3 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1884.c
 */ 

/* prototypes */
extern int compaddr_to_uncompaddr(char *addrstring, char *resultstring);
extern int ipv6addrstruct_to_compaddr(ipv6calc_ipv6addr *ipv6addr, char *resultstring);
