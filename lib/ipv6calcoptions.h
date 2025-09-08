/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Copyright  : 2002-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options
 */ 

#include <getopt.h> 
#include <stdio.h>

#ifndef _ipv6calcoptions_h_

#define _ipv6calcoptions_h_


#include "ipv6calctypes.h"
#include "ipv6calccommands.h"


#define IPV6CALC_MAXLONGOPTIONS	128

/* map between longopts and shortopts */
typedef struct {
        const char c;
        const int  val;
	const char *info;
} s_ipv6calc_longopts_shortopts_map;

#endif

extern int ipv6calc_quiet;

extern void ipv6calc_debug_from_env(void);
extern void ipv6calc_common_options_from_env(const struct option longopts[], s_ipv6calc_anon_set *ipv6calc_anon_set_p);
extern int ipv6calc_set_option(const struct option longopts[], const char *name, const char *value, s_ipv6calc_anon_set *ipv6calc_anon_set_p);

extern const char *ipv6calcoption_name(const int opt, const struct option longopts_p[]);

extern int ipv6calcoptions_common_basic(const int opt, const char *optarg, const struct option longopts[]);
extern int ipv6calcoptions_common_anon(const int opt, const char *optarg, const struct option longopts[], s_ipv6calc_anon_set *ipv6calc_anon_set_p);

extern void ipv6calc_options_add(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p, const char *shortopts_custom, const struct option longopts_custom[], const int longopts_custom_entries);
extern void ipv6calc_options_add_common_anon(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p);
extern void ipv6calc_options_add_common_basic(char *shortopts_p, const int shortopts_maxlen, struct option longopts[], int *maxentries_p);

