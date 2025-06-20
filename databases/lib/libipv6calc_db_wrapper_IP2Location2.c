/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location2.c
 * Copyright  : 2025-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc IP2Location database wrapper
 *    - decoupling databases from main binary
 */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "libipv6calc_db_wrapper.h"

#ifdef SUPPORT_IP2LOCATION2

#include "libipv6calc_db_wrapper_IP2Location2.h"

#define TEST_IP2LOCATION2_AVAILABLE(v)	((v != NULL) && (strstr(v, "unavailable") == NULL) && (strstr(v, " sample BIN ") == NULL) && (strstr(v, "INVALID") == NULL) && (strstr(v, "-") == NULL))

char ip2location2_db_dir[PATH_MAX] = IP2LOCATION2_DB;

static const char* wrapper_ip2location2_info = "IP2Location2";


/* database files */
static const db_file_desc2 libipv6calc_db_wrapper_IP2Location2_db_file_desc[] = {
	// sequence is important for automagic selection of the best

	/*****************/
	/**** SAMPLES ****/
	/*****************/
	// SAMPLES IPv4+IPv6
	{ IP2LOCATION2_IPVx_SAMPLE_DB1,
		"IP-COUNTRY-SAMPLE.MMDB", IP2LOCATION2_DB1,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_COUNTRY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB2,
		"IP-COUNTRY-ISP-SAMPLE.MMDB", IP2LOCATION2_DB2,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_COUNTRY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB3,
		"IP-COUNTRY-REGION-CITY-SAMPLE.MMDB", IP2LOCATION2_DB3,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB4,
		"IP-COUNTRY-REGION-CITY-ISP-SAMPLE.MMDB", IP2LOCATION2_DB4,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB5,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-SAMPLE.MMDB", IP2LOCATION2_DB5,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB6,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-SAMPLE.MMDB", IP2LOCATION2_DB6,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB7,
		"IP-COUNTRY-REGION-CITY-ISP-DOMAIN-SAMPLE.MMDB", IP2LOCATION2_DB7,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB8,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN-SAMPLE.MMDB", IP2LOCATION2_DB8,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB9,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-SAMPLE.MMDB", IP2LOCATION2_DB9,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB10,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-ISP-DOMAIN-SAMPLE.MMDB", IP2LOCATION2_DB10,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB11,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-SAMPLE.MMDB", IP2LOCATION2_DB11,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB12,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-SAMPLE.MMDB", IP2LOCATION2_DB12,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB13,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-TIMEZONE-NETSPEED-SAMPLE.MMDB", IP2LOCATION2_DB13,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB14,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-SAMPLE.MMDB", IP2LOCATION2_DB14,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB15,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-AREACODE-SAMPLE.MMDB", IP2LOCATION2_DB15,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB16,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-SAMPLE.MMDB", IP2LOCATION2_DB16,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB17,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-TIMEZONE-NETSPEED-WEATHER-SAMPLE.MMDB", IP2LOCATION2_DB17,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB18,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-SAMPLE.MMDB", IP2LOCATION2_DB18,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB19,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN-MOBILE-SAMPLE.MMDB", IP2LOCATION2_DB19,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB20,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-SAMPLE.MMDB", IP2LOCATION2_DB20,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB21,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-AREACODE-ELEVATION-SAMPLE.MMDB", IP2LOCATION2_DB21,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE  },
	{ IP2LOCATION2_IPVx_SAMPLE_DB22,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-SAMPLE.MMDB", IP2LOCATION2_DB22,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE  },
	{ IP2LOCATION2_IPVx_SAMPLE_DB23,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN-MOBILE-USAGETYPE-SAMPLE.MMDB", IP2LOCATION2_DB23,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE  },
	{ IP2LOCATION2_IPVx_SAMPLE_DB24,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE-SAMPLE.MMDB", IP2LOCATION2_DB24,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE  },
	{ IP2LOCATION2_IPVx_SAMPLE_DB25,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE-ADDRESSTYPE-CATEGORY-SAMPLE.MMDB", IP2LOCATION2_DB25,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_DB26,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE-ADDRESSTYPE-CATEGORY-DISTRICT-ASN-SAMPLE.MMDB", IP2LOCATION2_DB26,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_ASN , IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },
	{ IP2LOCATION2_IPVx_SAMPLE_ASN,
		"IP-ASN-SAMPLE.MMDB", IP2LOCATION2_ASN,
		IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_IPV6_TO_AS, IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE },


	/**************/
	/**** LITE ****/
	/**************/

	// LITE IPv4+IPv6
	{ IP2LOCATION2_DB_IPVx_COUNTRY_LITE,   "IP2LOCATION-LITE-DB1.MMDB"     , IP2LOCATION2_DB1,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_COUNTRY | IPV6CALC_DB_SC_IP2LOCATION2_IPV6_COUNTRY | IPV6CALC_DB_IP_TO_GEONAMEID, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },
	{ IP2LOCATION2_DB_IPVx_COUNTRY_REG_CIT_LITE, "IP2LOCATION-LITE-DB3.MMDB", IP2LOCATION2_DB3,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },
	{ IP2LOCATION2_DB_IPVx_COUNTRY_REG_CIT_LL_LITE, "IP2LOCATION-LITE-DB5.MMDB", IP2LOCATION2_DB5,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },
	{ IP2LOCATION2_DB_IPVx_COUNTRY_REG_CIT_LL_ZIP_LITE, "IP2LOCATION-LITE-DB9.MMDB", IP2LOCATION2_DB9,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },
	{ IP2LOCATION2_DB_IPVx_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE, "IP2LOCATION-LITE-DB11.MMDB", IP2LOCATION2_DB11,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },
	{ IP2LOCATION2_DB_IPVx_ASN_LITE, "IP2LOCATION-LITE-ASN.MMDB", IP2LOCATION2_ASN,
		IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_IPV6_TO_AS, IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE },


	/**************/
	/* commercial */
	/**************/

	// Commercial IPv4+IPv6
	{ IP2LOCATION2_IPVx_DB1,
		"IP-COUNTRY.MMDB", IP2LOCATION2_DB1,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_COUNTRY, 0 },
	{ IP2LOCATION2_IPVx_DB2,
		"IP-COUNTRY-ISP.MMDB", IP2LOCATION2_DB2,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_COUNTRY, 0 },
	{ IP2LOCATION2_IPVx_DB3,
		"IP-COUNTRY-REGION-CITY.MMDB", IP2LOCATION2_DB3,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB4,
		"IP-COUNTRY-REGION-CITY-ISP.MMDB", IP2LOCATION2_DB4,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB5,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE.MMDB", IP2LOCATION2_DB5,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB6,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP.MMDB", IP2LOCATION2_DB6,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB7,
		"IP-COUNTRY-REGION-CITY-ISP-DOMAIN.MMDB", IP2LOCATION2_DB7,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB8,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN.MMDB", IP2LOCATION2_DB8,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB9,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE.MMDB", IP2LOCATION2_DB9,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB10,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-ISP-DOMAIN.MMDB", IP2LOCATION2_DB10,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB11,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE.MMDB", IP2LOCATION2_DB11,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB12,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN.MMDB", IP2LOCATION2_DB12,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB13,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-TIMEZONE-NETSPEED.MMDB", IP2LOCATION2_DB13,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB14,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED.MMDB", IP2LOCATION2_DB14,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB15,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-AREACODE.MMDB", IP2LOCATION2_DB15,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB16,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE.MMDB", IP2LOCATION2_DB16,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB17,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-TIMEZONE-NETSPEED-WEATHER.MMDB", IP2LOCATION2_DB17,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB18,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER.MMDB", IP2LOCATION2_DB18,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB19,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN-MOBILE.MMDB", IP2LOCATION2_DB19,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB20,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE.MMDB", IP2LOCATION2_DB20,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB21,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-AREACODE-ELEVATION.MMDB", IP2LOCATION2_DB21,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB22,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION.MMDB", IP2LOCATION2_DB22,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB23,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ISP-DOMAIN-MOBILE-USAGETYPE.MMDB", IP2LOCATION2_DB23,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB24,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE.MMDB", IP2LOCATION2_DB24,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB25,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE-ADDRESSTYPE-CATEGORY.MMDB", IP2LOCATION2_DB25,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_CITY, 0 },
	{ IP2LOCATION2_IPVx_DB26,
		"IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE-ZIPCODE-TIMEZONE-ISP-DOMAIN-NETSPEED-AREACODE-WEATHER-MOBILE-ELEVATION-USAGETYPE-ADDRESSTYPE-CATEGORY-DISTRICT-ASN.MMDB", IP2LOCATION2_DB26,
		IPV6CALC_DB_SC_IP2LOCATION2_IPVx_ASN, 0 },
	{ IP2LOCATION2_IPVx_ASN,
		"IP-ASN.MMDB", IP2LOCATION2_ASN,
		IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_IPV6_TO_AS, 0 },

};


/* IP2Location Usage Type explanation
    see also: http://www.ip2location.com/databases/db24-ip-country-region-city-latitude-longitude-zipcode-timezone-isp-domain-netspeed-areacode-weather-mobile-elevation-usagetype
*/
static const s_type2 libipv6calc_db_wrapper_IP2Location2_UsageType[] = {
	{ 0,  "Commercial"                     , "COM" },
	{ 1,  "Organization"                   , "ORG" },
	{ 2,  "Government"                     , "GOV" },
	{ 3,  "Military"                       , "MIL" },
	{ 4,  "University/College/School"      , "EDU" },
	{ 5,  "Library"                        , "LIB" },
	{ 6,  "Content Delivery Network"       , "CDN" },
	{ 7,  "Fixed Line ISP"                 , "ISP" },
	{ 8,  "Mobile ISP"                     , "MOB" },
	{ 9,  "Data Center/Web Hosting/Transit", "DCH" },
	{ 10, "Search Engine Spider"           , "SES" },
	{ 11, "Reserved"                       , "RSV" }
};


// DB used for resolution after automatic election, which is currently done by checking for DB files and the sequence in the header file
static int ip2location2_db_country_v4 = 0;
static int ip2location2_db_country_v6 = 0;
static int ip2location2_db_region_city_v4 = 0;
static int ip2location2_db_region_city_v6 = 0;
static int ip2location2_db_asn_v4 = 0;
static int ip2location2_db_asn_v6 = 0;
static int ip2location2_db_geonameid_v4 = 0;
static int ip2location2_db_geonameid_v6 = 0;

#define IP2L_SAMPLE	0
#define IP2L_LITE	1
#define IP2L_COMM	2
#define	IP2L_MAX	3

static s_libipv6calc_db_wrapper_db_info ip2location2_db_country_v4_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_country_v6_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_region_city_v4_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_region_city_v6_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_asn_v4_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_asn_v6_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_geonameid_v4_best[IP2L_MAX];
static s_libipv6calc_db_wrapper_db_info ip2location2_db_geonameid_v6_best[IP2L_MAX];

static int ip2location2_db_country_sample_v4_lite_autoswitch = 0;
static int ip2location2_db_country_sample_v6_lite_autoswitch = 0;
static int ip2location2_db_region_city_sample_v4_lite_autoswitch = 0;
static int ip2location2_db_region_city_sample_v6_lite_autoswitch = 0;
static int ip2location2_db_asn_sample_v4_lite_autoswitch = 0;
static int ip2location2_db_asn_sample_v6_lite_autoswitch = 0;
static int ip2location2_db_geonameid_sample_v4_lite_autoswitch = 0;
static int ip2location2_db_geonameid_sample_v6_lite_autoswitch = 0;

static int type2index[IP2LOCATION2_DB_MAX+1];

// select automagically SAMPLE databases in case available and matching and not older than given months
int ip2location2_db_lite_to_sample_autoswitch_max_delta_months = 12;

// select LITE database if COMM is older than given months
int ip2location2_db_comm_to_lite_switch_min_delta_months = 12;

// select better database of same product (COMM/LITE/SAMPLE) only if not older than given months
int ip2location2_db_better_max_delta_months = 1;

// select only a specific IP2Location db type
int ip2location2_db_only_type = 0;

// allow soft links (usually skipped)
int ip2location2_db_allow_softlinks = 0;

char ***libipv6calc_db_wrapper_IP2Location2DBFileName_ptr = NULL;
const char **libipv6calc_db_wrapper_IP2Location2DBDescription = NULL;

/* database usage map */
#define IP2LOCATION2_DB_MAX_BLOCKS_32	32	// 0-1023
static uint32_t ip2location2_db_usage_map[IP2LOCATION2_DB_MAX_BLOCKS_32];

#define IP2LOCATION2_DB_USAGE_MAP_TAG(db)	if (db < (32 * IP2LOCATION2_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Tag usage for db: %d", db); \
							ip2location2_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * IP2LOCATION2_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char ip2location2_db_usage_string[IPV6CALC_STRING_MAX] = "";

// local cache
static MMDB_s mmdb_cache[IP2LOCATION2_DB_MAX+1];

// local prototyping
static char *libipv6calc_db_wrapper_IP2Location2_dbfilename(const unsigned int type); 
static void libipv6calc_db_wrapper_IP2Location2_close(const int type);


/*
 * function initialise the IP2Location wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_IP2Location2_wrapper_init(void) {
	int i, dbym, product, dbtype, result, ret;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Check for standard IP2Location(MMDB) databases");

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		// add entry to mapping
		type2index[libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number] = i;

		// clean local cache
		mmdb_cache[libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number].file_size = 0;
		mmdb_cache[libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number].flags = 0;

		// add features to implemented
		wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_IP2LOCATION2] |= libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "IP2Location(MMDB) database test for availability: %s", libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename);
		result = libipv6calc_db_wrapper_IP2Location2_db_avail(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number);
		if ((result != 1) && (result != 3)) {
			// db not available
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "IP2Location(MMDB) database available: %s", libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description);

		ret = libipv6calc_db_wrapper_IP2Location2_open_type(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number);
		if (ret != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location(MMDB) by type");
			continue;
		};

		dbtype = libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number;

		if ((ip2location2_db_only_type > 0) && (ip2location2_db_only_type != dbtype)) {
			// not selected
			continue;
		};

		time_t db_time = mmdb_cache[dbtype].metadata.build_epoch;
		struct tm *db_gmtime = gmtime(&db_time);

		dbym = (db_gmtime->tm_year + 1900) * 12 + db_gmtime->tm_mon;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "IP2Location(MMDB) type=%d dbym=%d Year/Month=%d unixtime=%llu", dbtype, dbym, UNPACK_YM(dbym), (long long unsigned int) mmdb_cache[dbtype].metadata.build_epoch);

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].internal & IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE) != 0) {
			product = IP2L_SAMPLE;
		} else if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].internal & IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE) != 0) {
			product = IP2L_LITE;
		} else {
			product = IP2L_COMM;
		};

#define IP2L_DB_SELECT_BETTER(best) \
			if ( \
			  (best.num == 0) \
			  || ( \
				(best.dbym > 0) \
				  && ((best.dbym - dbym) <= ip2location2_db_better_max_delta_months) \
			     ) \
			) { \
				best.num = libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number; \
				best.dbym = dbym; \
				best.dbtype = dbtype; \
			};
		

		// note: databases are listed in sequence "less data" before "more data"
		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_CC) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_country_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_CC) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_country_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY)) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_region_city_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY)) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_region_city_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IPV4_TO_AS)) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_asn_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IPV6_TO_AS)) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_asn_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_GEONAMEID) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_geonameid_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_GEONAMEID) != 0) {
			IP2L_DB_SELECT_BETTER(ip2location2_db_geonameid_v6_best[product])
		};

		wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] |= libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features;
	};

	// select databases for lite->sample autoswitch
	if (ip2location2_db_lite_to_sample_autoswitch_max_delta_months > 0) {
		// select best sample db for autoswitch lite->sample
#define SELECT_LITE_SAMPLE_AUTOSWITCH(sample, lite, autoswitch) \
		if ((sample.num > 0) && (lite.num > 0)) { \
			if (sample.dbtype > lite.dbtype) { \
				if (lite.dbym - sample.dbym <= ip2location2_db_lite_to_sample_autoswitch_max_delta_months) { \
						autoswitch = sample.num; \
				}; \
			}; \
		};

		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_country_v4_best[IP2L_SAMPLE], ip2location2_db_country_v4_best[IP2L_LITE], ip2location2_db_country_sample_v4_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_country_v6_best[IP2L_SAMPLE], ip2location2_db_country_v6_best[IP2L_LITE], ip2location2_db_country_sample_v6_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_region_city_v4_best[IP2L_SAMPLE], ip2location2_db_region_city_v4_best[IP2L_LITE], ip2location2_db_region_city_sample_v4_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_region_city_v6_best[IP2L_SAMPLE], ip2location2_db_region_city_v6_best[IP2L_LITE], ip2location2_db_region_city_sample_v6_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_asn_v4_best[IP2L_SAMPLE], ip2location2_db_asn_v4_best[IP2L_LITE], ip2location2_db_asn_sample_v4_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_asn_v6_best[IP2L_SAMPLE], ip2location2_db_asn_v6_best[IP2L_LITE], ip2location2_db_asn_sample_v6_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_geonameid_v4_best[IP2L_SAMPLE], ip2location2_db_geonameid_v4_best[IP2L_LITE], ip2location2_db_geonameid_sample_v4_lite_autoswitch)
		SELECT_LITE_SAMPLE_AUTOSWITCH(ip2location2_db_geonameid_v6_best[IP2L_SAMPLE], ip2location2_db_geonameid_v6_best[IP2L_LITE], ip2location2_db_geonameid_sample_v6_lite_autoswitch)
	};

	// select lite instead of comm, if comm is outdated and lite available
	if (ip2location2_db_comm_to_lite_switch_min_delta_months > 0) {
#define SELECT_LITE_INSTEAD_OF_COMM(lite, comm, final) \
	if ((lite.num > 0) && (comm.num > 0)) { \
		if (lite.dbym - comm.dbym > ip2location2_db_comm_to_lite_switch_min_delta_months) { \
			final = lite.num; \
		}; \
	};
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_country_v4_best[IP2L_LITE], ip2location2_db_country_v4_best[IP2L_COMM], ip2location2_db_country_v4)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_country_v6_best[IP2L_LITE], ip2location2_db_country_v6_best[IP2L_COMM], ip2location2_db_country_v6)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_region_city_v4_best[IP2L_LITE], ip2location2_db_region_city_v4_best[IP2L_COMM], ip2location2_db_region_city_v4)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_region_city_v6_best[IP2L_LITE], ip2location2_db_region_city_v6_best[IP2L_COMM], ip2location2_db_region_city_v6)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_asn_v4_best[IP2L_LITE], ip2location2_db_asn_v4_best[IP2L_COMM], ip2location2_db_asn_v4)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_asn_v6_best[IP2L_LITE], ip2location2_db_asn_v6_best[IP2L_COMM], ip2location2_db_asn_v6)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_geonameid_v4_best[IP2L_LITE], ip2location2_db_geonameid_v4_best[IP2L_COMM], ip2location2_db_geonameid_v4)
		SELECT_LITE_INSTEAD_OF_COMM(ip2location2_db_geonameid_v6_best[IP2L_LITE], ip2location2_db_geonameid_v6_best[IP2L_COMM], ip2location2_db_geonameid_v6)
	};

#define FILL_EMPTY(product, final) \
	if ((product.num > 0) && (final == 0)) { \
		final = product.num; \
	};

	// fill empty ones with comm
	FILL_EMPTY(ip2location2_db_country_v4_best[IP2L_COMM], ip2location2_db_country_v4)
	FILL_EMPTY(ip2location2_db_country_v6_best[IP2L_COMM], ip2location2_db_country_v6)
	FILL_EMPTY(ip2location2_db_region_city_v4_best[IP2L_COMM], ip2location2_db_region_city_v4)
	FILL_EMPTY(ip2location2_db_region_city_v6_best[IP2L_COMM], ip2location2_db_region_city_v6)
	FILL_EMPTY(ip2location2_db_asn_v4_best[IP2L_COMM], ip2location2_db_asn_v4)
	FILL_EMPTY(ip2location2_db_asn_v6_best[IP2L_COMM], ip2location2_db_asn_v6)
	FILL_EMPTY(ip2location2_db_geonameid_v4_best[IP2L_COMM], ip2location2_db_geonameid_v4)
	FILL_EMPTY(ip2location2_db_geonameid_v6_best[IP2L_COMM], ip2location2_db_geonameid_v6)

	// fill empty ones with lite
	FILL_EMPTY(ip2location2_db_country_v4_best[IP2L_LITE], ip2location2_db_country_v4)
	FILL_EMPTY(ip2location2_db_country_v6_best[IP2L_LITE], ip2location2_db_country_v6)
	FILL_EMPTY(ip2location2_db_region_city_v4_best[IP2L_LITE], ip2location2_db_region_city_v4)
	FILL_EMPTY(ip2location2_db_region_city_v6_best[IP2L_LITE], ip2location2_db_region_city_v6)
	FILL_EMPTY(ip2location2_db_asn_v4_best[IP2L_LITE], ip2location2_db_asn_v4)
	FILL_EMPTY(ip2location2_db_asn_v6_best[IP2L_LITE], ip2location2_db_asn_v6)
	FILL_EMPTY(ip2location2_db_geonameid_v4_best[IP2L_LITE], ip2location2_db_geonameid_v4)
	FILL_EMPTY(ip2location2_db_geonameid_v6_best[IP2L_LITE], ip2location2_db_geonameid_v6)

	// fill empty ones with sample
	FILL_EMPTY(ip2location2_db_country_v4_best[IP2L_SAMPLE], ip2location2_db_country_v4)
	FILL_EMPTY(ip2location2_db_country_v6_best[IP2L_SAMPLE], ip2location2_db_country_v6)
	FILL_EMPTY(ip2location2_db_region_city_v4_best[IP2L_SAMPLE], ip2location2_db_region_city_v4)
	FILL_EMPTY(ip2location2_db_region_city_v6_best[IP2L_SAMPLE], ip2location2_db_region_city_v6)
	FILL_EMPTY(ip2location2_db_asn_v4_best[IP2L_SAMPLE], ip2location2_db_asn_v4)
	FILL_EMPTY(ip2location2_db_asn_v6_best[IP2L_SAMPLE], ip2location2_db_asn_v6)
	FILL_EMPTY(ip2location2_db_geonameid_v4_best[IP2L_SAMPLE], ip2location2_db_geonameid_v4)
	FILL_EMPTY(ip2location2_db_geonameid_v6_best[IP2L_SAMPLE], ip2location2_db_geonameid_v6)

	/* close handles which are not necessary further on */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		if (mmdb_cache[libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number].file_size > 0) {
			if (
				(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v4_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v4_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v4_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v6_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v6_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_country_v6_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v4_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v4_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v4_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v6_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v6_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_region_city_v6_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v4_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v4_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v4_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v6_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v6_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_asn_v6_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v4_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v4_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v4_best[IP2L_SAMPLE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v6_best[IP2L_LITE].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v6_best[IP2L_COMM].num)
			    ||	(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == ip2location2_db_geonameid_v6_best[IP2L_SAMPLE].num)
			) {
				// database is in use
				continue;
			};

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Close further unused IP2Location(MMDB): type=%d desc='%s'", libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number, libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description);
			libipv6calc_db_wrapper_IP2Location2_close(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number);
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "custom directory: %s", ip2location2_db_dir);

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2];

	return 0;
};


/*
 * wrapper: IP2Location2_close
 */
void libipv6calc_db_wrapper_IP2Location2_close(const int type) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called type=%u", type);

	if (mmdb_cache[type].file_size > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Close IP2Location(MMDB): type=%d desc='%s'", type, libipv6calc_db_wrapper_IP2Location2_db_file_desc[type2index[type]].description);
		libipv6calc_db_wrapper_MMDB_close(&mmdb_cache[type]);
		/* cleanup cache entry */
		mmdb_cache[type].file_size = 0;
	} else if (mmdb_cache[type].flags > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Already closed IP2Location(MMDB): type=%d desc='%s'", type, libipv6calc_db_wrapper_IP2Location2_db_file_desc[type2index[type]].description);
	};
};



/*
 * function info of IP2Location2 wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_IP2Location2_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called");

	snprintf(string, size, "IP2Location(MMDB) database support: Country4=%d Country6=%d City4=%d City6=%d ASN4=%d ASN6=%d GeonameID4=%d GeonameID6=%d"
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_CC)   ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_CC)   ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_CITY) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_CITY) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_AS)   ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_AS)   ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_GEONAMEID) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_GEONAMEID) ? 1 : 0
	);

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished");
	return;
};

/*
 * function print database info of IP2Location2 wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_IP2Location2_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	int ret, i, type, count = 0, r;
	s_libipv6calc_db_wrapper_db_info_all db_info_all;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called");

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_IP2LOCATION2)

	fprintf(stderr, "%sIP2Location(MMDB): info of available databases in directory: %s\n", prefix, ip2location2_db_dir);
	fprintf(stderr, "%sIP2Location(MMDB): Copyright (c) IP2Location All Rights Reserved\n", prefix);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number;

		r = libipv6calc_db_wrapper_IP2Location2_db_avail(type);
		if (r == 2) {
			fprintf(stderr, "%sIP2Location(MMDB): %s %-43s:[%3d] %-35s (SOFTLINK IS UNSUPPORTED)\n", prefix,
				((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) == (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) ? "IPvx" : (((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IP2LOCATION2_IPV6) != 0) ? "IPv6" : "IPv4"),
				libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description,
				libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number,
				libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename);
		} else if ((r == 1) || (r == 3)) {
			// IP2Location returned that database is available
			ret = libipv6calc_db_wrapper_IP2Location2_open_type(type);
			if (ret != MMDB_SUCCESS) {
				fprintf(stderr, "%sIP2Location(MMDB): %s %-43s:[%3d] %-35s (CAN'T OPEN)\n", prefix,
					((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) == (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) ? "IPvx" : (((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IP2LOCATION2_IPV6) != 0) ? "IPv6" : "IPv4"),
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename
				);
			} else { 
				fprintf(stderr, "%sIP2Location(MMDB): %s %-43s:[%3d] %-35s (%s%s)\n", prefix,
					((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) == (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) ? "IPvx" : (((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IP2LOCATION2_IPV6) != 0) ? "IPv6" : "IPv4"),
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename,
					libipv6calc_db_wrapper_IP2Location2_database_info(type, level_verbose, i, 0),
					(r == 3) ? " SOFTLINK" : ""
				);
				libipv6calc_db_wrapper_IP2Location2_close(type);
				count++;
			};
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				fprintf(stderr, "%sIP2Location(MMDB): %s %-43s:[%3d] %-35s (%s)\n", prefix,
					((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) == (IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IP2LOCATION2_IPV4)) ? "IPvx" : (((libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].features & IPV6CALC_DB_IP2LOCATION2_IPV6) != 0) ? "IPv6" : "IPv4"),
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number,
					libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename,
					strerror(errno)
				);
			};
			continue;
		};
	};

	if (count == 0) {
		fprintf(stderr, "%sIP2Location(MMDB): NO available databases found in directory: %s\n", prefix, ip2location2_db_dir);
	} else {
		if (level_verbose >= LEVEL_VERBOSE) {
			db_info_all.country4.num    = ip2location2_db_country_v4_best[IP2L_SAMPLE].num;
			db_info_all.country4.dbtype = ip2location2_db_country_v4_best[IP2L_SAMPLE].dbtype;
			db_info_all.country4.dbym   = ip2location2_db_country_v4_best[IP2L_SAMPLE].dbym;
			db_info_all.country6.num    = ip2location2_db_country_v6_best[IP2L_SAMPLE].num;
			db_info_all.country6.dbtype = ip2location2_db_country_v6_best[IP2L_SAMPLE].dbtype;
			db_info_all.country6.dbym   = ip2location2_db_country_v6_best[IP2L_SAMPLE].dbym;
			db_info_all.city4.num    = ip2location2_db_region_city_v4_best[IP2L_SAMPLE].num;
			db_info_all.city4.dbtype = ip2location2_db_region_city_v4_best[IP2L_SAMPLE].dbtype;
			db_info_all.city4.dbym   = ip2location2_db_region_city_v4_best[IP2L_SAMPLE].dbym;
			db_info_all.city6.num    = ip2location2_db_region_city_v6_best[IP2L_SAMPLE].num;
			db_info_all.city6.dbtype = ip2location2_db_region_city_v6_best[IP2L_SAMPLE].dbtype;
			db_info_all.city6.dbym   = ip2location2_db_region_city_v6_best[IP2L_SAMPLE].dbym;
			db_info_all.asn4.num    = ip2location2_db_asn_v4_best[IP2L_SAMPLE].num;
			db_info_all.asn4.dbtype = ip2location2_db_asn_v4_best[IP2L_SAMPLE].dbtype;
			db_info_all.asn4.dbym   = ip2location2_db_asn_v4_best[IP2L_SAMPLE].dbym;
			db_info_all.asn6.num    = ip2location2_db_asn_v6_best[IP2L_SAMPLE].num;
			db_info_all.asn6.dbtype = ip2location2_db_asn_v6_best[IP2L_SAMPLE].dbtype;
			db_info_all.asn6.dbym   = ip2location2_db_asn_v6_best[IP2L_SAMPLE].dbym;
			db_info_all.geonameid4.num    = ip2location2_db_geonameid_v4_best[IP2L_SAMPLE].num;
			db_info_all.geonameid4.dbtype = ip2location2_db_geonameid_v4_best[IP2L_SAMPLE].dbtype;
			db_info_all.geonameid4.dbym   = ip2location2_db_geonameid_v4_best[IP2L_SAMPLE].dbym;
			db_info_all.geonameid6.num    = ip2location2_db_geonameid_v6_best[IP2L_SAMPLE].num;
			db_info_all.geonameid6.dbtype = ip2location2_db_geonameid_v6_best[IP2L_SAMPLE].dbtype;
			db_info_all.geonameid6.dbym   = ip2location2_db_geonameid_v6_best[IP2L_SAMPLE].dbym;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "IP2Location(MMDB)", "detected best databases SAMPLE", 3, db_info_all, 0);

			db_info_all.country4.num    = ip2location2_db_country_v4_best[IP2L_LITE].num;
			db_info_all.country4.dbtype = ip2location2_db_country_v4_best[IP2L_LITE].dbtype;
			db_info_all.country4.dbym   = ip2location2_db_country_v4_best[IP2L_LITE].dbym;
			db_info_all.country6.num    = ip2location2_db_country_v6_best[IP2L_LITE].num;
			db_info_all.country6.dbtype = ip2location2_db_country_v6_best[IP2L_LITE].dbtype;
			db_info_all.country6.dbym   = ip2location2_db_country_v6_best[IP2L_LITE].dbym;
			db_info_all.city4.num    = ip2location2_db_region_city_v4_best[IP2L_LITE].num;
			db_info_all.city4.dbtype = ip2location2_db_region_city_v4_best[IP2L_LITE].dbtype;
			db_info_all.city4.dbym   = ip2location2_db_region_city_v4_best[IP2L_LITE].dbym;
			db_info_all.city6.num    = ip2location2_db_region_city_v6_best[IP2L_LITE].num;
			db_info_all.city6.dbtype = ip2location2_db_region_city_v6_best[IP2L_LITE].dbtype;
			db_info_all.city6.dbym   = ip2location2_db_region_city_v6_best[IP2L_LITE].dbym;
			db_info_all.asn4.num    = ip2location2_db_asn_v4_best[IP2L_LITE].num;
			db_info_all.asn4.dbtype = ip2location2_db_asn_v4_best[IP2L_LITE].dbtype;
			db_info_all.asn4.dbym   = ip2location2_db_asn_v4_best[IP2L_LITE].dbym;
			db_info_all.asn6.num    = ip2location2_db_asn_v6_best[IP2L_LITE].num;
			db_info_all.asn6.dbtype = ip2location2_db_asn_v6_best[IP2L_LITE].dbtype;
			db_info_all.asn6.dbym   = ip2location2_db_asn_v6_best[IP2L_LITE].dbym;
			db_info_all.geonameid4.num    = ip2location2_db_geonameid_v4_best[IP2L_LITE].num;
			db_info_all.geonameid4.dbtype = ip2location2_db_geonameid_v4_best[IP2L_LITE].dbtype;
			db_info_all.geonameid4.dbym   = ip2location2_db_geonameid_v4_best[IP2L_LITE].dbym;
			db_info_all.geonameid6.num    = ip2location2_db_geonameid_v6_best[IP2L_LITE].num;
			db_info_all.geonameid6.dbtype = ip2location2_db_geonameid_v6_best[IP2L_LITE].dbtype;
			db_info_all.geonameid6.dbym   = ip2location2_db_geonameid_v6_best[IP2L_LITE].dbym;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "IP2Location(MMDB)", "detected best databases LITE", 3, db_info_all, 0);

			db_info_all.country4.num    = ip2location2_db_country_v4_best[IP2L_COMM].num;
			db_info_all.country4.dbtype = ip2location2_db_country_v4_best[IP2L_COMM].dbtype;
			db_info_all.country4.dbym   = ip2location2_db_country_v4_best[IP2L_COMM].dbym;
			db_info_all.country6.num    = ip2location2_db_country_v6_best[IP2L_COMM].num;
			db_info_all.country6.dbtype = ip2location2_db_country_v6_best[IP2L_COMM].dbtype;
			db_info_all.country6.dbym   = ip2location2_db_country_v6_best[IP2L_COMM].dbym;
			db_info_all.city4.num    = ip2location2_db_region_city_v4_best[IP2L_COMM].num;
			db_info_all.city4.dbtype = ip2location2_db_region_city_v4_best[IP2L_COMM].dbtype;
			db_info_all.city4.dbym   = ip2location2_db_region_city_v4_best[IP2L_COMM].dbym;
			db_info_all.city6.num    = ip2location2_db_region_city_v6_best[IP2L_COMM].num;
			db_info_all.city6.dbtype = ip2location2_db_region_city_v6_best[IP2L_COMM].dbtype;
			db_info_all.city6.dbym   = ip2location2_db_region_city_v6_best[IP2L_COMM].dbym;
			db_info_all.asn4.num    = ip2location2_db_asn_v4_best[IP2L_COMM].num;
			db_info_all.asn4.dbtype = ip2location2_db_asn_v4_best[IP2L_COMM].dbtype;
			db_info_all.asn4.dbym   = ip2location2_db_asn_v4_best[IP2L_COMM].dbym;
			db_info_all.asn6.num    = ip2location2_db_asn_v6_best[IP2L_COMM].num;
			db_info_all.asn6.dbtype = ip2location2_db_asn_v6_best[IP2L_COMM].dbtype;
			db_info_all.asn6.dbym   = ip2location2_db_asn_v6_best[IP2L_COMM].dbym;
			db_info_all.geonameid4.num    = ip2location2_db_geonameid_v4_best[IP2L_COMM].num;
			db_info_all.geonameid4.dbtype = ip2location2_db_geonameid_v4_best[IP2L_COMM].dbtype;
			db_info_all.geonameid4.dbym   = ip2location2_db_geonameid_v4_best[IP2L_COMM].dbym;
			db_info_all.geonameid6.num    = ip2location2_db_geonameid_v6_best[IP2L_COMM].num;
			db_info_all.geonameid6.dbtype = ip2location2_db_geonameid_v6_best[IP2L_COMM].dbtype;
			db_info_all.geonameid6.dbym   = ip2location2_db_geonameid_v6_best[IP2L_COMM].dbym;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "IP2Location(MMDB)", "detected best databases COMM", 3, db_info_all, 0);

			db_info_all.country4.num    = ip2location2_db_country_sample_v4_lite_autoswitch;
			db_info_all.country6.num    = ip2location2_db_country_sample_v6_lite_autoswitch;
			db_info_all.city4.num    = ip2location2_db_region_city_sample_v4_lite_autoswitch;
			db_info_all.city6.num    = ip2location2_db_region_city_sample_v6_lite_autoswitch;
			db_info_all.asn4.num    = ip2location2_db_asn_sample_v4_lite_autoswitch;
			db_info_all.asn6.num    = ip2location2_db_asn_sample_v6_lite_autoswitch;
			db_info_all.geonameid4.num    = ip2location2_db_geonameid_sample_v4_lite_autoswitch;
			db_info_all.geonameid6.num    = ip2location2_db_geonameid_sample_v6_lite_autoswitch;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "IP2Location(MMDB)", "selected best databases LI->SA*", 3, db_info_all, 0);

			db_info_all.country4.num   = ip2location2_db_country_v4;
			db_info_all.country6.num   = ip2location2_db_country_v6;
			db_info_all.city4.num      = ip2location2_db_region_city_v4;
			db_info_all.city6.num      = ip2location2_db_region_city_v6;
			db_info_all.asn4.num       = ip2location2_db_asn_v4;
			db_info_all.asn6.num       = ip2location2_db_asn_v6;
			db_info_all.geonameid4.num = ip2location2_db_geonameid_v4;
			db_info_all.geonameid6.num = ip2location2_db_geonameid_v6;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "IP2Location(MMDB)", "selected best databases normal", 3, db_info_all, 1);

			if (ip2location2_db_lite_to_sample_autoswitch_max_delta_months > 0) {
				fprintf(stderr, "%sIP2Location(MMDB): selected best databases method : * = autoswitch from LITE to SAMPLE enabled in case not older than %d months and having more features\n"
					, prefix
					, ip2location2_db_lite_to_sample_autoswitch_max_delta_months
				);
			};

			if (ip2location2_db_comm_to_lite_switch_min_delta_months > 0) {
				fprintf(stderr, "%sIP2Location(MMDB): selected best databases method : COMM older than %d months are deselected in case of LITE is available\n"
					, prefix
					, ip2location2_db_comm_to_lite_switch_min_delta_months
				);
			};

			if (ip2location2_db_better_max_delta_months > 0) {
				fprintf(stderr, "%sIP2Location(MMDB): selected best databases method : COMM/LITE/SAMPLE with more features are only selected in case not older than %d months of already found COMM/LITE/SAMPLE\n"
					, prefix
					, ip2location2_db_better_max_delta_months
				);
			};

			if (ip2location2_db_only_type > 0) {
				fprintf(stderr, "%sIP2Location(MMDB): selected best databases method : by applying given DB type filter: %d\n"
					, prefix
					, ip2location2_db_only_type
				);
			};

			fprintf(stderr, "%sIP2Location(MMDB): selected best databases method : softlinks %s\n"
				, prefix
				, (ip2location2_db_allow_softlinks == 0) ? "skipped-by-default" : "allowed-by-option"
			);

		};

	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_IP2Location2_wrapper_db_info_used(void) {
	unsigned int db;
	char tempstring[IPV6CALC_STRING_MAX] = "";
	char *info;

	unsigned int db_lite_used = 0;

	for (db = 0; db < 32 * IP2LOCATION2_DB_MAX_BLOCKS_32; db++) {
		if ((ip2location2_db_usage_map[db / 32] & (1 << (db % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "DB used: %d", db);

			// workaround to get lite info
			int  entry = -1, i;
			for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
				if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == db) {
					entry = i;
					break;
				};
			};

			if (entry < 0) {
				// should not happen
				continue;
			};

			info = libipv6calc_db_wrapper_IP2Location2_database_info(db, 0, entry, 1);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].internal & IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE) {
				db_lite_used = 1;
			};

			STRCLR(tempstring);
			if (strlen(ip2location2_db_usage_string) > 0) {
				if (strstr(ip2location2_db_usage_string, info) != NULL) { continue; }; // string already included

				STRCAT(tempstring, ip2location2_db_usage_string);
				STRCAT(tempstring, " / ");
			};
			STRCAT(tempstring, info);

			snprintf(ip2location2_db_usage_string, sizeof(ip2location2_db_usage_string), "%s", tempstring);
		};
	};

	if (db_lite_used == 1) {
		STRCLR(tempstring);
		STRCAT(tempstring, ip2location2_db_usage_string);
		STRCAT(tempstring, " / This site or product includes IP2Location LITE data available from https://lite.ip2location.com");
		snprintf(ip2location2_db_usage_string, sizeof(ip2location2_db_usage_string), "%s", tempstring);
	};

	return(ip2location2_db_usage_string);
};



/*******************************
 * Wrapper extension functions for IP2Location
 * because of missing support in library itself
 *******************************/

/*
 * wrapper extension: IP2Location2_dbfilename
 */
static char *libipv6calc_db_wrapper_IP2Location2_dbfilename(const unsigned int type) {
	static char tempstring[IPV6CALC_STRING_MAX];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called: %s type=%d", wrapper_ip2location2_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	STRCLR(tempstring);
	STRCAT(tempstring, ip2location2_db_dir);
	STRCAT(tempstring, "/");
	STRCAT(tempstring, libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].filename);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d has filename=%s", wrapper_ip2location2_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: IP2Location2_dbdescription
 */
const char *libipv6calc_db_wrapper_IP2Location2_dbdescription(const unsigned int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called: %s type=%d", wrapper_ip2location2_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d has description=%s", wrapper_ip2location2_info, type, libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].description);
};


