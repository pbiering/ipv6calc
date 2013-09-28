/*
 * Project    : ipv6calc/lib
 * File       : libipv6calcdebug.h
 * Version    : $Id: libipv6calcdebug.h,v 1.7 2013/09/28 11:23:20 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Debug information
 */ 


#ifndef _libipv6calcdebug_h_

#define _libipv6calcdebug_h_

#define DEBUG_ipv6calcoptions			0x000008l

#define DEBUG_libipv6calc			0x000010l
#define DEBUG_libipv6addr			0x000020l
#define DEBUG_libipv4addr			0x000040l
#define DEBUG_libipv6calctypes			0x000080l

#define DEBUG_librfc3041			0x000100l
#define DEBUG_librfc1884			0x000200l
#define DEBUG_librfc2874			0x000400l
#define DEBUG_librfc1886			0x000800l
#define DEBUG_librfc3056			0x001000l
#define DEBUG_libifinet6			0x002000l

#define DEBUG_libipv6addr_iidrandomdetection	0x010000l

#define DEBUG_libipv6addr_db_wrapper		0x100000l


/* print debug macros (old version) */
#define DEBUGPRINT(d, ...)	if ((ipv6calc_debug & d) != 0) { fprintf(stderr,  __VA_ARGS__); };

/* debug print with args */
#define DEBUGPRINT_WA(d, t, ...)	if ((ipv6calc_debug & d) != 0) { fprintf(stderr, "%s/%s: " t "\n", __FILE__, __func__, __VA_ARGS__); };

/* debug print no args */
#define DEBUGPRINT_NA(d, t)		if ((ipv6calc_debug & d) != 0) { fprintf(stderr, "%s/%s: " t "\n", __FILE__, __func__); };


#endif

/* global debug value */
extern long int ipv6calc_debug;
