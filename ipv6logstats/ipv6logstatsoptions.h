/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatsoptions.h
 * Version    : $Id: ipv6logstatsoptions.h,v 1.4 2012/03/04 18:08:50 peter Exp $
 * Copyright  : 2003-2012 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing optios for ipvlogstats.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6logstatsoptions_h_

#define _ipv6logstatsoptions_h_

/* Options */

/* define short options */
static char *ipv6logstats_shortopts = "vqh?uonicd:p:o:";

/* define long options */
static struct option ipv6logstats_longopts[] = {
	{"version", 0, 0, (int) 'v'},
	{"debug", 1, 0, (int) 'd'},
	{"quiet", 0, 0, (int) 'q'},

	/* help options */
	{"help", 0, 0, (int) 'h'},

	/* normal options */
	{"unknown", 0, 0, (int) 'u'},
	{"colums", 0, 0, (int) 'c'},
	{"prefix", 0, 0, (int) 'p'},
	{"iid", 0, 0, (int) 'i'},
	{"noheader", 0, 0, (int) 'n'},
	{"onlyheader", 0, 0, (int) 'o'},
	{"output", 1, 0, (int) 'O'},

	{NULL, 0, 0, 0}
};                

#endif