/*
 * wrapper extension: IP2Location2_db_avail
 * ret: 1=avail  0=not-avail 2=softlink (in case not allowed) 3=softlink (allowed)
 */
int libipv6calc_db_wrapper_IP2Location2_db_avail(const unsigned int type) {
	char *filename;
	int r, result = 0;
	struct stat st;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called: %s type=%d", wrapper_ip2location2_info, type);

	filename = libipv6calc_db_wrapper_IP2Location2_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	if (access(filename, R_OK) != 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d (not readable) (%s)", wrapper_ip2location2_info, type, strerror(errno));
		goto END_libipv6calc_db_wrapper;
	};

	r = lstat(filename, &st);
	if (r != 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d (lstat problem) (r=%d)", wrapper_ip2location2_info, type, r);
		goto END_libipv6calc_db_wrapper;
	};

	if (S_ISLNK(st.st_mode)) {
		if (ip2location2_db_allow_softlinks == 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d (unsupported softlink)", wrapper_ip2location2_info, type);
			result = 2;
			goto END_libipv6calc_db_wrapper;
		} else {
			result = 3;
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished: %s type=%d (OK, result=%d)", wrapper_ip2location2_info, type, result);
	result = 1;

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * wrapper extension: IP2Location2_open_type
 * input:
 * 	type (mandatory)
 * modified:
 * 	mmdb_cache
 * output:
 * 	result
 */
int libipv6calc_db_wrapper_IP2Location2_open_type(const unsigned int type) {
	char *filename;
	int  entry = -1, i;
	int ret;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called: %s type=%d", wrapper_ip2location2_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	if (mmdb_cache[type].file_size > 0) {
		// already open
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Database already opened (cached) type=%d", type);
		ret = MMDB_SUCCESS;
		goto END_libipv6calc_db_wrapper;
	};

	// retrieve filename
	filename = libipv6calc_db_wrapper_IP2Location2_dbfilename(type);

	if (filename == NULL) {
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	ret = libipv6calc_db_wrapper_MMDB_open(filename, MMDB_MODE_MMAP, &mmdb_cache[type]);

	if (ret != MMDB_SUCCESS) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "MMDB_open failed: %s (%s)\n", libipv6calc_db_wrapper_MMDB_strerror(ret), filename);
			if (ret == MMDB_IO_ERROR) {
				fprintf(stderr, "MMDB_open IO error: %s (%s)\n", strerror(errno), filename);
			};
		};
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(ret);
};


/*******************************
 * Wrapper functions for IP2Location
 *******************************/

/*
 * wrapper: IP2Location2_database_info
 * in:
 * flag_copyright: 0|1 -> append copyright string
 */
char *libipv6calc_db_wrapper_IP2Location2_database_info(const unsigned int type, const int level_verbose, const int entryold, const int flag_copyright) {
	static char resultstring[IPV6CALC_STRING_MAX2] = "";
	char datastring[IPV6CALC_STRING_MAX];

	//uint32_t entries_ipv4 = 0;
	//uint32_t entries_ipv6 = 0;
	//
	/* TODO remove/check */
	if (level_verbose > 0)  { };
	if (entryold > 0)  { };

	MMDB_s mmdb;
	int ret, i, entry = -1;


	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called: %s", wrapper_ip2location2_info);

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_IP2Location2_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		ERRORPRINT_WA("Invalid type (FIX CODE): %d", type);
		goto END_libipv6calc_db_wrapper;
	};


	ret = libipv6calc_db_wrapper_IP2Location2_open_type(type);

	if (ret != MMDB_SUCCESS) {
		snprintf(resultstring, sizeof(resultstring), "%s", "(CAN'T OPEN database information)");
		goto END_libipv6calc_db_wrapper;
	};

	// get metadata
	//
/*
 const char *meta_dump = "\n"
                            "  Database metadata\n"
                            "    Node count:    %i\n"
                            "    Record size:   %i bits\n"
                            "    IP version:    IPv%i\n"
                            "    Binary format: %i.%i\n"
                            "    Build epoch:   %llu (%s)\n"
                            "    Type:          %s\n"
                            "    Languages:     ";

    fprintf(stdout, meta_dump,
*/

	mmdb = mmdb_cache[type];

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "meta: NodeCount=%i RecordSize=%i IpVersion=%i BinaryFormat=%i.%i BuildTime=%llu Type=%s", \
            mmdb.metadata.node_count,
            mmdb.metadata.record_size,
            mmdb.metadata.ip_version,
            mmdb.metadata.binary_format_major_version,
            mmdb.metadata.binary_format_minor_version,
            (long long unsigned int) mmdb.metadata.build_epoch,
            mmdb.metadata.database_type);

	const time_t epoch = (const time_t)mmdb.metadata.build_epoch;

	char year[5];
	strftime(year, sizeof(year), "%Y", gmtime(&epoch));
	if (flag_copyright != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s Copyright (c) %s IP2Location All Rights Reserved"
			, mmdb.metadata.database_type
			, year
		);
	} else {
		snprintf(resultstring, sizeof(resultstring), "%s"
			, mmdb.metadata.database_type
		);
	};

	strftime(datastring, sizeof(datastring), "%Y%m%d-%H%M%S UTC", gmtime(&epoch));
	STRCAT(resultstring, ", created: ");
	STRCAT(resultstring, datastring);

