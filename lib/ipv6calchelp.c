/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.c
 * Version    : $Id: ipv6calchelp.c,v 1.26 2011/11/27 15:44:41 peter Exp $
 * Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"
#include "config.h"

/* to be defined in each application */
extern void printversion(void);
extern void printcopyright(void);


/* format option arguments */
void printhelp_print(void) {
	fprintf(stderr, "   --printprefix        : print only prefix of IPv6 address\n");
        fprintf(stderr, "   --printsuffix        : print only suffix of IPv6 address\n");
};

void printhelp_mask(void) {
	fprintf(stderr, "   --maskprefix         : mask IPv6 address with prefix length (clears suffix bits)\n");
        fprintf(stderr, "   --masksuffix         : mask IPv6 address with suffix length (clears prefix bits)\n");
};

void printhelp_case(void) {
	fprintf(stderr, "   --uppercase|-u       : print chars of IPv6 address in upper case\n");
        fprintf(stderr, "   --lowercase|-l       : print chars of IPv6 address in lower case [default]\n");
};

void printhelp_printstartend(void) {
	fprintf(stderr, "   --printstart <1-128> : print part of IPv6 address start from given number\n");
	fprintf(stderr, "   --printend   <1-128> : print part of IPv6 address end at given number\n");
};

void printhelp_doublecommands(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Only one command may be specified!\n");
};

void printhelp_missinginputdata(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Missing or to few input data given!\n");
};

/* list of input types */
void printhelp_inputtypes(const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	}

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available input types:\n");

	/* run through matrix */
	for (i = 0; i < (int) (sizeof(ipv6calc_formatmatrix) / sizeof(ipv6calc_formatmatrix[0])); i++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Row %d: %08x - %08x\n", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);
		};
		if (ipv6calc_formatmatrix[i][1] != 0) {
			/* available for input, look for name now */
			for (j = 0; j < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); j++) {
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				};
				if (ipv6calc_formatstrings[j].number == ipv6calc_formatmatrix[i][0]) {
					if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
						fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
					} else {
						fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token);
					};
				};
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* list of output types */
void printhelp_outputtypes(const uint32_t inputtype, const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
			fprintf(stderr, "\n Available output types filtered by input type:\n");
		} else {
			fprintf(stderr, "\n Available output types:\n");
		};
	};

	/* run through matrix */
	for (j = 0; j < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); j++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
		};

		for (i = 0; i < (int) (sizeof(ipv6calc_formatmatrix) / sizeof(ipv6calc_formatmatrix[0])); i++) {
			if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
				if (ipv6calc_formatmatrix[i][0] != inputtype) {
					/* skip */
					continue;
				};
			};
		
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "Row %d: %08x - %08x\n", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);
			};

			if ((ipv6calc_formatmatrix[i][1] & ipv6calc_formatstrings[j].number) != 0) {
				/* available for output, look for name now */
				if (strlen(ipv6calc_formatstrings[j].explanation) > 0) {
					fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				} else {
					fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token, "(empty)");
				};
				break;
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		fprintf(stderr, "\n For examples and available format options use:\n");
		fprintf(stderr, "    --out <type> --examples\n");
		fprintf(stderr, "\n");
	};
};


