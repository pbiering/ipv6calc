/*
 * Project    : ipv6calc
 * File       : ipv6calc_inttypes.h
 * Version    : $Id: ipv6calc_inttypes.h,v 1.3 2015/04/16 06:23:20 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Common header file to define inttypes like uint8_t and other basics
 */ 

#ifndef _ipv6calc_inttypes_h

#define _ipv6calc_inttypes_h 1


#if defined(__linux__) 
/* Linux/glibc has no inttypes.h */
#include <stdint.h>
#else
/* Should catch all *BSD, others, too */
#include <inttypes.h>
#include <sys/types.h>
#endif


/* max string length */
#define NI_MAXHOST      		1024
#define IPV6CALC_ADDR_STRING_MAX	256
#define IPV6CALC_SCOPEID_STRING_MAX	64
#define IPV6CALC_COUNTRYCODE_STRING_MAX	16


#endif // _ipv6calc_inttypes_h
