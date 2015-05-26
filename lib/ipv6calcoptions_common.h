/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions_common.h,v 1.17 2015/05/26 15:50:04 ds6peter Exp $
 * Copyright  : 2013-2015 by Peter Bieringer <pb (at) bieringer.de>
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
static char *ipv6calc_shortopts_common = "d:qV";

/* define common long options */
static struct option ipv6calc_longopts_common[] = {
	{"debug"  , 1, NULL, (int) 'd' },
	{"quiet"  , 0, NULL, (int) 'q' },
	{"verbose", 0, NULL, (int) 'V' },
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
	{"disable-ip2location"         , 0, NULL, DB_ip2location_disable },
	{"db-ip2location-disable"      , 0, NULL, DB_ip2location_disable },
	{"db-ip2location-dir"          , 1, NULL, DB_ip2location_dir     },

	/* database selection control */
	{"db-ip2location-lite-to-sample-autoswitch-max-delta-months", 1, NULL, DB_ip2location_lite_to_sample_autoswitch_max_delta_months },
	{"db-ip2location-comm-to-lite-switch-min-delta-months", 1, NULL, DB_ip2location_comm_to_lite_switch_min_delta_months },
	{"db-ip2location-only-type", 1, NULL, DB_ip2location_only_type },
	{"db-ip2location-allow-softlinks", 0, NULL, DB_ip2location_allow_softlinks },

#ifdef SUPPORT_IP2LOCATION_DYN
	{"db-ip2location-lib"          , 1, NULL, DB_ip2location_lib     },
#endif // SUPPORT_IP2LOCATION_DYN
};
#endif // SUPPORT_IP2LOCATION

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
	{"disable-geoip"               , 0, NULL, DB_geoip_disable       },
	{"db-geoip-disable"            , 0, NULL, DB_geoip_disable       },
	{"db-geoip-dir"                , 1, NULL, DB_geoip_dir           },
#ifdef SUPPORT_GEOIP_DYN
	{"db-geoip-lib"                , 1, NULL, DB_geoip_lib           },
#endif // SUPPORT_GEOIP_DYN
};
#endif // SUPPORT_GEOIP

#ifdef SUPPORT_DBIP
static char *ipv6calc_shortopts_dbip = "";

static struct option ipv6calc_longopts_dbip[] = {
	{"disable-dbip"                , 0, NULL, DB_dbip_disable       },
	{"db-dbip-disable"             , 0, NULL, DB_dbip_disable       },
	{"db-dbip-dir"                 , 1, NULL, DB_dbip_dir           },
};
#endif // SUPPORT_DBIP

#ifdef SUPPORT_EXTERNAL
static char *ipv6calc_shortopts_external = "";

static struct option ipv6calc_longopts_external[] = {
	{"disable-external"            , 0, NULL, DB_external_disable   },
	{"db-external-disable"         , 0, NULL, DB_external_disable   },
	{"db-external-dir"             , 1, NULL, DB_external_dir       },
};
#endif // SUPPORT_EXTERNAL

#ifdef SUPPORT_BUILTIN
static char *ipv6calc_shortopts_builtin = "";

static struct option ipv6calc_longopts_builtin[] = {
	{"disable-builtin"             , 0, NULL, DB_builtin_disable    },
	{"db-builtin-disable"          , 0, NULL, DB_builtin_disable    },
};
#endif // SUPPORT_BUILTIN

#if defined SUPPORT_EXTERNAL || defined SUPPORT_DBIP || defined SUPPORT_GEOIP || SUPPORT_IP2LOCATION
static char *ipv6calc_shortopts_db_common = "";

static struct option ipv6calc_longopts_db_common[] = {
	{"db-priorization"             , 1, NULL, DB_common_priorization },
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
	{ "ap"			, 1, NULL, CMD_ANON_PRESET_OPTION   },
	{ "anonymize-method"	, 1, NULL, CMD_ANON_METHOD_OPTION   },
	{ "mask-ipv4"		, 1, NULL, CMD_ANON_MASK_IPV4 },
	{ "mask-ipv6"		, 1, NULL, CMD_ANON_MASK_IPV6 },
	{ "mask-iid"		, 1, NULL, CMD_ANON_MASK_IID },
	{ "mask-eui64"		, 1, NULL, CMD_ANON_MASK_EUI64 },
	{ "mask-mac"		, 1, NULL, CMD_ANON_MASK_MAC },
	{ "mask-autoadjust"	, 1, NULL, CMD_ANON_MASK_AUTOADJUST },
};