/* list of action types */
void printhelp_actiontypes(const uint32_t formatoptions) {
	int i, j, o, test = 2, has_options = 0;
	size_t maxlen = 0;
	char printformatstring[20], printformatstring2[20], printformatstring3[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); j++) {
		if (strlen(ipv6calc_actionstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_actionstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_actionstrings'!\n");
		exit (EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	};

	snprintf(printformatstring2, sizeof(printformatstring2) - 1, "  %%-%ds%%s\n", (int) maxlen + 4);
	snprintf(printformatstring3, sizeof(printformatstring3) - 1, "  %%-%ds--%%s", (int) maxlen + 5);

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available action types:\n");

	for (j = 0; j < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); j++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_actionstrings[j].number, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);
		};

		if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
			fprintf(stderr, printformatstring, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);

			while (test != 0) {
				if (test == 1) {	
					fprintf(stderr, printformatstring2, "", "Required options:");
				};

				/* search for defined options */
				for (i = 0; i < (int) (sizeof(ipv6calc_actionoptionmap) / sizeof(ipv6calc_actionoptionmap[0])); i++) {
					if (ipv6calc_debug != 0) {
						fprintf(stderr, "Option %d\n", i);
					};

					if (ipv6calc_actionstrings[j].number == ipv6calc_actionoptionmap[i][0]) {
						if (ipv6calc_actionoptionmap[i][1] == 0) {
							/* no options supported */
							break;
						};
						
						if (ipv6calc_debug != 0) {
							fprintf(stderr, "Option value: %08x\n", (unsigned int) ipv6calc_actionoptionmap[i][1]);
						};

						/* run through options */
						for (o = 0; o < (int) (sizeof(ipv6calc_longopts) / sizeof (ipv6calc_longopts[0])); o++) {
							if ((ipv6calc_actionoptionmap[i][1] == ipv6calc_longopts[o].val)) {
								has_options = 1;
								if (test == 1) {	
									fprintf(stderr, printformatstring3, "", ipv6calc_longopts[o].name);
									if (ipv6calc_longopts[o].has_arg > 0) {
										fprintf(stderr, " ...");
									};
									fprintf(stderr, "\n");
								};
							};
						};
					};
				};
				if (has_options == 0) {
					break;
				};
				test--;
			};
		} else {
			fprintf(stdout, printformatstring, ipv6calc_actionstrings[j].token);
		};
	};
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* print global help */
void printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, " General:\n");
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "  [-q|--quiet]               : be more quiet (auto-enabled in pipe mode)\n");
	fprintf(stderr, "  [-f|--flush]               : flush each line in pipe mode\n");
	fprintf(stderr, "  -v                         : show version (and included features)\n");
	fprintf(stderr, "  -v -v                      : show verbose version information\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Usage with new style options:\n");
	fprintf(stderr, "  [--in|-I <input type>]   : specify input  type\n");
	fprintf(stderr, "                             (default: autodetect)\n");
	fprintf(stderr, "  [--out|-O <output type>] : specify output type\n");
	fprintf(stderr, "                             (sometimes: autodetect)\n");
	fprintf(stderr, "  [--action|-A <action>]   : specify action\n");
	fprintf(stderr, "                             (default: format conversion, sometimes: autodetect)\n");
	fprintf(stderr, "  [<format option> ...] : specify format options\n");
	fprintf(stderr, "  <input data> [...]    : input data\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Available input  types:  [-m] --in     -?|-h|--help\n");
	fprintf(stderr, "  Available output types:  [-m] --out    -?|-h|--help\n");
	fprintf(stderr, "  Available action types:  [-m] --action -?|-h|--help\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Other usage:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  --showinfo|-i [--machine_readable|-m] : show information about input data\n");
	fprintf(stderr, "  --showinfo|-i --show_types            : show available types on '-m'\n");
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, "\n");
	fprintf(stderr, "  [--db-ip2location-ipv4 <file>] : IP2Location IPv4 database file (optional)\n");
	fprintf(stderr, "  [--db-ip2location-ipv6 <file>] : IP2Location IPv6 database file (optional)\n");
#ifdef IP2LOCATION_DEFAULT_FILE_IPV4
	fprintf(stderr, "  [--db-ip2location-ipv4-default|-L] : enable & use IP2Location IPv4 default database file (optional)\n");
	fprintf(stderr, "                                        %s\n", IP2LOCATION_DEFAULT_FILE_IPV4);
#endif
#ifdef IP2LOCATION_DEFAULT_FILE_IPV6
	fprintf(stderr, "  [--db-ip2location-ipv6-default|-L] : enable & use IP2Location IPv6 default database file (optional)\n");
	fprintf(stderr, "                                        %s\n", IP2LOCATION_DEFAULT_FILE_IPV6);
#endif
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, "\n");
	fprintf(stderr, "  [--db-geoip-ipv4 <file>]  : GeoIP IPv4 database file (optional)\n");
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, "  [--db-geoip-ipv6 <file>]  : GeoIP IPv6 database file (optional)\n");
#endif
#ifdef GEOIP_DEFAULT_FILE_IPV4
	fprintf(stderr, "  [--db-geoip-ipv4-default|-G] : enable & use GeoIP IPv4 default database file (optional)\n");
	fprintf(stderr, "                                  %s\n", GEOIP_DEFAULT_FILE_IPV4);
