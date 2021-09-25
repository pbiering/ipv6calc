/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.c
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Format and type handling
 */ 

#include <stdio.h>
#include <string.h>
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"

/* text representations */
const s_format ipv6calc_formatstrings[] = {
	{ FORMAT_auto           , "auto"           , "automatic detection", "" },
	{ FORMAT_revnibbles_int , "revnibbles.int" , "dot separated nibbles reverse, ending with ip6.int.", "" },
	{ FORMAT_revnibbles_arpa, "revnibbles.arpa", "dot separated nibbles reverse, ending with ip6.arpa.", "" },
	{ FORMAT_bitstring      , "bitstring"      , "bitstring labes, ending with ip6.arpa.", "" },
	{ FORMAT_ipv6addr       , "ipv6addr"       , "IPv6 address", "ipv6" },
	{ FORMAT_ipv4addr       , "ipv4addr"       , "IPv4 address", "ipv4" },
	{ FORMAT_mac            , "mac"            , "MAC address (48 bits)", "eui48" },
	{ FORMAT_eui64          , "eui64"          , "EUI-64 identifier (64 bits)", "" },
	{ FORMAT_base85         , "base85"         , "Base-85 string", "" },
	{ FORMAT_ifinet6        , "ifinet6"        , "Like line in /proc/net/if_inet6", "" },
	{ FORMAT_iid            , "iid"            , "Interface identifier", "" },
	{ FORMAT_iid_token      , "iid+token"      , "Interface identifier and token", "" },
	{ FORMAT_addrtype       , "addrtype"       , "Address type", "" },
	{ FORMAT_ouitype        , "ouitype"        , "OUI (IEEE) type", "" },
	{ FORMAT_ipv6addrtype   , "ipv6addrtype"   , "IPv6 address type", "" },
	{ FORMAT_ipv6logconv    , "ipv6logconv"    , "ipv6logconv (currently not supported)", "" },
	{ FORMAT_any            , "any"            , "any type (currently not supported)", "" },
	{ FORMAT_revipv4	, "revipv4"        , "reverse IPv4, ending with in-addr.arpa", "" },
	{ FORMAT_ipv4hex	, "ipv4hex" 	   , "IPv4 in hexdecimal format", "" },
	{ FORMAT_ipv4revhex	, "ipv4revhex"     , "IPv4 in byte-reversed hexdecimal format", "" },
	{ FORMAT_octal		, "octal"	   , "IP address in escaped octal format", "" },
	{ FORMAT_hex		, "hex"	 	   , "IP address in hexadecimal format", "" },
	{ FORMAT_ipv6literal	, "ipv6literal"	   , "IPv6 address in literal", "" },
	{ FORMAT_ipv6rd		, "ipv6rd"	   , "IPv6 Rapid Deployment address", "" },
	{ FORMAT_ipv6to4	, "ipv6to4"	   , "IPv6 address based on 6to4 converted IPv4 address", "" },

	{ FORMAT_prefix_mac     , "prefix+mac"     , "IPv6 prefix and a MAC address", "" },
	{ FORMAT_asn            , "asn"            , "Autonomous System Number", "" },
};

const int ipv6calc_formatstrings_entries = MAXENTRIES_ARRAY(ipv6calc_formatstrings);


const s_formatoption ipv6calc_formatoptionstrings[] = {
	{ FORMATOPTION_printlowercase   , "--lowercase|-l"     , "Print output in lower case" },
	{ FORMATOPTION_printuppercase   , "--uppercase|-u"     , "Print output in upper case" },
	{ FORMATOPTION_printprefix      , "--printprefix"      , "Print only prefix" },
	{ FORMATOPTION_printsuffix      , "--printsuffix"      , "Print only suffix" },
	{ FORMATOPTION_maskprefix       , "--maskprefix"       , "Mask prefix bits (suffix set to 0)" },
	{ FORMATOPTION_masksuffix       , "--masksuffix"       , "Mask suffix bits (prefix set to 0)" },
	{ FORMATOPTION_printstart       , "--printstart <num>" , "Printing starts at bit <num>" },
	{ FORMATOPTION_printend         , "--printend <num>"   , "Printing ends at bit <num>" },
	{ FORMATOPTION_printcompressed  , "--printcompressed|-C"  , "Print in compressed format" },
	{ FORMATOPTION_printuncompressed  , "--printuncompressed|-U"  , "Print in uncompressed format" },
	{ FORMATOPTION_printfulluncompressed  , "--printfulluncompressed|-F"  , "Print in full uncompressed format" },
	{ FORMATOPTION_machinereadable  , "--machinereadable|-m" , "Print output machine readable" },
	{ FORMATOPTION_quiet            , "--quiet|-q"         , "Be more quiet" },
	{ FORMATOPTION_printmirrored	, "--printmirrored"    , "Print output mirrored" },
	{ FORMATOPTION_forceprefix	, "--forceprefix <num>", "Force prefix to <num>" },
	{ FORMATOPTION_print_iid_var	, "--print-iid-var"	, "IPv6 address IID variance"},
	{ FORMATOPTION_no_prefixlength	, "--no-prefixlength"	, "Don't print prefix length if given on input"},
	{ FORMATOPTION_print_octal	, "--print-octal"       , "Print IPv4 address in dot-separated octal notation"},
};

