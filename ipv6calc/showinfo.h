/*
 * Project    : ipv6calc
 * File       : showinfo.h
 * Version    : $Id$
 * Copyright  : 2001-2017 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for showinfo.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libmac.h"

#define DEBUG_showinfo		0x00000002l

/* output version of machine readable format */
// 1: never defined, until 0.51
// 2: 0.60.0cvs to 0.60.1
// 3: 0.61.0cvs to 0.70.0
// 4: 0.71.0cvs to < 0.92.0
//	- IP2LOCATION_DATABASE_INFO
//	+ IP2LOCATION_DATABASE_INFO_IPV4
//	+ IP2LOCATION_DATABASE_INFO_IPV6 
// 5: 0.92.0 to < 0.94.0
//	+ IPV6CALC_FEATURES (see ipv6calc -v)
// 6: 0.94.0 to < 0.95.0
//      TYPE renamed to IPV6_TYPE for alignment
//      + IPV6_ANON
//      + IPV4_ANON
//      + SETTINGS_ANON
// 7: 0.95.0 to 0.97.3
//      - GEOIP_DATABASE_INFO_IPV4
//      - GEOIP_DATABASE_INFO_IPV6
//      + GEOIP_DATABASE_INFO
//      - IP2LOCATION_DATABASE_INFO_IPV4
//      - IP2LOCATION_INFO_IPV6
//      + IP2LOCATION_DATABASE_INFO
// 8: 0.97.4
//      + IPV6CALC_CAPABILITIES
// 9: 0.98.0
//      + DBIP_COUNTRY_SHORT
//      + DBIP_CITY
//      + DBIP_DATABASE_INFO
//      + IPV4_COUNTRYCODE_SOURCE
//      + IPV6_COUNTRYCODE_SOURCE
//      + GEOP_AREACODE
//      + GEOP_DMACODE
//      + IP2LOCATION_AREACODE
//      + EXTERNAL_COUNTRY_SHORT
//      + EXTERNAL_DATABASE_INFO
//      + BUILTIN_DATABASE_INFO
// 10: 0.99.0 to current
// 	+ IP2LOCATION_TIMEZONE
// 	+ IP2LOCATION_NETSPEED
// 	+ IP2LOCATION_WEATHERSTATIONNAME
// 	+ IP2LOCATION_WEATHERSTATIONCODE
// 	+ IP2LOCATION_ELEVATION
// 	+ IP2LOCATION_USAGETYPE
// 	+ IP2LOCATION_MOBILEBRAND
// 	+ IP2LOCATION_MNC
// 	+ IP2LOCATION_MCC
// 	+ IP2LOCATION_IDDCODE
// 11: 0.99.2 to current
// 	+ DBIP_DISTRICT
// 	+ DBIP_ZIPCODE
// 	+ DBIP_GEONAME_ID
// 	+ DBIP_LATITUDE
// 	+ DBIP_LONGITUDE
// 	+ DBIP_TIMEZONE
// 	+ DBIP_TIMEZONE_NAME
// 	+ DBIP_NETSPEED
// 	+ DBIP_ISP
// 	+ DBIP_ORGNAME
// (inbetween)
//      + <geolocation provider>_AS_NUM
//      + <geolocation provider>_AS_TEXT
//      + <geolocation provider>_CONTINENT_SHORT
// 20: 2.0.0 to current
//      rename ORGNAME to AS_ORGNAME
//      remove AS_TEXT
//      + <geolocation provider>_GEONAME_ID_COUNTRY
//      + <geolocation provider>_GEONAME_ID_CONTINENT
//      + <geolocation provider>_RADIUS
//      + <geolocation provider>_ELEVATION
//      + <geolocation provider>_CONTINENT_LONG

#define IPV6CALC_OUTPUT_VERSION	20

/* prototypes */
extern int  showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t formatoptions);
extern int  showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions);
extern int  showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions);
extern int  showinfo_eui64(const ipv6calc_eui64addr *eui64addrp, const uint32_t formatoptions);
extern int  showinfo_asn(const uint32_t asn, const uint32_t formatoptions);
extern void showinfo_availabletypes(void);
