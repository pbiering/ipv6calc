/*
 * Project    : ipv6calc
 * File       : ifinet6_to_compressed.h
 * Version    : $Id: ifinet6_to_compressed.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Main header file
 */ 

/* prototypes */
extern int  ifinet6_to_compressed(char *addrstring, char *resultstring, unsigned long command);
extern int  ifinet6_to_compressedwithprefixlength(char *addrstring, char *prefixlengthstring, char *resultstring, unsigned long command);
extern void ifinet6_to_compressed_printhelp(void);
extern void ifinet6_to_compressed_printhelplong(void);
