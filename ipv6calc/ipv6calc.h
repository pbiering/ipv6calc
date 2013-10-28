/*
 * Project    : ipv6calc
 * File       : ipv6calc.h
 * Version    : $Id: ipv6calc.h,v 1.16 2013/10/28 07:25:31 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Main header file
 */ 


#ifndef _ipv6calc_h_

#define _ipv6calc_h_

/* global program related definitions */
#define PROGRAM_NAME "ipv6calc"
#define PROGRAM_COPYRIGHT "(P) & (C) 2001-" COPYRIGHT_YEAR " by Peter Bieringer <pb (at) bieringer.de>"

//#define ANONPRESET_DEFAULT "keep-type-asn-cc"
#define ANONPRESET_DEFAULT "anonymize-standard"

#define DEBUG_ipv6calc_general      0x00000001l

#endif

extern int feature_zeroize;
extern int feature_anon;
extern int feature_kp;
