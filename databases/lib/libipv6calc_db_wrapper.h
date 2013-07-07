/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.h
 * Version    : $Id: libipv6calc_db_wrapper.h,v 1.6 2013/07/07 20:21:14 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper.c
 */

#ifndef _libipv6calc_db_wrapper_h

#define _libipv6calc_db_wrapper_h 1

#include "libipv6calc_db_wrapper_GeoIP.h"

// define internal API versions
#define IPV6CALC_DB_API_GEOIP		1
#define IPV6CALC_DB_API_IP2LOCATION	1
#define IPV6CALC_DB_API_IEEE		1
#define IPV6CALC_DB_API_REGISTRIES	1


#define IPV6CALC_DL_STATUS_OK		1
#define IPV6CALC_DL_STATUS_UNKNOWN	0
#define IPV6CALC_DL_STATUS_ERROR	-1


#endif


extern int libipv6calc_db_wrapper_init(void);
extern void libipv6calc_db_wrapper_info(char * string, const size_t size);
extern void libipv6calc_db_wrapper_print_db_info(const int level_verbose);

/* functional wrappers */
const char * libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto);
char * libipv6calc_db_wrapper_asnum_by_addr(const char *addr, const int proto);
