/*
 * Project    : ipv6calc/ipv6calc
 * File       : ipv6calchelp_local.h
 * Version    : $Id$
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calchelp_local.c
 */

extern void printversion(void);
extern void printversion_help(void);
extern void printversion_verbose(int level_verbose);
extern void ipv6calc_printinfo(void);
extern void ipv6calc_printhelp(const struct option longopts[], const s_ipv6calc_longopts_shortopts_map longopts_shortopts_map[]);
