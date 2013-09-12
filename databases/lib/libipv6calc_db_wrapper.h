/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.h
 * Version    : $Id: libipv6calc_db_wrapper.h,v 1.13 2013/09/12 20:40:40 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper.c
 */

#ifndef _libipv6calc_db_wrapper_h

#define _libipv6calc_db_wrapper_h 1

#include "ipv6calctypes.h"

extern uint32_t wrapper_features;

// define features
#define IPV6CALC_DB_AS_TO_REGISTRY		0x00000001
#define IPV6CALC_DB_IPV4_TO_REGISTRY		0x00000002
#define IPV6CALC_DB_IPV6_TO_REGISTRY		0x00000004

#define IPV6CALC_DB_IPV4_TO_AS			0x00000010
#define IPV6CALC_DB_IPV6_TO_AS			0x00000020

#define IPV6CALC_DB_IPV4_TO_CC			0x00000100
#define IPV6CALC_DB_IPV6_TO_CC			0x00000200

#define IPV6CALC_DB_IEEE_TO_INFO		0x00001000

static const s_type ipv6calc_db_features[] = {
	{ IPV6CALC_DB_AS_TO_REGISTRY	, "DB_AS"	},
	{ IPV6CALC_DB_IPV4_TO_REGISTRY	, "DB_IPV4"	},
	{ IPV6CALC_DB_IPV6_TO_REGISTRY	, "DB_IPV6"	},
	{ IPV6CALC_DB_IPV4_TO_AS	, "DB_IPV4:AS"	},
	{ IPV6CALC_DB_IPV6_TO_AS	, "DB_IPV6_AS"	},
	{ IPV6CALC_DB_IPV4_TO_CC	, "DB_IPV4_CC"	},
	{ IPV6CALC_DB_IPV6_TO_CC	, "DB_IPV6_CC"	},
	{ IPV6CALC_DB_IEEE_TO_INFO	, "DB_IEEE"	}
};


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


extern int  libipv6calc_db_wrapper_init(void);
extern void libipv6calc_db_wrapper_info(char *string, const size_t size);
extern void libipv6calc_db_wrapper_features(char *string, const size_t size);
extern void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern int  libipv6calc_db_wrapper_has_features(uint32_t features);

/* functional wrappers */

// CountryCode Text/Number
extern char       *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto);

// Autonomous System Text/Number
extern char       *libipv6calc_db_wrapper_as_text_by_addr(const char *addr, const int proto);
extern uint32_t    libipv6calc_db_wrapper_as_num32_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_as_num16_by_addr(const char *addr, const int proto);

extern uint32_t    libipv6calc_db_wrapper_as_num32_comp17(const uint32_t as_num32);
extern uint32_t    libipv6calc_db_wrapper_as_num32_decomp17(const uint32_t as_num32_comp17);

// Registries
extern int         libipv6calc_db_wrapper_registry_num_by_as_num32(const uint32_t as_num32);
