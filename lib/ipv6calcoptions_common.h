/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions_common.h,v 1.3 2013/09/28 20:32:40 ds6peter Exp $
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
static char *ipv6calc_shortopts_common = "d:LGq";

/* define common long options */
static struct option ipv6calc_longopts_common[] = {
	{"debug"  , 1, NULL, (int) 'd' },
	{"quiet"  , 0, NULL, (int) 'q' },

	/* database options (old) */
	{"db-ip2location-ipv4"         , 1, NULL, DB_ip2location_ipv4   },
	{"db-ip2location-ipv6"         , 1, NULL, DB_ip2location_ipv6   },
	{"db-ip2location-default"      , 0, NULL, (int) 'L'             },
	{"db-ip2location-ipv4-default" , 0, NULL, DB_geoip_ipv4_default },
	{"db-ip2location-ipv6-default" , 0, NULL, DB_geoip_ipv6_default },
	{"db-geoip"                    , 1, NULL, DB_geoip_ipv4         }, // backward compatibility
	{"db-geoip-ipv4"               , 1, NULL, DB_geoip_ipv4         },
	{"db-geoip-ipv6"               , 1, NULL, DB_geoip_ipv6         },
	{"db-geoip-default"            , 0, NULL, (int) 'G'             },
	{"db-geoip-ipv4-default"       , 0, NULL, DB_geoip_ipv4_default },
	{"db-geoip-ipv6-default"       , 0, NULL, DB_geoip_ipv6_default },

	/* database options (new) */
	{"db-ip2location-disable"      , 0, NULL, DB_ip2location_disable },
	{"db-ip2location-dir"          , 1, NULL, DB_ip2location_dir     },
	{"db-ip2location-lib"          , 1, NULL, DB_ip2location_lib     },

	{"db-geoip-disable"            , 0, NULL, DB_geoip_disable       },
	{"db-geoip-dir"                , 1, NULL, DB_geoip_dir           },
	{"db-geoip-lib"                , 1, NULL, DB_geoip_lib           },
};                