END_libipv6calc_db_wrapper:
	return(resultstring);
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_IP2Location2_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Return with result: %d", result);
	return(result);
};


/* country_code */
int libipv6calc_db_wrapper_IP2Location2_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len) {
	int result = MMDB_INVALID_DATA_ERROR;

	unsigned int IP2Location2_type = 0;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		IP2Location2_type = ip2location2_db_country_v4;

		if ((ip2location2_db_country_sample_v4_lite_autoswitch > 0) && (ip2location2_db_country_v4_best[IP2L_COMM].num != IP2Location2_type)) {
			// lite database selected, sample database available (supporting 0.0.0.0-99.255.255.255)
			CONVERT_IPADDRP_IPV4ADDR(ipaddrp, ipv4addr)
			if (ipv4addr_getoctet(&ipv4addr, 0) <= 99) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Overwrite IP2Location2_type LITE %d with SAMPLE DB %d", IP2Location2_type, ip2location2_db_country_sample_v4_lite_autoswitch);
				IP2Location2_type = ip2location2_db_country_sample_v4_lite_autoswitch;
			};
		};

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv4 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		IP2Location2_type = ip2location2_db_country_v6;

		if ((ip2location2_db_country_sample_v6_lite_autoswitch > 0) && (ip2location2_db_country_v6_best[IP2L_COMM].num != IP2Location2_type)) {
			// lite database selected, sample database available (supporting 2A04:0:0:0:0:0:0:0-2A04:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF)
			CONVERT_IPADDRP_IPV6ADDR(ipaddrp, ipv6addr)
			if (ipv6addr_getword(&ipv6addr, 0) == 0x2a04) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Overwrite IP2Location2_type LITE %d with SAMPLE DB %d", IP2Location2_type, ip2location2_db_country_sample_v6_lite_autoswitch);
				IP2Location2_type = ip2location2_db_country_sample_v6_lite_autoswitch;
			};
		};

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv6 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_IP2Location2_open_type(IP2Location2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location(MMDB) by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_country_code_by_addr(ipaddrp, country, country_len, &mmdb_cache[IP2Location2_type]);
 
	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "result country=%s", country);

	IP2LOCATION2_DB_USAGE_MAP_TAG(IP2Location2_type);

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * get AS 32-bit number and orgname (optional)
 * in : ipaddrp (mandatory)
 * in : data_source_ptr (optional, can be NULL)
 * out: as_orgname (optional if not NULL and as_orgname_length != 0)
 * in : as_orgname_length (optional required != 0)
 * out: AS 32-bit number
 */
