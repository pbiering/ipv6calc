/*
 * Project    : ipv6calc
 * File       : ipv6loganonoptions.h
 * Version    : $Id: ipv6loganonoptions.h,v 1.4 2007/02/06 07:08:26 peter Exp $
 * Copyright  : 2007 by Peter Bieringer <pb (at) bieringer.de>
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
static char *ipv6loganon_shortopts = "vnVh?d:c:";

/* define long options */
static struct option ipv6loganon_longopts[] = {
	{"version", 0, 0, (int) 'v'},
	{"debug", 1, 0, (int) 'd'},
	{"verbose", 0, 0, (int) 'V'},
	{"nocache", 0, 0, (int) 'n'},
	{"cachelimit", 0, 0, (int) 'c'},

	/* help options */
	{"help", 0, 0, (int) 'h'},

	/* options */
	{ "anonymize-standard" , 0, 0, CMD_ANON_PRESET_STANDARD },
	{ "mask-ipv4" , 1, 0, CMD_ANON_MASK_IPV4 },
	{ "no-mask-iid" , 0, 0, CMD_ANON_NO_MASK_IID },

	{NULL, 0, 0, 0}
};                

#endif
