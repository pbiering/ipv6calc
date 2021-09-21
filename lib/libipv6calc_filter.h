/*
 * Project    : ipv6calc
 * File       : libipv6calc_filter.h
 * Version    : $Id$
 * Copyright  : 2021-2021 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GPLv2
 *
 * Information:
 *  Header file for libipv6calc filter implementation
 */ 

#ifndef _libipv6calc_filter_h_

#define _libipv6calc_filter_h_


#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"


/* master filter structure */
typedef struct {
	s_ipv6calc_filter_ipv4addr filter_ipv4addr;
	s_ipv6calc_filter_ipv6addr filter_ipv6addr;
	s_ipv6calc_filter_macaddr  filter_macaddr;
} s_ipv6calc_filter_master;


#endif // _ipv6calc_typedefs_h_


extern int  libipv6calc_filter_parse(const char *expression, s_ipv6calc_filter_master *filter_master);
extern int  libipv6calc_filter_check(s_ipv6calc_filter_master *filter_master);
extern void libipv6calc_filter_clear(s_ipv6calc_filter_master *filter_master);
extern void libipv6calc_filter_clear_db_cc(s_ipv6calc_filter_db_cc *filter_db_cc);
extern void libipv6calc_filter_clear_db_asn(s_ipv6calc_filter_db_asn *filter_db_asn);
extern void libipv6calc_filter_clear_db_registry(s_ipv6calc_filter_db_registry *filter_db_registry);