uint32_t libipv6calc_db_wrapper_IP2Location2_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length) {
	uint32_t as_num = ASNUM_AS_UNKNOWN;
	int result;

	unsigned int IP2Location2_type = 0;
	libipv6calc_db_wrapper_geolocation_record record_asn;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		IP2Location2_type = ip2location2_db_asn_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv4 ASN");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		IP2Location2_type = ip2location2_db_asn_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv6 ASN");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	if (IP2Location2_type == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database selected for proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_IP2Location2_open_type(IP2Location2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location(MMDB) by type");
		goto END_libipv6calc_db_wrapper;
	};

	// AS Number
	as_num = libipv6calc_db_wrapper_MMDB_asn_by_addr(ipaddrp, &mmdb_cache[IP2Location2_type]);
 
	if (as_num == ASNUM_AS_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "result ASN=%u", as_num);

	// AS Text (optional)
	if ((as_orgname != NULL) && (as_orgname_length > 0)) {
		result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, &record_asn, &mmdb_cache[IP2Location2_type]);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "no match found");
			goto END_libipv6calc_db_wrapper;
		};

		// copy information
		snprintf(as_orgname, as_orgname_length, "%s", record_asn.organization_name);

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "result AS_ORGNAME=%s", as_orgname);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "lookup AS_ORGNAME skipped");
	};

	IP2LOCATION2_DB_USAGE_MAP_TAG(IP2Location2_type);

