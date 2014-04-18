/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatsoptions.h
 * Version    : $Id: ipv6logstatsoptions.h,v 1.10 2014/04/18 10:25:46 ds6peter Exp $
 * Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
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
static char *ipv6logstats_shortopts = "vh?uosncp:w:";

/* define long options */
static struct option ipv6logstats_longopts[] = {
	{"version", 0, 0, (int) 'v'},

	/* help options */
	{"help", 0, 0, (int) 'h'},

	/* normal options */
	{"unknown"	, 0, 0, (int) 'u'},
	{"columns"	, 0, 0, (int) 'c'},
	{"prefix"	, 0, 0, (int) 'p'},
	{"noheader"	, 0, 0, (int) 'n'},
	{"onlyheader"	, 0, 0, (int) 'o'},
	{"simple"	, 0, 0, (int) 's'},
	{"write"	, 1, 0, (int) 'O'},
};                

#endif
