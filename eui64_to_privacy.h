/*
 * Project    : ipv6calc
 * File       : eui_to_privacy.h
 * Version    : $Id: eui64_to_privacy.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for eui_to_privacy.c
 */ 

/* prototypes */
extern int  eui64_to_privacy(char *hostidentifier, char *token, char *resultstring);
extern void eui64_to_privacy_printhelp(void);
extern void eui64_to_privacy_printhelplong(void);
