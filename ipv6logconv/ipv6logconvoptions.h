/*
 * Project    : ipv6calc
 * File       : ipv6logconvoptions.h
 * Version    : $Id$
 * Copyright  : 2002-2007 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipvlogconv.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6logconvoptions_h_

#define _ipv6logconvoptions_h_

/* Options */

/* define short options */
static char *ipv6logconv_shortopts = "vfh?nc:";

/* define long options */
static struct option ipv6logconv_longopts[] = {
	{"version", 0, NULL, (int) 'v' },
	{"flush"  , 0, NULL, (int) 'f' },

	/* help options */
	{"help"           , 0, NULL, (int) 'h'},
	{"examples"     , 0, 0, CMD_printexamples },
	{"printexamples", 0, 0, CMD_printexamples },

	/* cache options */
	{"nocache", 0, 0, (int) 'n'},
	{"cachelimit", 0, 0, (int) 'c'},

	/* options */
	{ "out"       , 1, 0, CMD_outputtype },
};                

#endif
