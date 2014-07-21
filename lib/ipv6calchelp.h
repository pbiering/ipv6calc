/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.h
 * Version    : $Id: ipv6calchelp.h,v 1.18 2014/07/21 06:14:27 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calchelp.c
 */

#include "ipv6calc_inttypes.h"
#include "ipv6calcoptions.h"

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
extern void printhelp_common(void);
extern void printhelp_shortcut_options(const struct option longopts[], const s_ipv6calc_longopts_shortopts_map longopts_shortopts_map[]);
extern void printhelp_output_dispatcher(const uint32_t outputtype);
extern void printhelp_action_dispatcher(const uint32_t action, const int embedded);

extern void ipv6calc_print_features_verbose(const int level_vebose);
