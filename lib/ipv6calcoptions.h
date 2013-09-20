/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions.h,v 1.26 2013/09/20 06:17:52 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options
 */ 

#include <getopt.h> 
#include <stdio.h>

#include "ipv6calctypes.h"
#include "ipv6calccommands.h"


#ifndef _ipv6calcoptions_h_

#define _ipv6calcoptions_h_

#define MAXLONGOPTIONS	100

#endif

extern void ipv6calc_debug_from_env(void);

extern const char *ipv6calcoption_name(const int opt, const struct option longopts_p[]);
extern int ipv6calcoptions(const int opt, const char *optarg, const int quiet, const struct option longopts[]);

extern void ipv6calc_options_add(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p, const char *shortopts_custom, const struct option longopts_custom[], const int longopts_custom_entries);