END_libipv6calc_db_wrapper:
	return(as_num);
};


/* GeonameID */
uint32_t libipv6calc_db_wrapper_IP2Location2_wrapper_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, int *source_ptr) {
	uint32_t result = IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN;

	int IP2Location2_type = 0;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		IP2Location2_type = ip2location2_db_geonameid_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV4_TO_GEONAMEID) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv4 GeonameID");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		IP2Location2_type = ip2location2_db_geonameid_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION2] & IPV6CALC_DB_IPV6_TO_GEONAMEID) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "No IP2Location(MMDB) database supporting IPv6 GeonameID");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_IP2Location2_open_type(IP2Location2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location(MMDB) by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_GeonameID_by_addr(ipaddrp, &mmdb_cache[IP2Location2_type], source_ptr);

	if (result == IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "result GeonameID=%u", result);

	IP2LOCATION2_DB_USAGE_MAP_TAG(IP2Location2_type);

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * return UsageType description related to the from database returned token
 *
 * in : UsageType token
 * out: UsageType description
 */
extern const char *libipv6calc_db_wrapper_IP2Location2_UsageType_description(char *UsageType) {
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_UsageType); i++) {
		if (strcmp(UsageType, libipv6calc_db_wrapper_IP2Location2_UsageType[i].tokensimple) == 0) {
			return(libipv6calc_db_wrapper_IP2Location2_UsageType[i].token);
		};
	};

	return(NULL);
};


