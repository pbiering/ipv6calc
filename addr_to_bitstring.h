/*
 * Project    : ipv6calc
 * File       : ipv6calc.h[.in]
 * Version    : $Id: addr_to_bitstring.h,v 1.1 2002/02/19 21:41:17 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  header file for 
 */

/* prototypes */
void addr_to_bitstring_printhelp();
void addr_to_bitstring_printhelplong();
int  addr_to_bitstring(char *addrstring, char *resultstring, long int command);
