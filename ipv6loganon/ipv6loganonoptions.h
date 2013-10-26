/*
 * Project    : ipv6calc
 * File       : ipv6loganonoptions.h
 * Version    : $Id: ipv6loganonoptions.h,v 1.12 2013/10/26 13:45:41 ds6peter Exp $
 * Copyright  : 2007-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipvloganon.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6loganonoptions_h_

#define _ipv6loganonoptions_h_

/* Options */

/* define short options */
static char *ipv6loganon_shortopts = "vh?Vnc:w:a:f";

/* define long options */
static struct option ipv6loganon_longopts[] = {
	{"version", 0, NULL, (int) 'v' },
	{"verbose", 0, NULL, (int) 'V' },

	/* help options */
	{"help"           , 0, NULL, (int) 'h'},

	/* special options */
	{"nocache"   , no_argument      , 0, (int) 'n'},
	{"flush"     , no_argument      , 0, (int) 'f'},
	{"cachelimit", required_argument, 0, (int) 'c'},
	{"write"     , required_argument, 0, (int) 'w'},
	{"append"    , required_argument, 0, (int) 'a'},

	/* options */
	{ "anonymize-standard", no_argument      , 0, CMD_ANON_PRESET_STANDARD },
	{ "anonymize-careful" , no_argument      , 0, CMD_ANON_PRESET_CAREFUL  },
	{ "anonymize-paranoid", no_argument      , 0, CMD_ANON_PRESET_PARANOID },
	{ "anonymize-preset"  , required_argument, 0, CMD_ANON_PRESET_OPTION   },
	{ "anonymize-method"  , required_argument, 0, CMD_ANON_METHOD_OPTION   },
	{ "mask-ipv4"         , required_argument, 0, CMD_ANON_MASK_IPV4       },
	{ "mask-ipv6"         , required_argument, 0, CMD_ANON_MASK_IPV6       },
	{ "mask-iid"          , required_argument, 0, CMD_ANON_MASK_IID        },
	{ "mask-mac"          , required_argument, 0, CMD_ANON_MASK_MAC        },
};                

#endif
