/*
 * Project    : ipv6calc
 * File       : librfc5569.h
 * Version    : $Id: librfc5569.h,v 1.1 2011/09/16 18:05:13 peter Exp $
 * Copyright  : 2011 by Raphaël Assénat <raph (at) raphnet.net>
 *
 * Information:
 *  Header file for librfc5569.c
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"

int librfc5569_calc_6rd_local_prefix(ipv6calc_ipv6addr *sixrd_prefix, const ipv6calc_ipv4addr *relay_prefix, const ipv6calc_ipv4addr *local_ip, char *resultstring);
