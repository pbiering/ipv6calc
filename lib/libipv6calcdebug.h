/*
 * Project    : ipv6calc/lib
 * File       : libipv6calcdebug.h
 * Version    : $Id: libipv6calcdebug.h,v 1.2 2002/04/04 19:40:27 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Debug information
 */ 


#ifndef _libipv6calcdebug_h_

#define _libipv6calcdebug_h_

#define DEBUG_libipv6calc		0x000010l
#define DEBUG_libipv6addr		0x000020l
#define DEBUG_libipv4addr		0x000040l
#define DEBUG_libipv6calctypes		0x000080l

#define DEBUG_librfc3041		0x000100l
#define DEBUG_librfc1884		0x000200l
#define DEBUG_librfc2874		0x000400l
#define DEBUG_librfc1886		0x000800l
#define DEBUG_librfc3056		0x001000l
#define DEBUG_libifinet6		0x002000l

/* global debug value */
extern long int ipv6calc_debug;

#endif
