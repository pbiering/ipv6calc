/*
 * Project    : ipv6calc
 * File       : ipv6loganon.h
 * Version    : $Id: ipv6loganon.h,v 1.1 2007/01/30 17:00:37 peter Exp $
 * Copyright  : 2007 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Main header file
 */ 

/* global program related definitions */
#define PROGRAM_NAME "ipv6loganon"
#define PROGRAM_COPYRIGHT "(P) & (C) 2007 by Peter Bieringer <pb (at) bieringer.de>"

/* LRU cache maximum size */
#define CACHE_LRU_SIZE 200

/* prototyping */
int cache_lru_limit;

int mask_ipv4;
int mask_ipv6_net;
int mask_eui_id;
