/*
 * Project    : ipv6calc
 * File       : ifinet6_to_compressed.h
 * Version    : $Id: ifinet6_to_compressed.h,v 1.2 2002/02/27 23:07:14 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ifinet6_to_compressed.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  ifinet6_to_compressed(char *addrstring, char *resultstring, unsigned long command);
extern int  ifinet6_to_compressedwithprefixlength(char *addrstring, char *prefixlengthstring, char *resultstring, unsigned long command);
extern void ifinet6_to_compressed_printhelp(void);
extern void ifinet6_to_compressed_printhelplong(void);
