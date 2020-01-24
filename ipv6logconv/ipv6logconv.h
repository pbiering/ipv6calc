/*
 * Project    : ipv6calc
 * File       : ipv6logconv.h
 * Version    : $Id$
 * Copyright  : 2002-2020 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Main header file
 */ 

/* global program related definitions */
#define PROGRAM_NAME "ipv6logconv"
#define PROGRAM_COPYRIGHT "(P) & (C) 2002-" COPYRIGHT_YEAR " by Peter Bieringer <pb (at) bieringer.de>"

/* LRU cache maximum size */
#define CACHE_LRU_SIZE 200


#define DEBUG_ipv6logconv_general      0x00000001l
#define DEBUG_ipv6logconv_processing   0x00000002l

extern int cache_lru_limit;

extern int feature_reg;
extern int feature_ieee;