/* all information */
int libipv6calc_db_wrapper_IP2Location2_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp) {
	int result = 0;

	int IP2Location2_type = 0;
	int IP2Location2_type_asn = 0;

	libipv6calc_db_wrapper_geolocation_record record_asn;

	libipv6calc_db_wrapper_geolocation_record_clear(recordp);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		IP2Location2_type = ip2location2_db_region_city_v4;
		IP2Location2_type_asn = ip2location2_db_asn_v4;

		if (IP2Location2_type == 0) {
			// fallback
			IP2Location2_type = ip2location2_db_country_v4;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		IP2Location2_type = ip2location2_db_region_city_v6;
		IP2Location2_type_asn = ip2location2_db_asn_v6;

		if (IP2Location2_type == 0) {
			// fallback
			IP2Location2_type = ip2location2_db_country_v6;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_IP2Location2_open_type(IP2Location2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location(MMDB) by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, recordp, &mmdb_cache[IP2Location2_type]);

	// ASN is stored potentially in a different database
	if (IP2Location2_type_asn > 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "fetch ASN information from dedicated database");
		result = libipv6calc_db_wrapper_IP2Location2_open_type(IP2Location2_type_asn);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Error opening IP2Location2 by type");
			goto END_libipv6calc_db_wrapper;
		};

		result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, &record_asn, &mmdb_cache[IP2Location2_type_asn]);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "no match found");
			goto END_libipv6calc_db_wrapper;
		};

		// copy information
		recordp->asn = record_asn.asn;
                snprintf(recordp->organization_name, IPV6CALC_DB_SIZE_ORG_NAME, "%s", record_asn.organization_name);

		IP2LOCATION2_DB_USAGE_MAP_TAG(IP2Location2_type_asn);
	};

	IP2LOCATION2_DB_USAGE_MAP_TAG(IP2Location2_type);

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * function cleanup the IP2Location(MMDB) wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_IP2Location2_wrapper_cleanup(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Called");

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location2_db_file_desc); i++) {
		libipv6calc_db_wrapper_IP2Location2_close(i);
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_IP2Location2, "Finished");
	return 0;
};


#endif // SUPPORT_IP2LOCATION2
