/*
 * Project    : ipv6calc
 * File       : ipv6logconv.h
 * Version    : $Id: ipv6logconv.h,v 1.5 2005/02/13 11:50:30 peter Exp $
 * Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Main header file
 */ 

/* global program related definitions */
#define PROGRAM_NAME "ipv6logconv"
#define PROGRAM_COPYRIGHT "(P) & (C) 2002-2005 by Peter Bieringer <pb (at) bieringer.de>"

/* LRU cache maximum size */
#define CACHE_LRU_SIZE 200

/* prototyping */
int cache_lru_limit;
