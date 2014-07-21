/*
 * Project    : ipv6calc/ipv6calc
 * File       : ipv6calchelp_local.h
 * Version    : $Id: ipv6calchelp_local.h,v 1.3 2014/07/21 06:14:27 ds6peter Exp $
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
