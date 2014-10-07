/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.h
 * Version    : $Id: libipv6calc_db_wrapper.h,v 1.34 2014/10/07 20:25:23 ds6peter Exp $
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

#ifdef HAVE_BERKELEY_DB_SUPPORT
#include <db.h>
#endif // HAVE_BERKELEY_DB_SUPPORT

extern uint32_t wrapper_features;
extern uint32_t wrapper_features_by_source[];
extern uint32_t wrapper_features_by_source_implemented[];

#define IPV6CALC_PROTO_IPV4				4
#define IPV6CALC_PROTO_IPV6				6

// define generic feature numbers
#define IPV6CALC_DB_FEATURE_NUM_MIN			0
#define IPV6CALC_DB_FEATURE_NUM_AS_TO_REGISTRY		0
#define IPV6CALC_DB_FEATURE_NUM_CC_TO_REGISTRY		1
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGISTRY	2
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_REGISTRY	3
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_AS		4
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_AS		5
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_CC		6
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_CC		7
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_COUNTRY		8
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_COUNTRY		9
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_CITY		10
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_CITY		11
#define IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGION		12
#define IPV6CALC_DB_FEATURE_NUM_IPV6_TO_REGION		13
#define IPV6CALC_DB_FEATURE_NUM_IEEE_TO_INFO		14
#define IPV6CALC_DB_FEATURE_NUM_MAX			IPV6CALC_DB_FEATURE_NUM_IEEE_TO_INFO

// define generic features
#define IPV6CALC_DB_AS_TO_REGISTRY		(1 << IPV6CALC_DB_FEATURE_NUM_AS_TO_REGISTRY)
#define IPV6CALC_DB_CC_TO_REGISTRY		(1 << IPV6CALC_DB_FEATURE_NUM_CC_TO_REGISTRY)
#define IPV6CALC_DB_IPV4_TO_REGISTRY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGISTRY)
#define IPV6CALC_DB_IPV6_TO_REGISTRY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_REGISTRY)

#define IPV6CALC_DB_IPV4_TO_AS			(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_AS)
#define IPV6CALC_DB_IPV6_TO_AS			(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_AS)

#define IPV6CALC_DB_IPV4_TO_CC			(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_CC)
#define IPV6CALC_DB_IPV6_TO_CC			(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_CC)

#define IPV6CALC_DB_IPV4_TO_COUNTRY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_COUNTRY)
#define IPV6CALC_DB_IPV6_TO_COUNTRY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_COUNTRY)

#define IPV6CALC_DB_IPV4_TO_CITY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_CITY)
#define IPV6CALC_DB_IPV6_TO_CITY		(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_CITY)

#define IPV6CALC_DB_IPV4_TO_REGION		(1 << IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGION)
#define IPV6CALC_DB_IPV6_TO_REGION		(1 << IPV6CALC_DB_FEATURE_NUM_IPV6_TO_REGION)

#define IPV6CALC_DB_IEEE_TO_INFO		(1 << IPV6CALC_DB_FEATURE_NUM_IEEE_TO_INFO)

// define database specific generic features
#define IPV6CALC_DB_GEOIP_IPV4			0x00010000
#define IPV6CALC_DB_GEOIP_IPV6			0x00020000

#define IPV6CALC_DB_IP2LOCATION_IPV4		0x00100000
#define IPV6CALC_DB_IP2LOCATION_IPV6		0x00200000

#define IPV6CALC_DB_DBIP_IPV4			0x01000000
#define IPV6CALC_DB_DBIP_IPV6			0x02000000

#define IPV6CALC_DB_EXTERNAL_IPV4		0x10000000
#define IPV6CALC_DB_EXTERNAL_IPV6		0x20000000