#endif
#ifdef GEOIP_DEFAULT_FILE_IPV6
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, "  [--db-geoip-ipv6-default|-G] : enable & use GeoIP IPv6 default database file (optional)\n");
	fprintf(stderr, "                                  %s\n", GEOIP_DEFAULT_FILE_IPV6);
#endif
#endif
#endif
	fprintf(stderr, "\n");
	fprintf(stderr, " To see old-style option use: --printoldoptions\n");
	fprintf(stderr, "\n");

	return;
};

void printhelp_oldoptions(void) {
	int i;
	
	printversion();
	printcopyright();
	fprintf(stderr, " Usage with old style (shortcut) options (going obsolete):\n");
	fprintf(stderr, "  <shortcut option> [<format option> ...] <input data> [...]\n");
	fprintf(stderr, "\n");
	for (i = 0; i < (int) (sizeof(ipv6calc_longopts) / sizeof(ipv6calc_longopts[0])); i++) {
		if (ipv6calc_longopts[i].val >= CMD_shortcut_start && ipv6calc_longopts[i].val <= CMD_shortcut_end) {
			fprintf(stderr, "  --%s\n", ipv6calc_longopts[i].name);
		};
	};
	
	fprintf(stderr, "\n");
	return;
};


/* print help for output type examples */


static void printhelp_output_base85(void) {
	fprintf(stderr, " Print a given IPv6 address in base85 format (RFC 1924), e.g.\n");
	fprintf(stderr, "  1080:0:0:0:8:800:200c:417a -> 4)+k&C#VzJ4br>0wv%%Yp\n");
};

static void printhelp_output_bitstring(void) {
	fprintf(stderr, " Print a given IPv6 address as a bitstring label for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff::1    -> \\[x3ffeffff000000000000000000000001/128].ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff::1/64 -> \\[x3ffeffff000000000000000000000001/64].ip6.arpa.\n");
	fprintf(stderr, "  --printsuffix 3ffe:ffff::1/64 -> \\[x0000000000000001/64]\n");
	fprintf(stderr, "  --printprefix 3ffe:ffff::1/64 -> \\[x3ffeffff00000000/64]\n");
};

static void printhelp_output_ipv6addr(void) {
	fprintf(stderr, " Print a given IPv6 address depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1    -> 3ffe:ffff:100:f101:0:0:0:1\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 3ffe:ffff:100:f101:0:0:0:1/64\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffe:ffff:0100:f101:0000:0000:0000:0001\n");
	fprintf(stderr, "  Compressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:0100:f101:0000:0000:0000:0001 -> 3ffe:ffff:100:f101::1\n");
};

static void printhelp_output_ipv6literal(void) {
	fprintf(stderr, " Print a given IPv6 address in literal format depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8-0-0-0-0-0-1.ipv6-literal.net\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net\n");
	fprintf(stderr, "  Compressed (default), e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8--1.ipv6-literal.net\n");
	fprintf(stderr, "  With Scope ID, e.g.\n");
	fprintf(stderr, "   fe80::1%%0    -> fe80--1s0.ipv6-literal.net\n");
};

static void printhelp_output_eui64(void) {
	fprintf(stderr, " Print a generated EUI-64 identifier, e.g.:\n");
	fprintf(stderr, "  00:50:BF:06:B4:F5 -> 0250:bfff:fe06:b4f5\n");
};

static void printhelp_output_iid_token(void) {
	fprintf(stderr, " Print generated interface identifier and token, e.g.:\n");
	fprintf(stderr, "  -> 4462:bdea:8654:776d 486072ff7074945e\n");
};

static void printhelp_output_revnibble_int(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
};

static void printhelp_output_revnibble_arpa(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
};

static void printhelp_output_ifinet6void(void) {
	fprintf(stderr, " Print a given IPv6 address to same format shown in Linux /proc/net/if_inet6:\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1    -> 3ffeffff0100f1010000000000000001 00\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64 -> 3ffeffff0100f1010000000000000001 00 40\n");
};

static void printhelp_output_ipv4addr(void) {
	fprintf(stderr, " Print an IPv4 address\n");
};

static void printhelp_output_revipv4(void) {
	fprintf(stderr, " Print an IPv4 address in reverse format for PTR/DNS\n");
	fprintf(stderr, "  1.2.3.4  -> 4.3.2.1.in-addr.arpa\n");
};

