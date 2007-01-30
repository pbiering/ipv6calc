/*
 * Project    : ipv6calc
 * File       : ipv6logconvoptions.h
 * Version    : $Id: ipv6logconvoptions.h,v 1.5 2007/01/30 15:46:45 peter Exp $
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
static char *ipv6logconv_shortopts = "vnqh?d:c:";

/* define long options */
static struct option ipv6logconv_longopts[] = {
	{"version", 0, 0, (int) 'v'},
	{"debug", 1, 0, (int) 'd'},
	{"quiet", 0, 0, (int) 'q'},
	{"nocache", 0, 0, (int) 'n'},
	{"cachelimit", 0, 0, (int) 'c'},

	/* help options */
	{"help", 0, 0, (int) 'h'},
	{"examples"     , 0, 0, CMD_printexamples },
	{"printexamples", 0, 0, CMD_printexamples },

	/* options */
	{ "out"       , 1, 0, CMD_outputtype },

	{NULL, 0, 0, 0}
};                

#endif