static const s_formatoption ipv6calc_db_features[] = {
	{ IPV6CALC_DB_GEOIP_IPV4	, "GeoIP"		, "GeoIPv4 database"},
	{ IPV6CALC_DB_GEOIP_IPV6	, "GeoIPv6"		, "GeoIPv6 database"},
	{ IPV6CALC_DB_IP2LOCATION_IPV4	, "IP2Location"		, "IP2Location IPv4 database"},
	{ IPV6CALC_DB_IP2LOCATION_IPV6	, "IP2Location6"	, "IP2Location IPv6 database"},
	{ IPV6CALC_DB_DBIP_IPV4		, "DBIPv4"		, "db-ip.com IPv4 database"},
	{ IPV6CALC_DB_DBIP_IPV6		, "DBIPv6"		, "db-ip.com IPv6 database"},
	{ IPV6CALC_DB_AS_TO_REGISTRY	, "DB_AS_REG"		, "AS-Number to Registry database"},
	{ IPV6CALC_DB_IPV4_TO_REGISTRY	, "DB_IPV4_REG"		, "IPv4 to Registry database"},
	{ IPV6CALC_DB_IPV6_TO_REGISTRY	, "DB_IPV6_REG"		, "IPv6 to Registry database"},
	{ IPV6CALC_DB_IPV4_TO_AS	, "DB_IPV4_AS"		, "IPv4 to AS database" },
	{ IPV6CALC_DB_IPV6_TO_AS	, "DB_IPV6_AS"		, "IPv6 to AS database" },
	{ IPV6CALC_DB_IPV4_TO_CC	, "DB_IPV4_CC"		, "IPv4 to CountryCode database" },
	{ IPV6CALC_DB_IPV6_TO_CC	, "DB_IPV6_CC"		, "IPv6 to CountryCode database" },
	{ IPV6CALC_DB_IPV4_TO_COUNTRY	, "DB_IPV4_COUNTRY"	, "IPv4 to Country database" },
	{ IPV6CALC_DB_IPV6_TO_COUNTRY	, "DB_IPV6_COUNTRY"	, "IPv6 to Country database" },
	{ IPV6CALC_DB_IPV4_TO_CITY	, "DB_IPV4_CITY"	, "IPv4 to City database" },
	{ IPV6CALC_DB_IPV6_TO_CITY	, "DB_IPV6_CITY"	, "IPv6 to City database" },
	{ IPV6CALC_DB_IPV4_TO_REGION	, "DB_IPV4_REGION"	, "IPv4 to Region database" },
	{ IPV6CALC_DB_IPV6_TO_REGION	, "DB_IPV6_REGION"	, "IPv6 to Region database" },
	{ IPV6CALC_DB_CC_TO_REGISTRY	, "DB_CC_REG"		, "CountryCode to Registry database" },
	{ IPV6CALC_DB_IEEE_TO_INFO	, "DB_IEEE"		, "IEEE/OUI/OUI36 Vendor database" },
};

// data sources
#define IPV6CALC_DB_SOURCE_UNKNOWN		0

#define IPV6CALC_DB_SOURCE_MIN			1

#define IPV6CALC_DB_SOURCE_GEOIP		1
#define IPV6CALC_DB_SOURCE_IP2LOCATION		2
#define IPV6CALC_DB_SOURCE_DBIP			3
#define IPV6CALC_DB_SOURCE_EXTERNAL		4
#define IPV6CALC_DB_SOURCE_BUILTIN		5

#define IPV6CALC_DB_SOURCE_MAX			5

#define IPV6CALC_DB_PRIO_MAX			IPV6CALC_DB_SOURCE_MAX

typedef struct {
	const int  number;
	const char *name;
} s_data_sources;

static const s_data_sources data_sources[] = {
	{ IPV6CALC_DB_SOURCE_GEOIP	, "GeoIP"},
	{ IPV6CALC_DB_SOURCE_IP2LOCATION, "IP2Location"},
	{ IPV6CALC_DB_SOURCE_DBIP	, "db-ip.com"},
	{ IPV6CALC_DB_SOURCE_BUILTIN	, "BuiltIn"},
	{ IPV6CALC_DB_SOURCE_EXTERNAL	, "External"}
};

