/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id$
 * Copyright  : 2013-2020 by Peter Bieringer <pb (at) bieringer.de>
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

	/* dummy catch if support is not compiled in */
#ifndef SUPPORT_IP2LOCATION
	{"disable-ip2location"         , 0, NULL, OPTION_NOOP },
	{"db-ip2location-disable"      , 0, NULL, OPTION_NOOP },
#endif

	// catch EOL GeoIP(legacy) options, TODO remove in 4.0.0
	{"disable-geoip"               , 0, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-disable"            , 0, NULL, OPTION_GEOIP_EOL },
	{"db-geoip"                    , 1, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-ipv4"               , 1, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-ipv6"               , 1, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-default"            , 0, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-ipv4-default"       , 0, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-ipv6-default"       , 0, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-dir"                , 1, NULL, OPTION_GEOIP_EOL },
	{"db-geoip-lib"                , 1, NULL, OPTION_GEOIP_EOL },

#ifndef SUPPORT_GEOIP2
	{"disable-geoip2"              , 0, NULL, OPTION_NOOP },
	{"db-geoip2-disable"           , 0, NULL, OPTION_NOOP },
#endif

	// catch EOL dbip.com(BerkeleyDB) options, TODO remove in 4.0.0
	{"disable-dbip"                , 0, NULL, OPTION_DBIP_EOL },
	{"db-dbip-disable"             , 0, NULL, OPTION_DBIP_EOL },
	{"db-dbip-dir"                 , 1, NULL, OPTION_DBIP_EOL },
	{"db-dbip-comm-to-free-switch-min-delta-months", 1, NULL, OPTION_DBIP_EOL },
	{"db-dbip-only-type"           , 1, NULL, OPTION_DBIP_EOL },

#ifndef SUPPORT_DBIP2
	{"disable-dbip2"               , 0, NULL, OPTION_NOOP },
	{"db-dbip2-disable"            , 0, NULL, OPTION_NOOP },
#endif

#ifndef SUPPORT_EXTERNAL
	{"disable-external"            , 0, NULL, OPTION_NOOP },
	{"db-external-disable"         , 0, NULL, OPTION_NOOP },
#endif

#ifndef SUPPORT_BUILTIN
	{"disable-builtin"             , 0, NULL, OPTION_NOOP },
	{"db-builtin-disable"          , 0, NULL, OPTION_NOOP },
#endif
};

#ifdef SUPPORT_IP2LOCATION
static char *ipv6calc_shortopts_ip2location = "L";

static struct option ipv6calc_longopts_ip2location[] = {
	/* database options (EOS) */
	{"db-ip2location-ipv4"         , 1, NULL, OPTION_IP2LOCATION_EOS},
	{"db-ip2location-ipv6"         , 1, NULL, OPTION_IP2LOCATION_EOS},
	{"db-ip2location-default"      , 0, NULL, OPTION_IP2LOCATION_EOS},
	{"db-ip2location-ipv4-default" , 0, NULL, OPTION_IP2LOCATION_EOS},
	{"db-ip2location-ipv6-default" , 0, NULL, OPTION_IP2LOCATION_EOS},

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

#ifdef SUPPORT_GEOIP2
static char *ipv6calc_shortopts_geoip2 = "";

static struct option ipv6calc_longopts_geoip2[] = {
	{"disable-geoip2"              , 0, NULL, DB_geoip2_disable       },
	{"db-geoip2-disable"           , 0, NULL, DB_geoip2_disable       },
	{"db-geoip2-dir"               , 1, NULL, DB_geoip2_dir           },
};
#endif // SUPPORT_GEOIP2

#ifdef SUPPORT_MMDB
static char *ipv6calc_shortopts_mmdb = "M";

static struct option ipv6calc_longopts_mmdb[] = {
	/* database options */
	{"disable-mmdb"               , 0, NULL, DB_mmdb_disable       },
	{"db-mmdb-disable"            , 0, NULL, DB_mmdb_disable       },
#ifdef SUPPORT_MMDB_DYN
	{"db-mmdb-lib"                , 1, NULL, DB_mmdb_lib           },
#endif // SUPPORT_MMDB_DYN
};
#endif // SUPPORT_MMDB

#ifdef SUPPORT_DBIP2
static char *ipv6calc_shortopts_dbip2 = "";

static struct option ipv6calc_longopts_dbip2[] = {
	{"disable-dbip2"               , 0, NULL, DB_dbip2_disable       },
	{"db-dbip2-disable"             , 0, NULL, DB_dbip2_disable       },
	{"db-dbip2-dir"                 , 1, NULL, DB_dbip2_dir           },
	{"db-dbip2-comm-to-free-switch-min-delta-months", 1, NULL, DB_dbip2_comm_to_free_switch_min_delta_months },
	{"db-dbip2-only-type", 1, NULL, DB_dbip2_only_type },
};
#endif // SUPPORT_DBIP2

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

#if defined SUPPORT_EXTERNAL || defined SUPPORT_DBIP || defined SUPPORT_GEOIP || SUPPORT_IP2LOCATION || defined SUPPORT_MMDB || defined SUPPORT_GEOIP2 || defined SUPPORT_DBIP2
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
