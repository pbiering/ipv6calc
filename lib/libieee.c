/*
 * Project    : ipv6calc
 * File       : libieee.c
 * Version    : $Id$
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for IEEE information
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "libieee.h"
#include "libmac.h"
#include "libeui64.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"


/*
 * check for OUI-36/IAB
 * in:  bits_00_23
 * out: 0=not OUI-36/IAB, 1=is OUI-36/IAB
 */
int libieee_check_oui36_iab(const uint32_t bits_00_23) {
	int r = 0, i;

	DEBUGPRINT_WA(DEBUG_libieee, "called with bits_00_23=%06x", bits_00_23);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		DEBUGPRINT_WA(DEBUG_libieee, "check against: %06x", ieee_mapping[i].bits_00_23);

		if (ieee_mapping[i].bits_00_23 == bits_00_23) {
			DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x", bits_00_23);
			r = 1;
			break;
		};
	};

	return(r);
};


/*
 * map OUI-36/IAB
 * in:  bits_00_23, bits_24_36
 * out: mapping value
 */
uint32_t libieee_map_oui36_iab(const uint32_t bits_00_23, const uint32_t bits_24_36) {
	int i;
	uint32_t map_value = bits_00_23;

	DEBUGPRINT_WA(DEBUG_libieee, "called with bits_00_23=%06x bits_24_36=%03x", bits_00_23, bits_24_36);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		DEBUGPRINT_WA(DEBUG_libieee, "check against: %06x", ieee_mapping[i].bits_00_23);

		if (ieee_mapping[i].bits_00_23 == bits_00_23) {
			// hit, set flag (0x1mmmvvv), mapping number (mmm) and 12 bit vendor code
			map_value = 0x1000000 | (bits_00_23 & 0xff0000) | (ieee_mapping[i].mapping << 12) | bits_24_36;

			DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x -> %08x", bits_00_23, map_value);

			break;
		};
	};

	return (map_value);
};

/*
 * unmap OUI-36/IAB
 * in : mapping value
 * out:  bits_00_23, bits_24_36
 */
int libieee_unmap_oui36_iab(const uint32_t map_value, uint32_t *bits_00_23_p, uint32_t *bits_24_36_p) {
	int i;
	uint32_t map_index = (map_value & 0x00f000) >> 12;

	*bits_00_23_p = 0;
	*bits_24_36_p = 0;

	DEBUGPRINT_WA(DEBUG_libieee, "called with map_value=%08x", map_value);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		if (ieee_mapping[i].mapping == map_index) {
			DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x", map_index);

			*bits_00_23_p = ieee_mapping[i].bits_00_23;
			*bits_24_36_p = map_value & 0xfff;

			DEBUGPRINT_WA(DEBUG_libieee, "mapped to bits_00_23=%06x bits_24_36=%03x", *bits_00_23_p, *bits_24_36_p);

			return (0);
		};
	};

	fprintf(stderr, "%s/%s: missing entry in map: %06x\n", __FILE__, __func__, map_index);
	return (1);
};

/*
 * map IAB/OUI-36
 * in:  macaddrp
 * out: OUI (0x0......) or mapped IAB/OUI-36 (0x1mmm...)
 *	mmm = map index
 */
uint32_t libieee_map_oui_macaddr(const ipv6calc_macaddr *macaddrp) {
	uint32_t oui, ven;

	oui = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	ven = (macaddrp->addr[3] << 4)  | (macaddrp->addr[4] >> 4);

	DEBUGPRINT_WA(DEBUG_libieee, "called with OUI: %06x", oui);

	return (libieee_map_oui36_iab(oui, ven));
};

/*
 * map IAB/OUI-36
 * in:  eui64addrp
 * out: OUI (0x0......) or mapped IAB/OUI-36 (0x1mmm...)
 *	mmm = map index
 */
uint32_t libieee_map_oui_eui64addr(const ipv6calc_eui64addr *eui64addrp) {
	uint32_t oui, ven;

	oui = (eui64addrp->addr[0] << 16) | (eui64addrp->addr[1] << 8) | eui64addrp->addr[2];
	ven = (eui64addrp->addr[3] << 4)  | (eui64addrp->addr[4] >> 4);

	DEBUGPRINT_WA(DEBUG_libieee, "called with OUI: %06x", oui);

	return (libieee_map_oui36_iab(oui, ven));
};

/*
 * unmap IAB/OUI-36
 * in : OUI (0x0......) or mapped IAB/OUI-36 (0x1mmm...)
 *	mmm = map index
 * mod: macaddrp (last bits zeroized)
 * out: result
 */
int libieee_unmap_oui_macaddr(ipv6calc_macaddr *macaddrp, uint32_t map_value) {
	uint32_t bits_00_23, bits_24_36;

	mac_clearall(macaddrp);

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	libieee_unmap_oui36_iab(map_value, &bits_00_23, &bits_24_36);

	macaddrp->addr[0] = (bits_00_23 & 0xff0000) >> 16;
	macaddrp->addr[1] = (bits_00_23 & 0x00ff00) >>  8;
	macaddrp->addr[2] = (bits_00_23 & 0x0000ff);
	macaddrp->addr[3] = (bits_24_36 & 0x000ff0) >>  4;
	macaddrp->addr[4] = (bits_24_36 & 0x00000f) <<  4;
	macaddrp->flag_valid = 1;

	return (0);
};


/*
 * unmap IAB/OUI-36
 * in : OUI (0x0......) or mapped IAB/OUI-36 (0x1mmm...)
 *	mmm = map index
 * mod: eui64addrp (last bits zeroized)
 * out: result
 */
int libieee_unmap_oui_eui64addr(ipv6calc_eui64addr *eui64addrp, uint32_t map_value) {
	uint32_t bits_00_23, bits_24_36;

	libeui64_clearall(eui64addrp);

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	libieee_unmap_oui36_iab(map_value, &bits_00_23, &bits_24_36);

	eui64addrp->addr[0] = (bits_00_23 & 0xff0000) >> 16;
	eui64addrp->addr[1] = (bits_00_23 & 0x00ff00) >>  8;
	eui64addrp->addr[2] = (bits_00_23 & 0x0000ff);
	eui64addrp->addr[3] = (bits_24_36 & 0x000ff0) >>  4;
	eui64addrp->addr[4] = (bits_24_36 & 0x00000f) <<  4;
	eui64addrp->flag_valid = 1;

	return (0);
};
