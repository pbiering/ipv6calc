/*
 * Project    : ipv6calc
 * File       : ipv6loganonoptions.h
 * Version    : $Id: ipv6loganonoptions.h,v 1.7 2011/05/12 10:30:47 peter Exp $
 * Copyright  : 2007-2011 by Peter Bieringer <pb (at) bieringer.de>
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
static char *ipv6loganon_shortopts = "vnVh?fd:c:w:a:";

/* define long options */
static struct option ipv6loganon_longopts[] = {
	{"version"   , no_argument      , 0, (int) 'v'},
	{"debug"     , required_argument, 0, (int) 'd'},
	{"verbose"   , no_argument      , 0, (int) 'V'},
	{"nocache"   , no_argument      , 0, (int) 'n'},
	{"cachelimit", required_argument, 0, (int) 'c'},
	{"write"     , required_argument, 0, (int) 'w'},
	{"append"    , required_argument, 0, (int) 'a'},
	{"flush"     , no_argument      , 0, (int) 'f'},

	/* help options */
	{"help"      , no_argument      , 0, (int) 'h'},

	/* options */
	{ "anonymize-standard", no_argument      , 0, CMD_ANON_PRESET_STANDARD },
	{ "anonymize-careful" , no_argument      , 0, CMD_ANON_PRESET_CAREFUL  },
	{ "anonymize-paranoid", no_argument      , 0, CMD_ANON_PRESET_PARANOID },
	{ "mask-ipv4"         , required_argument, 0, CMD_ANON_MASK_IPV4       },
	{ "mask-ipv6"         , required_argument, 0, CMD_ANON_MASK_IPV6       },
	{ "no-mask-iid"       , no_argument      , 0, CMD_ANON_NO_MASK_IID     },

	{NULL, 0, 0, 0}
};                

#endif
