/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions_common.h,v 1.8 2013/11/10 18:20:53 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options
 */ 

#include <getopt.h> 
#include <stdio.h>


/* Options */

/* define short options
 *  d <debugvalue> = debug value
 */

/* define common short options */
static char *ipv6calc_shortopts_common = "d:q";

/* define common long options */
static struct option ipv6calc_longopts_common[] = {
	{"debug"  , 1, NULL, (int) 'd' },
	{"quiet"  , 0, NULL, (int) 'q' },
};

#ifdef SUPPORT_IP2LOCATION
static char *ipv6calc_shortopts_ip2location = "L";

static struct option ipv6calc_longopts_ip2location[] = {
	/* database options (old) */
	{"db-ip2location-ipv4"         , 1, NULL, DB_ip2location_ipv4   },
	{"db-ip2location-ipv6"         , 1, NULL, DB_ip2location_ipv6   },
	{"db-ip2location-default"      , 0, NULL, (int) 'L'             },
	{"db-ip2location-ipv4-default" , 0, NULL, DB_geoip_ipv4_default },
	{"db-ip2location-ipv6-default" , 0, NULL, DB_geoip_ipv6_default },

	/* database options (new) */
	{"db-ip2location-disable"      , 0, NULL, DB_ip2location_disable },
	{"db-ip2location-dir"          , 1, NULL, DB_ip2location_dir     },
	{"db-ip2location-lib"          , 1, NULL, DB_ip2location_lib     },
};
#endif

#ifdef SUPPORT_GEOIP
static char *ipv6calc_shortopts_geoip = "G";

static struct option ipv6calc_longopts_geoip[] = {
	/* database options (old) */
	{"db-geoip"                    , 1, NULL, DB_geoip_ipv4         }, // backward compatibility
	{"db-geoip-ipv4"               , 1, NULL, DB_geoip_ipv4         },
	{"db-geoip-ipv6"               , 1, NULL, DB_geoip_ipv6         },
	{"db-geoip-default"            , 0, NULL, (int) 'G'             },
	{"db-geoip-ipv4-default"       , 0, NULL, DB_geoip_ipv4_default },
	{"db-geoip-ipv6-default"       , 0, NULL, DB_geoip_ipv6_default },

	/* database options (new) */
	{"db-geoip-disable"            , 0, NULL, DB_geoip_disable       },
	{"db-geoip-dir"                , 1, NULL, DB_geoip_dir           },
	{"db-geoip-lib"                , 1, NULL, DB_geoip_lib           },
};                
#endif


/* define common anonymization short options */
static char *ipv6calc_shortopts_common_anon = "";

/* define common anonymization long options */
static struct option ipv6calc_longopts_common_anon[] = {
	{ "anonymize-standard"	, 0, NULL, CMD_ANON_PRESET_STANDARD },
	{ "anonymize-careful"	, 0, NULL, CMD_ANON_PRESET_CAREFUL  },
	{ "anonymize-paranoid"	, 0, NULL, CMD_ANON_PRESET_PARANOID },
	{ "anonymize-preset"	, 1, NULL, CMD_ANON_PRESET_OPTION   },
	{ "anonymize-method"	, 1, NULL, CMD_ANON_METHOD_OPTION   },
	{ "mask-ipv4"		, 1, NULL, CMD_ANON_MASK_IPV4 },
	{ "mask-ipv6"		, 1, NULL, CMD_ANON_MASK_IPV6 },
	{ "mask-iid"		, 1, NULL, CMD_ANON_MASK_IID },
	{ "mask-eui64"		, 1, NULL, CMD_ANON_MASK_EUI64 },
	{ "mask-mac"		, 1, NULL, CMD_ANON_MASK_MAC },
	{ "mask-autoadjust"	, 1, NULL, CMD_ANON_MASK_AUTOADJUST },
};
