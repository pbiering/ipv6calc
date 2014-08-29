/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.h
 * Version    : $Id: libipv6calc_db_wrapper.h,v 1.28 2014/08/29 06:11:35 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper.c
 */

#ifndef _libipv6calc_db_wrapper_h

#define _libipv6calc_db_wrapper_h 1

#include "ipv6calctypes.h"
#include "libmac.h"
#include "libipv4addr.h"
#include "libipv6addr.h"

extern uint32_t wrapper_features;

// define features
#define IPV6CALC_DB_AS_TO_REGISTRY		0x00000001
#define IPV6CALC_DB_IPV4_TO_REGISTRY		0x00000002
#define IPV6CALC_DB_IPV6_TO_REGISTRY		0x00000004
#define IPV6CALC_DB_CC_TO_REGISTRY		0x00000008

#define IPV6CALC_DB_IPV4_TO_AS			0x00000010
#define IPV6CALC_DB_IPV6_TO_AS			0x00000020

#define IPV6CALC_DB_IPV4_TO_CC			0x00000100
#define IPV6CALC_DB_IPV6_TO_CC			0x00000200

#define IPV6CALC_DB_IEEE_TO_INFO		0x00001000

#define IPV6CALC_DB_GEOIP_IPV4			0x00010000
#define IPV6CALC_DB_GEOIP_IPV6			0x00020000

#define IPV6CALC_DB_IP2LOCATION_IPV4		0x00100000
#define IPV6CALC_DB_IP2LOCATION_IPV6		0x00200000

#define IPV6CALC_DB_DBIP_IPV4			0x01000000
#define IPV6CALC_DB_DBIP_IPV6			0x02000000

static const s_formatoption ipv6calc_db_features[] = {
	{ IPV6CALC_DB_GEOIP_IPV4	, "GeoIP"	, "GeoIPv4 database (external)"},
	{ IPV6CALC_DB_GEOIP_IPV6	, "GeoIPv6"	, "GeoIPv6 database (external)"},
	{ IPV6CALC_DB_IP2LOCATION_IPV4	, "IP2Location"	, "IP2Location IPv4 database (external)"},
	{ IPV6CALC_DB_IP2LOCATION_IPV6	, "IP2Location6", "IP2Location IPv6 database (external)"},
	{ IPV6CALC_DB_DBIP_IPV4		, "DBIPv4"	, "DB-IP IPv4 database (external)"},
	{ IPV6CALC_DB_DBIP_IPV6		, "DBIPv6"	, "DB-IP IPv6 database (external)"},
	{ IPV6CALC_DB_AS_TO_REGISTRY	, "DB_AS_REG"	, "AS-Number to Registry database (BuiltIn)"},
	{ IPV6CALC_DB_IPV4_TO_REGISTRY	, "DB_IPV4_REG"	, "IPv4 to Registry database (BuiltIn)"},
	{ IPV6CALC_DB_IPV6_TO_REGISTRY	, "DB_IPV6_REG"	, "IPv6 to Registry database (BuiltIn)"},
	{ IPV6CALC_DB_IPV4_TO_AS	, "DB_IPV4_AS"	, "IPv4 to AS database (provided by GeoIP)" },
	{ IPV6CALC_DB_IPV6_TO_AS	, "DB_IPV6_AS"	, "IPv6 to AS database (provided by GeoIPv6)" },
	{ IPV6CALC_DB_IPV4_TO_CC	, "DB_IPV4_CC"	, "IPv4 to CountryCode database (provided by GeoIP/IP2Location)" },
	{ IPV6CALC_DB_IPV6_TO_CC	, "DB_IPV6_CC"	, "IPv6 to CountryCode database (provided by GeoIPv6/IP2Location6)" },
	{ IPV6CALC_DB_CC_TO_REGISTRY	, "DB_CC_REG"	, "CountryCode to Registry database (BuiltIn)" },
	{ IPV6CALC_DB_IEEE_TO_INFO	, "DB_IEEE"	, "IEEE/OUI/OUI36 Vendor database (BuiltIn)" },
};


// database names and descriptions
typedef struct {
	const unsigned int number;
	const char        *filename;
	const char        *description;
	const uint32_t     feature;
} db_file_desc;


// define internal API versions
#define IPV6CALC_DB_API_GEOIP		1
#define IPV6CALC_DB_API_IP2LOCATION	1
#define IPV6CALC_DB_API_DBIP		1
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
#define COUNTRYCODE_INDEX_MAX            1023	// 0x3ff
#define COUNTRYCODE_INDEX_UNKNOWN        1022	// 0x3fe

#define COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MAX   (COUNTRYCODE_INDEX_UNKNOWN - 1)
#define COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN   (COUNTRYCODE_INDEX_UNKNOWN - 16)

// macros for mapping index to chars and vice-versa
#define COUNTRYCODE_INDEX_TO_CHAR1(index)  ((index % COUNTRYCODE_LETTER1_MAX) + 'A')
#define COUNTRYCODE_INDEX_TO_CHAR2(index)  ((index / COUNTRYCODE_LETTER1_MAX) > 9) ? ((index / COUNTRYCODE_LETTER1_MAX) - 10 + 'A') : ((index / COUNTRYCODE_LETTER1_MAX) + '0')


#endif


extern int  libipv6calc_db_wrapper_init(void);
extern void libipv6calc_db_wrapper_info(char *string, const size_t size);
extern void libipv6calc_db_wrapper_features(char *string, const size_t size);
extern void libipv6calc_db_wrapper_capabilities(char *string, const size_t size);
extern void libipv6calc_db_wrapper_features_help(void);
extern void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern int  libipv6calc_db_wrapper_has_features(uint32_t features);
extern int  libipv6calc_db_wrapper_options(const int opt, const char *optarg, const struct option longopts[]);

/* functional wrappers */

// CountryCode Text/Number
extern char       *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto);

extern int         libipv6calc_db_wrapper_country_code_by_cc_index(char *string, int length, const uint16_t cc_index);

// Autonomous System Text/Number
extern char       *libipv6calc_db_wrapper_as_text_by_addr(const char *addr, const int proto);
extern uint32_t    libipv6calc_db_wrapper_as_num32_by_addr(const char *addr, const int proto);
extern uint16_t    libipv6calc_db_wrapper_as_num16_by_addr(const char *addr, const int proto);

extern uint32_t    libipv6calc_db_wrapper_as_num32_comp17(const uint32_t as_num32);
extern uint32_t    libipv6calc_db_wrapper_as_num32_decomp17(const uint32_t as_num32_comp17);

// Registries
extern int         libipv6calc_db_wrapper_registry_num_by_as_num32(const uint32_t as_num32);
extern int         libipv6calc_db_wrapper_registry_num_by_cc_index(const uint16_t cc_index);

// IEEE
extern int libipv6calc_db_wrapper_ieee_vendor_string_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp);
extern int libipv6calc_db_wrapper_ieee_vendor_string_short_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp);

// IPv4 Registry
extern int libipv6calc_db_wrapper_registry_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length);
extern int libipv6calc_db_wrapper_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp);

// IPv6 Registry
extern int libipv6calc_db_wrapper_registry_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);
extern int libipv6calc_db_wrapper_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp);

#ifdef HAVE_BERKELEY_DB_SUPPORT
#include <db.h>

int libipv6calc_db_wrapper_get_dbentry_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, DB *dbp, const int db_format, char *resultstring, const size_t resultstring_length);
int libipv6calc_db_wrapper_get_dbentry_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, DB *dbp, const int db_format, char *resultstring, const size_t resultstring_length);
#endif // HAVE_BERKELEY_DB_SUPPORT
