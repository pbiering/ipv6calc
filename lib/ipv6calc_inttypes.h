/*
 * Project    : ipv6calc
 * File       : ipv6calc_inttypes.h
 * Version    : $Id: ipv6calc_inttypes.h,v 1.1 2002/04/21 11:24:08 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Common header file to define inttypes like uint8_t and other basics
 */ 

#if defined(__linux__) 
/* Linux/glibc has no inttypes.h */
#include <stdint.h>
#else
/* Should catch all *BSD, others, too */
#include <inttypes.h>
#include <sys/types.h>
#endif


/* max string length */
#define NI_MAXHOST      1024
