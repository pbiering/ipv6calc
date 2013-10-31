/*
 * Project    : ipv6calc
 * File       : libieee.c
 * Version    : $Id: libieee.c,v 1.18 2013/10/31 21:24:46 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
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

#ifdef SUPPORT_DB_IEEE
#include "../databases/ieee-oui/dbieee_oui.h"
#include "../databases/ieee-oui36/dbieee_oui36.h"
#include "../databases/ieee-iab/dbieee_iab.h"
#endif

/*
 * Get vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libieee_get_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, NI_MAXHOST - 1, "Linux ISDN-NET/PLIP");
		return (0);
	};

	if ( (macaddrp->addr[0] & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

#ifdef SUPPORT_DB_IEEE
	idval = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	subidval = (macaddrp->addr[3] << 16) | (macaddrp->addr[4] << 8) | macaddrp->addr[5];

	/* run through IAB list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_iab); i++) {
		if (libieee_iab[i].id == idval) {
			/* major id match */
			if (libieee_iab[i].subid_begin <= subidval && libieee_iab[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_iab[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui36[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui[i].string_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "(IEEE databases not compiled in)");
	return (0);
#endif

	/* not found */
   	retval = 1;	
	return (retval);
};


/*
 * Get short vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libieee_get_short_vendor_string(char *resultstring, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, NI_MAXHOST - 1, "Linux-ISDN-NET+PLIP");
		return (0);
	};
	
	if ( (macaddrp->addr[0] & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

#ifdef SUPPORT_DB_IEEE
	idval = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	subidval = (macaddrp->addr[3] << 16) | (macaddrp->addr[4] << 8) | macaddrp->addr[5];

	/* run through IAB list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_iab); i++) {
		if (libieee_iab[i].id == idval) {
			/* major id match */
			if (libieee_iab[i].subid_begin <= subidval && libieee_iab[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_iab[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui36[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui[i].shortstring_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "(IEEE databases not compiled in)");
	return (0);
#endif

	/* not found */
   	retval = 1;	
	return (retval);
};


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

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s/%s: called with map_value=%08x\n", __FILE__, __func__, map_value);
	};

	/* run through map */
	for (i = 0; i < (int) (sizeof(ieee_mapping) / sizeof(ieee_mapping[0])); i++) {
		if (ieee_mapping[i].mapping == map_index) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s/%s: found entry in map: %06x\n", __FILE__, __func__, map_index);
			};

			*bits_00_23_p = ieee_mapping[i].bits_00_23;
			*bits_24_36_p = map_value & 0xfff;

			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s/%s: mapped to bits_00_23=%06x bits_24_36=%03x\n", __FILE__, __func__, *bits_00_23_p, *bits_24_36_p);
			};

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

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s/%s: called with OUI: %06x\n", __FILE__, __func__, oui);
	};

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

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s/%s: called with OUI: %06x\n", __FILE__, __func__, oui);
	};

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

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s/%s: called\n", __FILE__, __func__);
	};

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

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s/%s: called\n", __FILE__, __func__);
	};

	libieee_unmap_oui36_iab(map_value, &bits_00_23, &bits_24_36);

	eui64addrp->addr[0] = (bits_00_23 & 0xff0000) >> 16;
	eui64addrp->addr[1] = (bits_00_23 & 0x00ff00) >>  8;
	eui64addrp->addr[2] = (bits_00_23 & 0x0000ff);
	eui64addrp->addr[3] = (bits_24_36 & 0x000ff0) >>  4;
	eui64addrp->addr[4] = (bits_24_36 & 0x00000f) <<  4;
	eui64addrp->flag_valid = 1;

	return (0);
};
