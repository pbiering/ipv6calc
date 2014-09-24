/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.h
 * Version    : $Id: ipv6calchelp.h,v 1.21 2014/09/24 09:07:57 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calchelp.c
 */

#include "ipv6calc_inttypes.h"
#include "ipv6calcoptions.h"

#ifndef _ipv6calchelp_h

#define _ipv6calchelp_h 1

#define IPV6CALC_HELP_GEOIP		0x0010
#define IPV6CALC_HELP_IP2LOCATION	0x0020
#define IPV6CALC_HELP_DBIP		0x0040
#define IPV6CALC_HELP_EXTERNAL		0x0080
#define IPV6CALC_HELP_BUILTIN		0x0100
#define IPV6CALC_HELP_ALL		0xffff
#define IPV6CALC_HELP_BASIC		0x0000
#define IPV6CALC_HELP_QUIET		0x0001

#endif

/* defined in each main program */
extern void printhelp_info(void);
extern void printhelp_copyright(void);
extern void printhelp_version(void);

/* defined in ipv6calchelp.c */
/* extern void printhelp_print(void); */
/* extern void printhelp_mask(void); */
/* extern void printhelp_case(void); */
/* extern void printhelp_printstartend(void); */

extern void printhelp_doublecommands(void);
extern void printhelp_missinginputdata(void);
extern void printhelp_inputtypes(const uint32_t formatoptions);
extern void printhelp_outputtypes(const uint32_t inputtype, const uint32_t formatoptions);
extern void printhelp_actiontypes(const uint32_t formatoptions, const struct option longopts[]);
extern void printhelp_common(const uint32_t help_features);
extern void printhelp_shortcut_options(const struct option longopts[], const s_ipv6calc_longopts_shortopts_map longopts_shortopts_map[]);
extern void printhelp_output_dispatcher(const uint32_t outputtype);
extern void printhelp_action_dispatcher(const uint32_t action, const int embedded);

extern void ipv6calc_print_features_verbose(const int level_vebose);
