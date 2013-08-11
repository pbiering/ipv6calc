/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.h
 * Version    : $Id: libipv6calc_db_wrapper.h,v 1.8 2013/08/11 16:42:11 ds6peter Exp $
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

// AS Number handling
#define ASNUM_AS_UNKNOWN 0
#define ASNUM_AS_TRANS   23456  // special 16-bit AS number for compatibility

// CountryCode handling
#define COUNTRYCODE_LETTER1_MAX          26     // A-Z
#define COUNTRYCODE_LETTER2_MAX          36     // 0-9A-Z
#define COUNTRYCODE_INDEX_LETTER_MAX     (COUNTRYCODE_LETTER1_MAX * COUNTRYCODE_LETTER2_MAX - 1)
#define COUNTRYCODE_INDEX_MAX            1023
#define COUNTRYCODE_INDEX_UNKNOWN        1022

// macros for mapping index to chars and vice-versa
#define COUNTRYCODE_INDEX_TO_CHAR1(index)  ((index % COUNTRYCODE_LETTER1_MAX) + 'A')
#define COUNTRYCODE_INDEX_TO_CHAR2(index)  ((index / COUNTRYCODE_LETTER1_MAX) > 9) ? ((index / COUNTRYCODE_LETTER1_MAX) - 10 + 'A') : ((index / COUNTRYCODE_LETTER1_MAX) + '0')


#endif


extern int libipv6calc_db_wrapper_init(void);
extern void libipv6calc_db_wrapper_info(char *string, const size_t size);
extern void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string);

/* functional wrappers */
// CountryCode Text/Number
extern const char *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto);

// Autonomous System Text/Number
extern char       *libipv6calc_db_wrapper_as_text_by_addr(const char *addr, const int proto);
extern uint32_t    libipv6calc_db_wrapper_as_num32_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_as_num16_by_addr(const char *addr, const int proto);

extern uint32_t    libipv6calc_db_wrapper_as_num32_comp17(const uint32_t as_num32);
extern uint32_t    libipv6calc_db_wrapper_as_num32_decomp17(const uint32_t as_num32_comp17);

// Registries
extern int         libipv6calc_db_wrapper_registry_num_by_as_num32(const uint32_t as_num32);
