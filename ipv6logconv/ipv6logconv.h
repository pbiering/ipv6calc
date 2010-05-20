/*
 * Project    : ipv6calc
 * File       : ipv6logconv.h
 * Version    : $Id: ipv6logconv.h,v 1.7 2010/05/20 16:21:22 peter Exp $
 * Copyright  : 2002-2006 by Peter Bieringer <pb (at) bieringer.de>
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

/* prototyping */
int cache_lru_limit;