static void printhelp_output_addrtype(void) {
	fprintf(stderr, " Print type of a given IPv4/IPv6 address:\n");
	fprintf(stderr, "  IPv4 address  -> ipv4-addr.addrtype.ipv6calc\n");
	fprintf(stderr, "  IPv6 address  -> ipv6-addr.addrtype.ipv6calc\n");
};

static void printhelp_output_ipv6addrtype(void) {
	fprintf(stderr, " Print type of a given IPv6 address:\n");
	fprintf(stderr, "  3ffe::/16         -> 6bone-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2002::/16         -> 6to4-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2001::/16         -> productive-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fe80::/10         -> link-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fec0::/10         -> site-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::ffff:0:0:0:0/96 -> mapped-ipv4.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::0:0:0:0/96      -> compat-ipv4.ipv6addrtype.ipv6calc\n");
};

static void printhelp_output_ouitype(void) {
	fprintf(stderr, " Print OUI name of a given IPv6 address:\n");
	fprintf(stderr, "  IID local scope   -> local-scoppe.ouitype.ipv6calc\n");
	fprintf(stderr, "  IID global scope  -> vendorname.ouitype.ipv6calc\n");
};

static void printhelp_output_octal(void) {
	fprintf(stderr, " Print octal representation of a given IPv6 address:\n");
	fprintf(stderr, "  (useful for djbdns/tinydns)\n");
	fprintf(stderr, "  3ffe:ffff::1 ->\n    \\77\\376\\377\\377\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\1\n");
	fprintf(stderr, "  --printfulluncompressed 3ffe:ffff::1 ->\n    \\077\\376\\377\\377\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\001\n");
};

void printhelp_output_dispatcher(const uint32_t outputtype) {
	int i, j;

	printversion();
	
	fprintf(stderr, "\n");

	switch (outputtype) {
		case FORMAT_base85:
			printhelp_output_base85();
			break;

		case FORMAT_bitstring:
			printhelp_output_bitstring();
			break;
			
		case FORMAT_ipv6addr:
			printhelp_output_ipv6addr();
			break;
			
		case FORMAT_ipv6literal:
			printhelp_output_ipv6literal();
			break;
			
		case FORMAT_eui64:
			printhelp_output_eui64();
			break;

		case FORMAT_revnibbles_int:
			printhelp_output_revnibble_int();
			break;

		case FORMAT_revnibbles_arpa:
			printhelp_output_revnibble_arpa();
			break;

		case FORMAT_ifinet6:
			printhelp_output_ifinet6void();
			break;
			
		case FORMAT_iid_token:
			printhelp_output_iid_token();
			break;

		case FORMAT_ipv4addr:
			printhelp_output_ipv4addr();
			break;

		case FORMAT_addrtype:
			printhelp_output_addrtype();
			break;

		case FORMAT_ipv6addrtype:
			printhelp_output_ipv6addrtype();
			break;
			
		case FORMAT_ouitype:
			printhelp_output_ouitype();
			break;
			
		case FORMAT_revipv4:
			printhelp_output_revipv4();
			break;
			
		case FORMAT_octal:
			printhelp_output_octal();
			break;
			
		default:
			fprintf(stderr, " Examples currently missing...!\n");
			break;
	};
	
	/* looking for outtype */
	for (i = 0; i < (int) (sizeof(ipv6calc_outputformatoptionmap) / sizeof(ipv6calc_outputformatoptionmap[0])); i++) {
		if (outputtype == ipv6calc_outputformatoptionmap[i][0]) {
			if (ipv6calc_outputformatoptionmap[i][1] == 0) {
				fprintf(stderr, " No format options supported\n");
				break;
			};
			
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "Format value: %08x\n", (unsigned int) ipv6calc_outputformatoptionmap[i][1]);
			};
			
			fprintf(stderr, "\n");
			fprintf(stderr, " Available format options:\n");

			/* run through format options */
			for (j = 0; j < (int) (sizeof(ipv6calc_formatoptionstrings) / sizeof (ipv6calc_formatoptionstrings[0])); j++) {
				if ((ipv6calc_outputformatoptionmap[i][1] & ipv6calc_formatoptionstrings[j].number) != 0) {
					fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
				};
			};
			break;
		};
	};
	fprintf(stderr, "\n");
};