const int ipv6calc_formatoptionstrings_entries = MAXENTRIES_ARRAY(ipv6calc_formatoptionstrings);


const s_action ipv6calc_actionstrings[] = {
	{ ACTION_auto                 , "auto"            , "Automatic selection of action (default)", "" },
	{ ACTION_mac_to_eui64         , "geneui64"        , "Converts a MAC address to an EUI-64 address", "" },
	{ ACTION_ipv4_to_6to4addr     , "conv6to4"        , "Converts IPv4 address <-> 6to4 IPv6 address (prefix)", "" },
	{ ACTION_ipv4_to_nat64        , "convnat64"       , "Converts IPv4 address <-> NAT64 IPv6 address", "" },
	{ ACTION_ipv6_to_mac          , "ipv6tomac"       , "Extracts MAC of an IPv6 address (if detected)", "" },
	{ ACTION_ipv6_to_eui64        , "ipv6toeui64"     , "Extracts EUI64 of an IPv6 address (if detected)", "" },
	{ ACTION_iid_token_to_privacy , "genprivacyiid"   , "Generates a privacy interface ID out of a given one (arg1) and a token (arg2)", "" },
	{ ACTION_prefix_mac_to_ipv6   , "prefixmac2ipv6"  , "Generates an IPv6 address out of a prefix and a MAC address", "" },
	{ ACTION_anonymize            , "anonymize"       , "Anonymize IPv4/IPv6 address without loosing much information", "" },
	{ ACTION_6rd_local_prefix     , "6rd_local_prefix", "Calculate the 6rd prefix from given IPv6 prefix (& relay prefix) and IPv4", "" },
	{ ACTION_6rd_extract_ipv4     , "6rd_extract_ipv4", "Extract from 6rd address the include IPv4 address", "" },
	{ ACTION_filter	              , "filter"          , "Filter addresses related to filter options", "" },
	{ ACTION_test                 , "test"            , "Test address against given prefix or address", "" },
	{ ACTION_addr_to_countrycode  , "addr2cc"         , "Converts IPv4/IPv6 address into Country Code", "" },
	{ ACTION_db_dump              , "dbdump"          , "Dump Database", "" },
};

const int ipv6calc_actionstrings_entries = MAXENTRIES_ARRAY(ipv6calc_actionstrings);


/* Format conversion matrix */
const uint32_t ipv6calc_formatmatrix[][2] = {
	{ FORMAT_auto           , 0x5ff },
	{ FORMAT_revnibbles_int , 0x5ff },
	{ FORMAT_revnibbles_arpa, 0x5ff },
	{ FORMAT_bitstring      , 0x5ff },
	{ FORMAT_ipv6addr       , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 | FORMAT_octal | FORMAT_ipv6literal | FORMAT_hex },
	{ FORMAT_ipv6literal    , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 | FORMAT_octal | FORMAT_ipv6literal },
	{ FORMAT_ipv4addr       , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex | FORMAT_hex | FORMAT_ipv6to4 },
	{ FORMAT_ipv4hex        , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex | FORMAT_hex | FORMAT_ipv6to4 },
	{ FORMAT_ipv4revhex     , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex | FORMAT_ipv6to4 },
	{ FORMAT_mac            , FORMAT_eui64 },
	{ FORMAT_eui64          , FORMAT_eui64 },
	{ FORMAT_base85         , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_ifinet6        , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_iid_token      , FORMAT_iid_token },
	{ FORMAT_ipv6logconv    , FORMAT_ipv6addrtype | FORMAT_addrtype | FORMAT_ouitype | FORMAT_any },
	{ FORMAT_prefix_mac     , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 },
	{ FORMAT_asn            , FORMAT_auto_noresult },
};

const int ipv6calc_formatmatrix_entries = MAXENTRIES_ARRAY(ipv6calc_formatmatrix);


/* Possible format option map */
const uint32_t ipv6calc_outputformatoptionmap[][2]  = {
	{ FORMAT_revnibbles_int , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printmirrored },
	{ FORMAT_revnibbles_arpa, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printmirrored },
	{ FORMAT_revipv4, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printmirrored },
	{ FORMAT_bitstring      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_ipv6addr       , FORMATOPTION_machinereadable | FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_forceprefix | FORMATOPTION_print_iid_var | FORMATOPTION_no_prefixlength},
	{ FORMAT_mac            , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_eui64          , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_base85         , 0 },
	{ FORMAT_ifinet6        , 0 },
	{ FORMAT_ipv4addr       , FORMATOPTION_machinereadable | FORMATOPTION_no_prefixlength | FORMATOPTION_forceprefix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_print_octal },
	{ FORMAT_iid_token      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_octal          , FORMATOPTION_printfulluncompressed },
	{ FORMAT_ipv6literal    , FORMATOPTION_machinereadable | FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed },
	{ FORMAT_hex            , FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_forceprefix },
	{ FORMAT_prefix_mac     , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase }
};

