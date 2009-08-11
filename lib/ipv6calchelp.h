/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.h
 * Version    : $Id: ipv6calchelp.h,v 1.9 2009/08/11 21:04:44 peter Exp $
 * Copyright  : 2002-2009 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calchelp.c
 */

#include "ipv6calc_inttypes.h"

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
extern void printhelp_actiontypes(const uint32_t formatoptions);
extern void printhelp(void);
extern void printhelp_oldoptions(void);
extern void printhelp_output_dispatcher(const uint32_t outputtype);
