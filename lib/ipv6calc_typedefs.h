/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id$
 * Copyright  : 2021-2021 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GPLv2
 *
 * Information:
 *  Header file for all the generic typedefs (migration work-in-progress)
 */ 

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"


/* defines */
#ifndef _ipv6calc_typedefs_h_

#define _ipv6calc_typedefs_h_

/* master filter structure */
typedef struct {
	s_ipv6calc_filter_ipv4addr filter_ipv4addr;
	s_ipv6calc_filter_ipv6addr filter_ipv6addr;
	s_ipv6calc_filter_macaddr  filter_macaddr;
} s_ipv6calc_filter_master;


#endif // _ipv6calc_typedefs_h_