// database names and descriptions
typedef struct {
	const unsigned int number;
	const char        *filename;
	const char        *description;
	const uint32_t     features;
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

// db-info macro
#define IPV6CALC_DB_FEATURE_INFO(prefix, data_source) \
        printf("%s%s: features available/implemented: 0x%08x/0x%08x", \
		prefix, \
		libipv6calc_db_wrapper_get_data_source_name_by_number(data_source), \
		wrapper_features_by_source[data_source], \
		wrapper_features_by_source_implemented[data_source]); \
	if (strlen(prefix) == 0) { \
		int wrapper_features_by_source_bitcount = libipv6calc_bitcount_uint32_t(wrapper_features_by_source[data_source]); \
		int wrapper_features_by_source_implemented_bitcount = libipv6calc_bitcount_uint32_t(wrapper_features_by_source_implemented[data_source]); \
		if (wrapper_features_by_source_implemented_bitcount == 0) { \
			printf(" (NONE)"); \
		} else { \
			printf(" (%d%%)", (wrapper_features_by_source_bitcount * 100) / wrapper_features_by_source_implemented_bitcount); \
		}; \
	}; \
	printf("\n");

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

// macros for mapping index to chars
#define COUNTRYCODE_INDEX_TO_CHAR1(index)  ((index % COUNTRYCODE_LETTER1_MAX) + 'A')
#define COUNTRYCODE_INDEX_TO_CHAR2(index)  ((index / COUNTRYCODE_LETTER1_MAX) > 9) ? ((index / COUNTRYCODE_LETTER1_MAX) - 10 + 'A') : ((index / COUNTRYCODE_LETTER1_MAX) + '0')


// generic database lookup function
#define IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY		1	 // array

#ifdef HAVE_BERKELEY_DB_SUPPORT
#define IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB		2	 // Berkeley DB
#endif // HAVE_BERKELEY_DB_SUPPORT

// data storage type
#define IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST	1	 // key is first-last
#define IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK	2	 // key is base/mask

// data search type
#define IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY	1	 // binary search
#define IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST	2	 // sequential longest match

// Berkeley DB  lookup function
#ifdef HAVE_BERKELEY_DB_SUPPORT
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2		0
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4		1
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2		2
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4		3
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x2	4
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4	5
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREFIX_32x2	6
#define IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREFIX_32x4	7

// database info/data
typedef struct {
	DB *db_info_ptr;
	DB *db_data_ptr;
	long int db_data_max;
} s_db_info_data;

#endif // HAVE_BERKELEY_DB_SUPPORT

#endif


extern int  libipv6calc_db_wrapper_init(void);
extern int  libipv6calc_db_wrapper_cleanup(void);
extern void libipv6calc_db_wrapper_info(char *string, const size_t size);
extern void libipv6calc_db_wrapper_features(char *string, const size_t size);
extern void libipv6calc_db_wrapper_capabilities(char *string, const size_t size);
extern void libipv6calc_db_wrapper_features_help(void);
extern void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern int  libipv6calc_db_wrapper_has_features(uint32_t features);
extern int  libipv6calc_db_wrapper_options(const int opt, const char *optarg, const struct option longopts[]);
extern const char *libipv6calc_db_wrapper_get_data_source_name_by_number(const int number);


/* functional wrappers */

// CountryCode Text/Number
extern char       *libipv6calc_db_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, unsigned int *data_source_ptr);
extern int         libipv6calc_db_wrapper_country_code_by_cc_index(char *string, int length, const uint16_t cc_index);
extern uint16_t    libipv6calc_db_wrapper_cc_index_by_addr(const ipv6calc_ipaddr *ipaddrp, unsigned int *data_source_ptr);

// Autonomous System Text/Number
extern char       *libipv6calc_db_wrapper_as_text_by_addr(const ipv6calc_ipaddr *ipaddrp);
extern uint32_t    libipv6calc_db_wrapper_as_num32_by_addr(const ipv6calc_ipaddr *ipaddrp);
extern uint16_t    libipv6calc_db_wrapper_as_num16_by_addr(const ipv6calc_ipaddr *ipaddrp);

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
extern int libipv6calc_db_wrapper_bdb_get_data_by_key(DB *dbp, char *token, char *value, const size_t value_size);
#endif // HAVE_BERKELEY_DB_SUPPORT

// generic DB lookup
extern int libipv6calc_db_wrapper_get_entry_generic(
	void 		*db_ptr,		// pointer to database in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	const uint8_t	data_ptr_type,		// type of data_ptr
	const uint8_t	data_key_type,		// key type
	const uint8_t   data_key_format,        // key format
	const uint8_t	data_key_length,	// key length
	const uint8_t	data_search_type,	// search type
	const long int	data_key_row_min,	// number of first usable row (begin)
	const long int	data_key_row_max,	// number of last usable row (end)
	const uint32_t	lookup_key_00_31,	// lookup key MSB
	const uint32_t	lookup_key_32_63,	// lookup key LSB
	void            *data_ptr,		// pointer to DB data in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	int  (*get_array_row)()			// function to get array row
	);