const int ipv6calc_outputformatoptionmap_entries = MAXENTRIES_ARRAY(ipv6calc_outputformatoptionmap);


/* Possible action option map (required) */
const uint32_t ipv6calc_actionoptionmap[][3]  = {
	// action, command, 0=required, 1=optional
	{ ACTION_auto			, 0 , 0},
	{ ACTION_mac_to_eui64		, 0 , 0},
	{ ACTION_ipv4_to_6to4addr	, 0 , 0},
	{ ACTION_ipv4_to_nat64		, 0 , 0},
	{ ACTION_ipv6_to_mac		, 0 , 0},
	{ ACTION_ipv6_to_eui64		, 0 , 0},
	{ ACTION_iid_token_to_privacy	, 0 , 0},
	{ ACTION_prefix_mac_to_ipv6	, 0 , 0},
	{ ACTION_anonymize		, 0 , 0},
	{ ACTION_addr_to_countrycode	, 0 , 0},
	{ ACTION_6rd_local_prefix	, CMD_6rd_prefix, 0},
	{ ACTION_6rd_local_prefix	, CMD_6rd_relay_prefix, 1 },
	{ ACTION_6rd_extract_ipv4	, CMD_6rd_prefixlength, 0},
	{ ACTION_test			, CMD_test_prefix, 1},
	{ ACTION_test			, CMD_test_gt, 1},
	{ ACTION_test			, CMD_test_ge, 1},
	{ ACTION_test			, CMD_test_lt, 1},
	{ ACTION_test			, CMD_test_le, 1},
};

const int ipv6calc_actionoptionmap_entries = MAXENTRIES_ARRAY(ipv6calc_actionoptionmap);


/*
 * check given type string
 * in : format string
 * ret: format type
 */
uint32_t ipv6calctypes_checktype(const char *string) {
	int i;
	uint32_t number = FORMAT_undefined;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Got string: %s", string);

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against: %s", ipv6calc_formatstrings[i].token);

		/* check main token */
		if (strcmp(string, ipv6calc_formatstrings[i].token) == 0) {
			number = ipv6calc_formatstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_formatstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against aliases in string: %s", ipv6calc_formatstrings[i].aliases);

		snprintf(tokenlist, sizeof(tokenlist), "%s", ipv6calc_formatstrings[i].aliases);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against alias token: %s", token);
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_formatstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( number == FORMAT_undefined ) {
		DEBUGPRINT_NA(DEBUG_libipv6calctypes, "Found no proper string");
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Found format number: %08x", (unsigned int) number);
	};

	return(number);
};


/*
 * check given type string
 * in : action string
 * ret: action type
 */
uint32_t ipv6calctypes_checkaction(const char *string) {
	int i;
	uint32_t number = ACTION_undefined;
	char tokenlist[100];
	char *token, *cptr, **ptrptr;

	ptrptr = &cptr;
	
	DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Got string: %s", string);

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_actionstrings); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against: %s", ipv6calc_actionstrings[i].token);

		/* check main token */
		if (strcmp(string, ipv6calc_actionstrings[i].token) == 0) {
			number = ipv6calc_actionstrings[i].number;
			break;
		};

		if (strlen(ipv6calc_actionstrings[i].aliases) == 0) {
			/* no aliases defined */
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against aliases in string: %s", ipv6calc_actionstrings[i].aliases);

		snprintf(tokenlist, sizeof(tokenlist), "%s", ipv6calc_formatstrings[i].aliases);

		token = strtok_r(tokenlist, " ", ptrptr);

		while (token != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Compare against alias token: %s", token);
			
			/* compare alias */
			if (strcmp(string, token) == 0) {
				number = ipv6calc_actionstrings[i].number;
				break;
			};

			/* get next token */
			token = strtok_r(NULL, " ", ptrptr);
		};
	};

	if ( number == FORMAT_undefined ) {
		DEBUGPRINT_NA(DEBUG_libipv6calctypes, "Found no proper string");
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Found action number: %08x", (unsigned int)  number);
	};

	return(number);
};


/*
 * return format string
 * in : format type
 * ret: format string
 */
const char *ipv6calctypes_format_string_by_type(const uint32_t format_type) {
	int i;
	const char *format_string = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calctypes, "Got type: %08x", format_type);

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
		if (ipv6calc_formatstrings[i].number == format_type) {
			format_string = ipv6calc_formatstrings[i].token;
			break;
		};
	};

	return(format_string);
};
