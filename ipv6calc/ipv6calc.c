/*
 * Project    : ipv6calc
 * File       : ipv6calc/ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.122 2015/05/08 06:26:11 ds6peter Exp $
 * Copyright  : 2001-2015 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IP(v4/v6)/MAC address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "config.h"

#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calcoptions_local.h"
#include "ipv6calchelp.h"
#include "ipv6calchelp_local.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"
#include "libeui64.h"

#include "showinfo.h"

#include "librfc1884.h"
#include "librfc1886.h"
#include "librfc1924.h"
#include "libifinet6.h"
#include "librfc2874.h"
#include "librfc3056.h"
#include "librfc3041.h"
#include "librfc5569.h"
#include "libeui64.h"
#include "libieee.h"
#include "libipv4addr.h"
#include "libipv6addr.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"

/* 
 *  * API_VERSION is defined as a bareword in IP2Location.h, 
 *   * we need this trick to stringify it. Blah.
 *    */
#define makestr(x) #x
#define xmakestr(x) makestr(x)
#endif


#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#endif


long int ipv6calc_debug = 0; // ipv6calc_debug usage ok

/* pipe support */
int input_is_pipe = 0;
#define LINEBUFFER      16384

/* anonymization default values */
s_ipv6calc_anon_set ipv6calc_anon_set;

/* features */
int feature_zeroize = 1; // always supported
int feature_anon    = 1; // always supported
int feature_kp      = 0; // will be checked later

/* showinfo machine readable filter */
char showinfo_machine_readable_filter[32] = "";
int showinfo_machine_readable_filter_used = 0;


/**************************************************/
/* main */
int main(int argc, char *argv[]) {
	/* check compatibility */
#ifdef SHARED_LIBRARY
	IPV6CALC_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
	IPV6CALC_DB_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
#endif // SHARED_LIBRARY

	char resultstring[NI_MAXHOST] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, j, lop, result;
	extern int optopt;
	uint32_t command = 0;
	int bit_start = 0, bit_end = 0, force_prefix = 0;
	char *input1 = NULL, *input2 = NULL;
	int inputc;
	int inputtype_given = 0, outputtype_given = 0, action_given = 0;

	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined;
	uint32_t outputtype = FORMAT_undefined;
	uint32_t action     = ACTION_undefined;

	/* format options storage */
	uint32_t formatoptions = 0;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr, ipv4addr2;
	ipv6calc_macaddr  macaddr;
	ipv6calc_eui64addr  eui64addr;

	/* used simple data */
	uint32_t asn;
	int asn_valid = 0;

	/* filter structure */
	s_ipv6calc_filter_master filter_master;
	libipv6calc_filter_clear(&filter_master);

	/* IID statistics */
	s_iid_statistics iid_statistics;
	int iid_random_result;

	/* clear address structures */
	ipv6addr_clearall(&ipv6addr);
	ipv6addr_clearall(&ipv6addr2);
	ipv6addr_clearall(&ipv6addr3);
	ipv6addr_clearall(&ipv6addr4);
	ipv4addr_clearall(&ipv4addr);
	mac_clearall(&macaddr);

	/* pipe mode */
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char *charptr, *cptr, **ptrptr;
	ptrptr = &cptr;
	int linecounter = 0;
	int flush_mode = 0;

	/* options */
	struct option longopts[MAXLONGOPTIONS];
	char   shortopts[NI_MAXHOST] = "";
	int    longopts_maxentries = 0;

	/* initialize debug value from environment for bootstrap debugging */
	ipv6calc_debug_from_env(); // ipv6calc_debug usage ok

	/* add options */
	ipv6calc_options_add_common_anon(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add(shortopts, sizeof(shortopts), longopts, &longopts_maxentries, ipv6calc_shortopts, ipv6calc_longopts, MAXENTRIES_ARRAY(ipv6calc_longopts));

	/* default */
	result = libipv6calc_anon_set_by_name(&ipv6calc_anon_set, ANONPRESET_DEFAULT);
	if (result != 0) {
		fprintf(stderr, "major problem, ipv6calc anonymization default preset not found: %s", ANONPRESET_DEFAULT);
		exit(EXIT_FAILURE);
	};

	if (argc <= 1) {
		ipv6calc_printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	DEBUGSECTION_BEGIN(DEBUG_ipv6calc_general) 
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Start parsing options: shortopts=%s", shortopts); 
		i = 0;
		while(longopts[i].name != NULL) {
			DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Long option: %s/%d/%08x", longopts[i].name, longopts[i].has_arg, longopts[i].val);
			i++;
		};
	DEBUGSECTION_END
	
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Parsing option: 0x%08x", i);

		if ((i == '?') && (optopt != 0)) {
			exit(EXIT_FAILURE);
		};

		/* catch "common basic" options */
		result = ipv6calcoptions_common_basic(i, optarg, longopts);
		if (result == 0) {
			// found
			continue;
		};

		/* catch "common anon" options */
		result = ipv6calcoptions_common_anon(i, optarg, longopts, &ipv6calc_anon_set);
		if (result == 0) {
			// found
			continue;
		};

		/* specific options */
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Parsing option locally: 0x%08x", i);

		switch (i) {
			case -1:
				DEBUGPRINT_NA(DEBUG_ipv6calc_general, "end of options");
				break;

			case 'v':
				if ((command & CMD_printversion_verbose) != 0) {
					// 3rd time '-v'
					command |= CMD_printversion_verbose2;
				} else if ((command & CMD_printversion) != 0) {
					// 2nd time '-v'
					command |= CMD_printversion_verbose;
				} else {
					command |= CMD_printversion;
				};
				break;

			case 'f':
				flush_mode = 1;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				DEBUGPRINT_NA(DEBUG_ipv6calc_general, "help option detected");
				break;
				
			case 'L':
				fprintf(stderr, "Option is obsolete: -L\n");
				break;

			case DB_ip2location_ipv4_default:
				fprintf(stderr, "Option is obsolete: --db-ip2location-ipv4-default\n");
				break;

			case DB_ip2location_ipv6_default:
				fprintf(stderr, "Option is obsolete: --db-ip2location-ipv6-default\n");
				break;

			case 'G':
				fprintf(stderr, "Option is obsolete: -G\n");
				break;

			case DB_geoip_ipv4_default:
				fprintf(stderr, "Option is obsolete: --db-geoip-ipv4-default\n");
				break;

			case DB_geoip_ipv6_default:
				fprintf(stderr, "Option is obsolete: --db-geoip-ipv6-default\n");
				break;

			case CMD_printexamples:
				command = CMD_printexamples;
				break;

			/* backward compatibility/shortcut commands */
			case 'r':
			case CMD_addr_to_ip6int:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_auto;
				outputtype = FORMAT_revnibbles_int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_auto;
				outputtype = FORMAT_revnibbles_arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_addr_to_uncompressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case CMD_addr_to_base85:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_base85;
				break;
				
			case CMD_base85_to_addr:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_base85;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;

			case CMD_mac_to_eui64:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_mac;
				outputtype = FORMAT_eui64;
				action = ACTION_mac_to_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ifinet6;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_ipv4_to_6to4addr:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ipv4addr;
				outputtype = FORMAT_ipv6addr;
				action = ACTION_ipv4_to_6to4addr;
				break;
				
			case CMD_eui64_to_privacy:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_iid_token;
				outputtype = FORMAT_iid_token;
				action = ACTION_iid_token_to_privacy;
				break;

			case 'i':
			case CMD_showinfo:
				command = CMD_showinfo;
				break;

			case CMD_showinfotypes:
				command = CMD_showinfotypes;
				break;

			case CMD_6rd_relay_prefix:
				retval = addr_to_ipv4addrstruct(optarg, resultstring, sizeof(resultstring), &ipv4addr2);
				break;

			case CMD_6rd_prefix:
				retval = addr_to_ipv6addrstruct(optarg, resultstring, sizeof(resultstring), &ipv6addr);
				break;

			/* format options */
			case 'C':
			case FORMATOPTION_NUM_printcompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case 'U':
			case FORMATOPTION_NUM_printuncompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case 'F':
			case FORMATOPTION_NUM_printfulluncompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case FORMATOPTION_NUM_printprefix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printprefix;
				break;
				
			case FORMATOPTION_NUM_printsuffix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printsuffix;
				break;
				
			case FORMATOPTION_NUM_maskprefix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_maskprefix;
				break;
				
			case FORMATOPTION_NUM_masksuffix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_masksuffix;
				break;
				
			case 'l':	
			case FORMATOPTION_NUM_printlowercase + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printlowercase;
				break;
				
			case 'u':	
			case FORMATOPTION_NUM_printuppercase + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printuppercase;
				break;
				
			case FORMATOPTION_NUM_forceprefix + FORMATOPTION_NUM_HEAD:
				DEBUGPRINT_NA(DEBUG_ipv6calc_general, "format option 'forceprefix' selected");
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					force_prefix = atoi(optarg);
					formatoptions |= FORMATOPTION_forceprefix;
				} else {
					fprintf(stderr, " Argument of option 'forceprefix' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_NUM_printstart + FORMATOPTION_NUM_HEAD:
				DEBUGPRINT_NA(DEBUG_ipv6calc_general, "format option 'printstart' selected");
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_NUM_printend + FORMATOPTION_NUM_HEAD:
				DEBUGPRINT_NA(DEBUG_ipv6calc_general, "format option 'printend' selected");
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;

			case FORMATOPTION_NUM_mr_quote_always + FORMATOPTION_NUM_HEAD:
				formatoptions |= (FORMATOPTION_mr_quote_always | FORMATOPTION_machinereadable);
				break;

			case FORMATOPTION_NUM_mr_quote_never + FORMATOPTION_NUM_HEAD:
				formatoptions |= (FORMATOPTION_mr_quote_never | FORMATOPTION_machinereadable);
				break;

			case FORMATOPTION_NUM_mr_value_only + FORMATOPTION_NUM_HEAD:
				if (strlen(showinfo_machine_readable_filter) >= sizeof(showinfo_machine_readable_filter)) {
					fprintf(stderr, " Argument of option 'mrtvo' is too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				snprintf(showinfo_machine_readable_filter, sizeof(showinfo_machine_readable_filter), "%s", optarg);
				formatoptions |= (FORMATOPTION_mr_select_token | FORMATOPTION_mr_value_only | FORMATOPTION_machinereadable);
				break;
				
			case FORMATOPTION_NUM_mr_select_token + FORMATOPTION_NUM_HEAD:
				if (strlen(showinfo_machine_readable_filter) >= sizeof(showinfo_machine_readable_filter)) {
					fprintf(stderr, " Argument of option 'mrst' is too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				snprintf(showinfo_machine_readable_filter, sizeof(showinfo_machine_readable_filter), "%s", optarg);
				formatoptions |= (FORMATOPTION_mr_select_token | FORMATOPTION_machinereadable);
				break;

			case FORMATOPTION_NUM_mr_select_token_pa + FORMATOPTION_NUM_HEAD:
				if (strlen(showinfo_machine_readable_filter) >= sizeof(showinfo_machine_readable_filter)) {
					fprintf(stderr, " Argument of option 'mrstpa' is too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				snprintf(showinfo_machine_readable_filter, sizeof(showinfo_machine_readable_filter), "%s", optarg);
				formatoptions |= (FORMATOPTION_mr_select_token | FORMATOPTION_mr_select_token_pa | FORMATOPTION_machinereadable);
				break;

			case FORMATOPTION_NUM_mr_match_token + FORMATOPTION_NUM_HEAD:
				if (strlen(showinfo_machine_readable_filter) >= sizeof(showinfo_machine_readable_filter)) {
					fprintf(stderr, " Argument of option 'mrmt' is too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				snprintf(showinfo_machine_readable_filter, sizeof(showinfo_machine_readable_filter), "%s", optarg);
				formatoptions |= (FORMATOPTION_mr_match_token | FORMATOPTION_machinereadable);
				break;


			case 'm':	
			case FORMATOPTION_NUM_machinereadable + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_machinereadable;
				break;
				
			case FORMATOPTION_NUM_quiet + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_quiet;
				break;
				
			case FORMATOPTION_NUM_printmirrored + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printmirrored;
				break;

			case FORMATOPTION_NUM_print_iid_var + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_print_iid_var;
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;

			/* new options */
			case 'I':	
			case CMD_inputtype:
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Got input string: %s", optarg);
				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					inputtype = FORMAT_auto;
					command = CMD_printhelp;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype == FORMAT_undefined) {
					fprintf(stderr, " Input option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				inputtype_given = 1;
				break;	
				
			case 'O':	
			case CMD_outputtype:
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Got output string: %s", optarg);
				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					outputtype = FORMAT_auto;
					command = CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype == FORMAT_undefined) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				outputtype_given = 1;
				break;	

			case 'A':	
			case CMD_actiontype:
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Got action string: %s", optarg);

				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					action = ACTION_auto;
					command = CMD_printhelp;
					break;
				};
				action = ipv6calctypes_checkaction(optarg);
				if (action == ACTION_undefined) {
					fprintf(stderr, " Action option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				action_given = 1;
				break;

			case 'E':	
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Got expression string: %s", optarg);

				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					action = ACTION_filter;
					command = CMD_printhelp;
					break;
				};

				result = libipv6calc_filter_parse(optarg, &filter_master);

				if (result != 0) {
					exit(EXIT_FAILURE);
				};

				if (action == ACTION_undefined) {
					// autodefine action
					action = ACTION_filter;
					action_given = 1;
				};
				break;
				
			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				ipv6calc_printhelp(longopts, ipv6calc_longopts_shortopts_map);
				exit(EXIT_FAILURE);
		};
	};
	argv += optind;
	argc -= optind;

	if (ipv6calc_quiet != 0) {
		formatoptions |= FORMATOPTION_quiet;
	};

	/* initialise database wrapper */
	result = libipv6calc_db_wrapper_init("");
	if (result != 0) {
		exit(EXIT_FAILURE);
	};

	/* check for KeepTypeAsnCC support */
	if ((libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 1) \
	    && (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1)) {
		feature_kp = 1;
	};

	/* print help handling */
	if ((outputtype_given == 1) && (command & CMD_printhelp)) {
		command = CMD_printexamples;
	};

	if (command == CMD_printhelp) {
		if ( (outputtype == FORMAT_undefined) && (inputtype == FORMAT_undefined) && (action == ACTION_undefined)) {
			ipv6calc_printhelp(longopts, ipv6calc_longopts_shortopts_map);
			exit(EXIT_FAILURE);
		} else if (outputtype == FORMAT_auto) {
			if (inputtype == FORMAT_undefined) {
				inputtype = FORMAT_auto;
			};
			printhelp_outputtypes(inputtype, formatoptions);
			exit(EXIT_FAILURE);
		} else if (inputtype == FORMAT_auto) {
			if (outputtype == FORMAT_undefined) {
				printhelp_inputtypes(formatoptions);
			} else {
				printhelp_output_dispatcher(outputtype);
			};
			exit(EXIT_FAILURE);
		} else if (action == ACTION_auto) {
			printhelp_actiontypes(formatoptions, ipv6calc_longopts);
			exit(EXIT_FAILURE);
		} else if (action != ACTION_undefined) {
			printhelp_action_dispatcher(action, 0);
			exit(EXIT_FAILURE);
		};

	} else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	} else if (command == CMD_showinfotypes) {
		showinfo_availabletypes();
		exit(EXIT_FAILURE);
	};

	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Debug value:%08lx command:%08lx inputtype:%08lx outputtype:%08lx action:%08lx formatoptions:%08lx", (unsigned long) ipv6calc_debug, (unsigned long) command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action, (unsigned long) formatoptions); // ipv6calc_debug usage ok

	/* do work depending on selection */
	if ((command & CMD_printversion) != 0) {
		if ((command & CMD_printversion_verbose) != 0) {
			printversion_verbose(((command & CMD_printversion_verbose2) !=0) ? LEVEL_VERBOSE2 : LEVEL_VERBOSE);
		} else {
			printversion();
		};

		if ((command & CMD_printhelp) != 0) {
			printversion_help();
		};

		exit(EXIT_SUCCESS);
	};

	if (action == ACTION_anonymize) {
		/* check requirements */
		if (libipv6calc_anon_supported(&ipv6calc_anon_set) == 0) {
			fprintf(stderr, "ipv6calc anonymization method not supported: %s\n", libipv6calc_anon_method_name(&ipv6calc_anon_set));
			exit(EXIT_FAILURE);
		};
	};

	if (action == ACTION_filter) {
		if (libipv6calc_filter_check(&filter_master) != 0) {
			fprintf(stderr, "ipv6calc filter check causes a problem\n");
			exit(EXIT_FAILURE);
		};
	};

	if (argc > 0) {
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Got input %s", argv[0]);
	} else {
		if (isatty (STDIN_FILENO)) {
		} else {
			input_is_pipe = 1;
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Input is a pipe");
			formatoptions |= FORMATOPTION_quiet; // force quiet mode
		};
	};

	/***** input type handling *****/
	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of input type handling");

	/* check length of input */
	if (argc > 0) {
		if (input_is_pipe == 1) {
			fprintf(stderr, "Additional input found in pipe mode\n");
			exit(EXIT_FAILURE);
		};
		for (i = 0; i < argc; i++) {
			if ( argv[i] != NULL ) {
				if ( strlen(argv[i]) >= NI_MAXHOST ) {
					/* that's not good for size limited buffers...*/
					fprintf(stderr, "Argument %d is too long\n", i + 1);
					exit(EXIT_FAILURE);
				};
			};
		};
	};

	/* loop for pipe */
	if (input_is_pipe == 1) {
PIPE_input:
                /* read line from stdin */
                charptr = fgets(linebuffer, LINEBUFFER, stdin);

		if (charptr == NULL) {
			/* end of input */
			exit(retval);
		};

		linecounter++;

		if (linecounter == 1) {
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Ok, proceeding stdin...");
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Line: %d", linecounter);

		if (strlen(linebuffer) >= NI_MAXHOST) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			exit(EXIT_FAILURE);
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			goto PIPE_input;
		};

		/* remove trailing \n */
		if (linebuffer[strlen(linebuffer) - 1] == '\n') {
			linebuffer[strlen(linebuffer) - 1] = '\0';
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "stdin got line: '%s'", linebuffer);

		if (linebuffer[strlen(linebuffer) - 1] == '\n') {
			/* remove trailing \n */
			linebuffer[strlen(linebuffer) - 1] = '\0';
		};

		/* look for first token */
		charptr = strtok_r(linebuffer, " \t\n", ptrptr);

		if ( charptr == NULL ) {
			fprintf(stderr, "Line contains no token: %d\n", linecounter);
			goto PIPE_input;
		};

		if ( strlen(charptr) >=  LINEBUFFER) {
			fprintf(stderr, "Line too strange: %d\n", linecounter);
			goto PIPE_input;
		};

		snprintf(token, sizeof(token), "%s", charptr);
		
		input1 = token;
		inputc = 1;

		DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Token 1: '%s'", input1);

		/* check for second token */
		if (*ptrptr[0] != '\0') {
			input2 = *ptrptr;
			inputc = 2;

			DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Token 2: '%s'", input2);
		};

	} else {
		inputc = argc;
		if (argc >= 1) {
			input1 = argv[0];
		};
		if (argc >= 2) {
			input2 = argv[1];
		};
	};
	
	/* reset input type in case of action=filter and pipe mode */
	if ((input_is_pipe == 1) && (action_given == 1) && (action == ACTION_filter)) {
		DEBUGPRINT_NA(DEBUG_ipv6calc_general, "reset input type for later autodetection");
		inputtype = FORMAT_auto;
	};

	/* autodetection */
	if ((inputtype == FORMAT_undefined || inputtype == FORMAT_auto) && inputc > 0) {
		DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Call input type autodetection");
		/* no input type specified or automatic selected */
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No input type specified, try autodetection...");
		};
		
		inputtype = libipv6calc_autodetectinput(input1);

		if ( inputtype != FORMAT_undefined ) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
					};
					break;
				};
			};
		} else {
			fprintf(stderr, "no result!\n");
		};
	} else {
		DEBUGPRINT_NA(DEBUG_ipv6calc_general, "input type autodetection skipped");
	};

	/* auto set of output type*/
	if (inputtype == FORMAT_revnibbles_int || inputtype == FORMAT_revnibbles_arpa || inputtype == FORMAT_ipv6literal) {
		if ( outputtype == FORMAT_undefined ) {
			outputtype = FORMAT_ipv6addr;
		};
	};

	/***** automatic action handling *****/
	if (action == ACTION_undefined) {
		if ( (inputtype == FORMAT_mac && (input_is_pipe == 0 || inputtype_given == 1)) && (outputtype == FORMAT_eui64 && outputtype_given == 1) ) {
			action = ACTION_mac_to_eui64;
		} else if ( (inputtype == FORMAT_iid_token && (input_is_pipe == 0 || inputtype_given == 1)) && (outputtype == FORMAT_iid_token && outputtype_given == 1) ) {
			action = ACTION_iid_token_to_privacy;
		} else if ( inputtype == FORMAT_prefix_mac && (input_is_pipe == 0 || inputtype_given == 1)) {
			action = ACTION_prefix_mac_to_ipv6;
			if ( outputtype == FORMAT_undefined ) {
				outputtype = FORMAT_ipv6addr;
			};
		};
	};

	if (input_is_pipe == 0 && action_given == 0) {
		if ( action != ACTION_undefined ) {
			if ((formatoptions & FORMATOPTION_quiet) == 0) {
				fprintf(stderr, "No action type specified, try autodetection...");
			};
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_actionstrings); i++) {
				if (action == ipv6calc_actionstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_actionstrings[i].token);
					};
					break;
				};
			};
		};
	};

	/* check formatoptions for validity */
	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "check format option of output type: 0x%08lx", (unsigned long) outputtype);
		
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_outputformatoptionmap); i++) {
		if (outputtype != ipv6calc_outputformatoptionmap[i][0]) {
			continue;
		};

		if ( (ipv6calc_outputformatoptionmap[i][1] & (formatoptions & ~FORMATOPTION_quiet) ) == (formatoptions & ~FORMATOPTION_quiet ) ) {
			/* all options valid */
			break;
		};
		
		if ((input_is_pipe == 1) && ((formatoptions & FORMATOPTION_print_iid_var) == FORMATOPTION_print_iid_var)) {
			/* workaround */
			continue;
		};
		fprintf(stderr, " Unsupported format option(s):\n");

		/* run through format options */
		for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatoptionstrings); j++) {
			if ( (ipv6calc_formatoptionstrings[j].number & FORMATOPTION_quiet) != 0 ) {
				/* skip format option "quiet" on check */
				continue;
			};
			if ((((~ ipv6calc_outputformatoptionmap[i][1]) & formatoptions) & ipv6calc_formatoptionstrings[j].number) != 0) {
				fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
			};
		};
		exit(EXIT_FAILURE);
	};
	
	/* proceed input depending on type */	
	retval = -1; /* default */

	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Process input (inputtype: 0x%08lx, count=%d)", (unsigned long) inputtype, inputc);

	switch (inputtype) {
		case FORMAT_ipv6addr:
		case FORMAT_ipv6literal:
		case FORMAT_ipv4addr:
		case FORMAT_ipv4hex:
		case FORMAT_ipv4revhex:
		case FORMAT_base85:
		case FORMAT_mac:
		case FORMAT_eui64:
		case FORMAT_ifinet6:
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
		case FORMAT_bitstring:
			if (inputc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			break;

		case FORMAT_iid:
			if (inputc == 2) {
				inputtype = FORMAT_iid_token;
				if (outputtype == FORMAT_undefined) {
					outputtype = FORMAT_iid_token;
				};
			};
			break;

		case FORMAT_iid_token:
			if (inputc < 2) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			if (outputtype == FORMAT_undefined) {
				outputtype = FORMAT_iid_token;
			};
			break;

		case FORMAT_prefix_mac:
			if (inputc < 2) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			break;

		case FORMAT_asn:
			if (command != CMD_showinfo) {
				fprintf(stderr, " Input-type only supported for 'showinfo'\n");
				exit(EXIT_FAILURE);
			};
			break;

		case FORMAT_auto_noresult:
		default:
			fprintf(stderr, " Input-type isn't autodetected\n");
			exit(EXIT_FAILURE);
	};

	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv6literal:
			retval = addrliteral_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(input1, resultstring, sizeof(resultstring), &ipv4addr);
			argc--;
			break;
			
		case FORMAT_ipv4hex:
			retval = addrhex_to_ipv4addrstruct(input1, resultstring, sizeof(resultstring), &ipv4addr, 0);
			argc--;
			break;

		case FORMAT_ipv4revhex:
			retval = addrhex_to_ipv4addrstruct(input1, resultstring, sizeof(resultstring), &ipv4addr, 1);
			argc--;
			break;

		case FORMAT_base85:
			retval = base85_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			argc--;
			break;
			
		case FORMAT_mac:
			retval = mac_to_macaddrstruct(input1, resultstring, sizeof(resultstring), &macaddr);
			argc--;
			break;

		case FORMAT_eui64:
			retval = libeui64_addr_to_eui64addrstruct(input1, resultstring, sizeof(resultstring), &eui64addr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (inputc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(input1, input2, resultstring, sizeof(resultstring), &ipv6addr);
			};
			break;

		case FORMAT_iid_token:
			/* Get first interface identifier */
			retval = identifier_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			if (retval != 0) { break; };
			
			/* Get second token */
			retval = tokenlsb64_to_ipv6addrstruct(input2, resultstring, sizeof(resultstring), &ipv6addr2);
			break;
			
		case FORMAT_prefix_mac:
			/* Get first: IPv6 prefix */
			retval = addr_to_ipv6addrstruct(input1, resultstring, sizeof(resultstring), &ipv6addr);
			if (retval != 0) { break; };
			
			/* Get second: MAC address */
			retval = mac_to_macaddrstruct(input2, resultstring, sizeof(resultstring), &macaddr);
			break;

		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			retval = librfc1886_nibblestring_to_ipv6addrstruct(input1, &ipv6addr, resultstring, sizeof(resultstring));
			argc--;
			break;
			
		case FORMAT_bitstring:
			retval = librfc2874_bitstring_to_ipv6addrstruct(input1, &ipv6addr, resultstring, sizeof(resultstring));
			argc--;
			break;

		case FORMAT_asn:
			retval = sscanf(input1, "%d", &asn);
			if (retval == 0) {
				retval = -1;
				snprintf(resultstring, sizeof(resultstring), "error parsing ASN");
			} else {
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "ASN parsed: %d", asn);
				asn_valid = 1;
				retval = 0;
			};
			argc--;
			break;
			
		case FORMAT_auto_noresult:
			fprintf(stderr, " Input-type isn't autodetected\n");
			exit(EXIT_FAILURE);

		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			exit(EXIT_FAILURE);
	};

	if (retval != 0) {
		if (strlen(resultstring) > 0) fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "End of input type handling");

	/***** postprocessing input *****/
	
	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of postprocessing input");

	if (ipv4addr.flag_valid == 1) {
		/* force prefix */
		if ((formatoptions & (FORMATOPTION_forceprefix)) != 0) {
			ipv4addr.flag_prefixuse = 1;
			ipv4addr.prefixlength = force_prefix;
		};
	};

	if (ipv6addr.flag_valid == 1) {
		/* force prefix */
		if ((formatoptions & (FORMATOPTION_forceprefix)) != 0) {
			ipv6addr.flag_prefixuse = 1;
			ipv6addr.prefixlength = force_prefix;
		};
		
		/* mask bits */
		if ((formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) != 0) {
			if (ipv6addr.flag_prefixuse == 1) {
				if ((formatoptions & FORMATOPTION_maskprefix) != 0) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if ((formatoptions & FORMATOPTION_masksuffix) != 0) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit(EXIT_FAILURE);
			};
		};
		
		/* start bit */
		if ((formatoptions & FORMATOPTION_printstart) != 0) {
			DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Set bit start to: %d", bit_start);
			ipv6addr.bit_start = (uint8_t) bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if ((formatoptions & FORMATOPTION_printend) != 0) {
			DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Set bit end to: %d", bit_end);
			ipv6addr.bit_end = (uint8_t) bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};

		/* prefix+suffix */
		if ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit(EXIT_FAILURE);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%u' lower than given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than or eqal to given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else if (ipv6addr.prefixlength >= ipv6addr.bit_start) {
							fprintf(stderr, " Error: prefix length '%u' higher than or equal to given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						};
					};
				};
			};
		};

		/* check start/end */
		if ( ipv6addr.flag_startend_use == 1 ) {
			if ( ipv6addr.bit_start > ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit bigger than end bit\n");
				exit(EXIT_FAILURE);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit(EXIT_FAILURE);
			};
		};
	};

	
	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "result of 'inputtype': %d", retval);

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "End of postprocessing input");
	
	/***** action *****/
	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of action");

	/***** automatic output handling *****/
	if ( outputtype == FORMAT_undefined ) {
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No output type specified, try autodetection...");
		};

		if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_ipv4_to_6to4addr) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_mac) && (action == ACTION_mac_to_eui64) ) {
			outputtype = FORMAT_eui64;
			formatoptions |= FORMATOPTION_printfulluncompressed;
		} else if ( (inputtype == FORMAT_ipv6addr) && ( (action == ACTION_undefined) || (action == ACTION_anonymize) ) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_anonymize) ) {
			outputtype = FORMAT_ipv4addr;
		} else if ( ((inputtype == FORMAT_ipv4addr) || (inputtype == FORMAT_ipv4hex) || (inputtype == FORMAT_ipv4revhex)) && (action == ACTION_undefined || action == ACTION_anonymize) ) {
			outputtype = FORMAT_ipv4addr;
		} else if ( (inputtype == FORMAT_mac) ) {
			outputtype = FORMAT_mac;
		} else if ( (inputtype == FORMAT_eui64) ) {
			outputtype = FORMAT_eui64;
		} else if ( (inputtype == FORMAT_bitstring) || (inputtype == FORMAT_base85) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_6rd_local_prefix) ) {
			outputtype = FORMAT_ipv6addr;
		};

		if ( outputtype != FORMAT_undefined ) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
				if (outputtype == ipv6calc_formatstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
					};
					break;
				};
			};
		} else {
			if ((formatoptions & FORMATOPTION_quiet) == 0) {
				fprintf(stderr, "no result!\n");
			};
		};
	};

	if ( (outputtype & (FORMAT_revnibbles_int | FORMAT_revnibbles_arpa)) != 0 ) {
		/* workaround for reverse IPv4 */
		 if (ipv6addr.flag_valid == 0 && ipv4addr.flag_valid == 1) {
			outputtype = FORMAT_revipv4;
		};
	};

	/* clear resultstring */
	snprintf(resultstring, sizeof(resultstring), "%s", "");

	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Process action (action: 0x%08lx)", (unsigned long) action);
	
	switch (action) {
		case ACTION_mac_to_eui64:
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(EXIT_FAILURE);
			};
			retval = create_eui64_from_mac(&ipv6addr, &macaddr);
			break;
			
		case ACTION_ipv4_to_6to4addr:
			if (inputtype == FORMAT_ipv4addr && outputtype == FORMAT_ipv6addr) {
				/* IPv4 -> IPv6 */
				if (ipv4addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv4 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv4addr_to_ipv6to4addr(&ipv6addr, &ipv4addr);
			} else if (inputtype == FORMAT_ipv6addr && outputtype == FORMAT_ipv4addr) {
				/* IPv6 -> IPv4 */
				if (ipv6addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv6 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv6addr_to_ipv4addr(&ipv4addr, &ipv6addr, resultstring, sizeof(resultstring));
			} else {
				fprintf(stderr, "Unsupported 6to4 conversion!\n");
				exit(EXIT_FAILURE);
			};
			break;

		case ACTION_anonymize:
			if ((inputtype == FORMAT_ipv4addr || inputtype == FORMAT_ipv4hex || inputtype == FORMAT_ipv4revhex) && outputtype == FORMAT_ipv4addr) {
				/* anonymize IPv4 address */
				if (ipv4addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv4 address given!\n");
					exit(EXIT_FAILURE);
				};
				libipv4addr_anonymize(&ipv4addr, ipv6calc_anon_set.mask_ipv4, ipv6calc_anon_set.method);
			} else if ((inputtype == FORMAT_ipv6addr || inputtype == FORMAT_bitstring || inputtype == FORMAT_revnibbles_int || inputtype == FORMAT_revnibbles_arpa || inputtype == FORMAT_base85 || inputtype == FORMAT_ipv6literal) && outputtype == FORMAT_ipv6addr) {
				/* anonymize IPv6 address */
				if (ipv6addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv6 address given!\n");
					exit(EXIT_FAILURE);
				};
				libipv6addr_anonymize(&ipv6addr, &ipv6calc_anon_set);
			} else if (inputtype == FORMAT_mac && outputtype == FORMAT_mac) {
				/* anonymize MAC address */
				if (macaddr.flag_valid != 1) {
					fprintf(stderr, "No valid MAC address given!\n");
					exit(EXIT_FAILURE);
				};
				libmacaddr_anonymize(&macaddr, &ipv6calc_anon_set);
			} else if (inputtype == FORMAT_eui64 && outputtype == FORMAT_eui64) {
				/* anonymize EUI-64 address */
				if (eui64addr.flag_valid != 1) {
					fprintf(stderr, "No valid EUI-64 address given!\n");
					exit(EXIT_FAILURE);
				};
				libeui64_anonymize(&eui64addr, &ipv6calc_anon_set);
			} else {
				fprintf(stderr, "Unsupported anonymization!\n");
				exit(EXIT_FAILURE);
			};
			break;
			
		case ACTION_iid_token_to_privacy:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) {
				fprintf(stderr, "No valid interface identifiers given!\n");
				exit(EXIT_FAILURE);
			};
			retval = librfc3041_calc(&ipv6addr, &ipv6addr2, &ipv6addr3, &ipv6addr4);
			ipv6addr_copy(&ipv6addr, &ipv6addr3);
			ipv6addr_copy(&ipv6addr2, &ipv6addr4);
			break;

		case ACTION_prefix_mac_to_ipv6:
			/* check IPv6 prefix */
			if ( ipv6addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv6 address given!\n");
				exit(EXIT_FAILURE);
			};

			/* check MAC */
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(EXIT_FAILURE);
			};
			
			/* check for empty IID, otherwise display warning */
			for ( i = 8; i <= 15; i++ ) {
				if (ipv6addr.in6_addr.s6_addr[i] != 0) {
					fprintf(stderr, "Warning: given prefix is not 0 in 64 LSBs!\n");
					break;	
				};
			};
			
			/* convert MAC to IID */
			retval = create_eui64_from_mac(&ipv6addr2, &macaddr);

			/* put IID into address */
			for ( i = 8; i <= 15; i++ ) {
				ipv6addr.in6_addr.s6_addr[i] = ipv6addr2.in6_addr.s6_addr[i];
			};
			break;

		case ACTION_6rd_local_prefix:
			/* check formats */
			if ( outputtype != FORMAT_ipv6addr ) {
				fprintf(stderr, "output type incompatible (no IPv6 address)!\n");
				exit(EXIT_FAILURE);
			};
	
			if ( inputtype != FORMAT_ipv4addr ) {
				fprintf(stderr, "input type incompatible (no IPv4 address)!\n");
				exit(EXIT_FAILURE);
			};
	
			/* check IPv6 prefix */
			if ( ipv6addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv6 address given as 6rd prefix!\n");
				exit(EXIT_FAILURE);
			}

			if ( !ipv6addr.flag_prefixuse ) {
				fprintf(stderr, "Specified IPv6 address has no prefix length!\n");
				exit(EXIT_FAILURE);
			}

			/* check IPv4 local ip */
			if ( ipv4addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv4 address given as local IP!\n");
				exit(EXIT_FAILURE);
			}
		
			if ( ipv4addr.flag_prefixuse ) {
				fprintf(stderr, "No prefix length should be specified for the local IP!\n");
				exit(EXIT_FAILURE);
			}

			/* check IPv4 relay prefix */
			if ( ipv4addr2.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv4 address given as 6rd relay prefix!\n");
				exit(EXIT_FAILURE);
			}

			if ( !ipv4addr2.flag_prefixuse ) {
				fprintf(stderr, "Specified relay prefix IPv4 address has no prefix length!\n");
				exit(EXIT_FAILURE);
			}

			retval = librfc5569_calc_6rd_local_prefix(&ipv6addr, &ipv4addr2, &ipv4addr, resultstring, sizeof(resultstring));
			break;

		case ACTION_filter:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of action: filter");

			outputtype = inputtype;

			result = 1; /* default, skip */
			if (inputtype == FORMAT_ipv4addr) {
				/* call filter for IPv4 addresses */
				result = ipv4addr_filter(&ipv4addr, &filter_master.filter_ipv4addr);
			} else if (inputtype == FORMAT_ipv6addr) {
				/* call filter for IPv6 addresses */
				result = ipv6addr_filter(&ipv6addr, &filter_master.filter_ipv6addr);
			} else if (inputtype == FORMAT_mac) {
				/* call filter for MAC addresses */
				result = macaddr_filter(&macaddr, &filter_master.filter_macaddr);
			} else {
				/* TODO: more specific notice */
				fprintf(stderr, "Action-type isn't currently implemented for inputtype\n");
			};

			if (result > 0) {
				/* skip everything */
				DEBUGPRINT_WA(DEBUG_ipv6calc_general, "filter result SKIP (%d): '%s'", result, linebuffer);
			} else {
				snprintf(resultstring, sizeof(resultstring), "%s", linebuffer);
			};
			goto RESULT_print;

			break;

		case ACTION_undefined:
			/* no action selected */
			break;

		default:
			fprintf(stderr, " Action-type isn't implemented\n");
			exit(EXIT_FAILURE);
			/* break; */
	};

	if (retval != 0) {
		fprintf(stderr, "Problem occurs during action: %s\n", resultstring);
		exit(EXIT_FAILURE);
	};

	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "End of action");

	/***** output type *****/
	DEBUGPRINT_WA(DEBUG_ipv6calc_general, "Process output (outputtype: 0x%08lx)", (unsigned long) outputtype);
	
	/* catch showinfo */	
	if (command == CMD_showinfo) {
		// change anonymization method depending on support
		if (feature_kp == 1) {
			result = libipv6calc_anon_set_by_name(&ipv6calc_anon_set, "keep-type-asn-cc");
			if (result != 0) {
				// fallback
				result = libipv6calc_anon_set_by_name(&ipv6calc_anon_set, ANONPRESET_DEFAULT);
			};
		};

		if (ipv6addr.flag_valid == 1) {
			retval = showinfo_ipv6addr(&ipv6addr, formatoptions);
	       	} else if (ipv4addr.flag_valid == 1) {
			retval = showinfo_ipv4addr(&ipv4addr, formatoptions);
	       	} else if (macaddr.flag_valid == 1) {
			retval = showinfo_eui48(&macaddr, formatoptions);
	       	} else if (eui64addr.flag_valid == 1) {
			retval = showinfo_eui64(&eui64addr, formatoptions);
	       	} else if (asn_valid == 1) {
			retval = showinfo_asn(asn, formatoptions);
		} else {
		       	fprintf(stderr, "No valid or supported input address given!\n");
			retval = 1;
		};
		if (retval != 0) {
			fprintf(stderr, "Problem occurs during selection of showinfo\n");
			exit(EXIT_FAILURE);
		};
		goto RESULT_print;
	};

	switch (outputtype) {
		case -1:
			/* old implementation */
			break;

		case FORMAT_base85:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_base85");
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = ipv6addrstruct_to_base85(&ipv6addr, resultstring, sizeof(resultstring));
			break;
				
		case FORMAT_bitstring:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_bitstring");
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Call 'librfc2874_addr_to_bitstring'");
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			break;
			
		case FORMAT_octal:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_octal");
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_octal(&ipv4addr, resultstring, sizeof(resultstring), formatoptions);
			} else if (ipv6addr.flag_valid == 1) {
				retval = libipv6addr_to_octal(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			};
			break;
				
		case FORMAT_hex:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_hex");
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_hex(&ipv4addr, resultstring, sizeof(resultstring), formatoptions);
			} else if (ipv6addr.flag_valid == 1) {
				retval = libipv6addr_to_hex(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			};
			break;

		case FORMAT_ipv4hex:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_ipv4hex");
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_hex(&ipv4addr, resultstring, sizeof(resultstring), formatoptions);
			};
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_revnibbles_int/arpa");
			if (ipv6addr.flag_valid == 1) {
				switch (outputtype) {
					case FORMAT_revnibbles_int:
						retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, sizeof(resultstring), formatoptions, "ip6.int.");
						break;
					case FORMAT_revnibbles_arpa:
						retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, sizeof(resultstring), formatoptions, "ip6.arpa.");
						break;
				};
			} else {
			       fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE);
			};
			break;
				
		case FORMAT_revipv4:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_revipv4");
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_reversestring(&ipv4addr, resultstring, sizeof(resultstring), formatoptions);
			} else {
			       fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE);
		       	};
			break;
			
		case FORMAT_ifinet6:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_ifinet6");
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = libifinet6_ipv6addrstruct_to_ifinet6(&ipv6addr, resultstring, sizeof(resultstring));
			break;

		case FORMAT_ipv6addr:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_ipv6addr");
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			};

			if ((formatoptions & FORMATOPTION_print_iid_var) == FORMATOPTION_print_iid_var) {
				iid_random_result = ipv6addr_iidrandomdetection(&ipv6addr, &iid_statistics);
				sprintf(resultstring2, "%-40s R=%d h=%7.3f r=%7.3f db= %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d dbh= %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d da= %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d dd= %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d dda= %d", resultstring, \
					iid_random_result, iid_statistics.hexdigit, iid_statistics.lls_residual, \
					iid_statistics.digit_blocks[0], iid_statistics.digit_blocks[1], iid_statistics.digit_blocks[2], iid_statistics.digit_blocks[3], iid_statistics.digit_blocks[4], iid_statistics.digit_blocks[5], iid_statistics.digit_blocks[6], iid_statistics.digit_blocks[7], iid_statistics.digit_blocks[8], iid_statistics.digit_blocks[9], iid_statistics.digit_blocks[10], iid_statistics.digit_blocks[11], iid_statistics.digit_blocks[12], iid_statistics.digit_blocks[13], iid_statistics.digit_blocks[14], iid_statistics.digit_blocks[15], \
					iid_statistics.digit_blocks_hexdigit[0], iid_statistics.digit_blocks_hexdigit[1], iid_statistics.digit_blocks_hexdigit[2], iid_statistics.digit_blocks_hexdigit[3], iid_statistics.digit_blocks_hexdigit[4], iid_statistics.digit_blocks_hexdigit[5], iid_statistics.digit_blocks_hexdigit[6], iid_statistics.digit_blocks_hexdigit[7], iid_statistics.digit_blocks_hexdigit[8], iid_statistics.digit_blocks_hexdigit[9], iid_statistics.digit_blocks_hexdigit[10], iid_statistics.digit_blocks_hexdigit[11], iid_statistics.digit_blocks_hexdigit[12], iid_statistics.digit_blocks_hexdigit[13], iid_statistics.digit_blocks_hexdigit[14], iid_statistics.digit_blocks_hexdigit[15], \
					iid_statistics.digit_amount[0], iid_statistics.digit_amount[1], iid_statistics.digit_amount[2], iid_statistics.digit_amount[3], iid_statistics.digit_amount[4], iid_statistics.digit_amount[5], iid_statistics.digit_amount[6], iid_statistics.digit_amount[7], iid_statistics.digit_amount[8], iid_statistics.digit_amount[9], iid_statistics.digit_amount[10], iid_statistics.digit_amount[11], iid_statistics.digit_amount[12], iid_statistics.digit_amount[13], iid_statistics.digit_amount[14], iid_statistics.digit_amount[15], \
					iid_statistics.digit_delta[0], iid_statistics.digit_delta[1], iid_statistics.digit_delta[2], iid_statistics.digit_delta[3], iid_statistics.digit_delta[4], iid_statistics.digit_delta[5], iid_statistics.digit_delta[6], iid_statistics.digit_delta[7], iid_statistics.digit_delta[8], iid_statistics.digit_delta[9], iid_statistics.digit_delta[10], iid_statistics.digit_delta[11], iid_statistics.digit_delta[12], iid_statistics.digit_delta[13], iid_statistics.digit_delta[14], iid_statistics.digit_delta[15], \
					iid_statistics.digit_delta[16], iid_statistics.digit_delta[17], iid_statistics.digit_delta[18], iid_statistics.digit_delta[19], iid_statistics.digit_delta[20], iid_statistics.digit_delta[21], iid_statistics.digit_delta[22], iid_statistics.digit_delta[23], iid_statistics.digit_delta[24], iid_statistics.digit_delta[25], iid_statistics.digit_delta[26], iid_statistics.digit_delta[27], iid_statistics.digit_delta[28], iid_statistics.digit_delta[29], iid_statistics.digit_delta[30], \
					iid_statistics.digit_delta_amount
				);
				sprintf(resultstring, "%s", resultstring2);
			};
			break;

		case FORMAT_ipv6literal:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_ipv6literal");
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed)) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, sizeof(resultstring), formatoptions | FORMATOPTION_literal);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, sizeof(resultstring), formatoptions | FORMATOPTION_literal);
			};
			break;
			
			
		case FORMAT_ipv4addr:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_ipv4addr");
			if (ipv4addr.flag_valid != 1) { fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE); };
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, sizeof(resultstring), formatoptions);
			break;

		case FORMAT_eui64:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_eui64");

			if (action == ACTION_mac_to_eui64) {
				if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid EUI-64 identifier given!\n"); exit(EXIT_FAILURE); };
				if (ipv6addr.prefixlength != 64) { fprintf(stderr, "No valid EUI-64 identifier given!\n"); exit(EXIT_FAILURE); };
				formatoptions |= FORMATOPTION_printsuffix;
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, sizeof(resultstring), formatoptions);
			} else if (action == ACTION_anonymize) {
				retval = libeui64_eui64addrstruct_to_string(&eui64addr, resultstring, sizeof(resultstring), formatoptions);
			} else {
				fprintf(stderr, "Specify proper action or input for output format: eui64\n"); exit(EXIT_FAILURE);
			};
			break;
			
		case FORMAT_mac:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_mac");
			if (macaddr.flag_valid != 1) { fprintf(stderr, "No valid MAC address given!\n"); exit(EXIT_FAILURE); };
			retval = macaddrstruct_to_string(&macaddr, resultstring, sizeof(resultstring), formatoptions);
			break;
			
		case FORMAT_iid_token:
			DEBUGPRINT_NA(DEBUG_ipv6calc_general, "Start of output handling for FORMAT_iid_token");
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) { fprintf(stderr, "No valid IPv6 addresses given!\n"); exit(EXIT_FAILURE); };
			/* get interface identifier */
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring2, sizeof(resultstring2), formatoptions | FORMATOPTION_printsuffix);
			if (retval != 0) { break; };
			
			/* get token */
			retval = libipv6addr_ipv6addrstruct_to_tokenlsb64(&ipv6addr2, resultstring3, sizeof(resultstring3), formatoptions);
			
			/* cat together */
			snprintf(resultstring, sizeof(resultstring), "%s %s", resultstring2, resultstring3);
			break;


		default:
			fprintf(stderr, " Outputtype isn't implemented: %8lx\n", (unsigned long) outputtype);
			exit(EXIT_FAILURE);
	};

	DEBUGPRINT_NA(DEBUG_ipv6calc_general, "End of output handling");

RESULT_print:
	/* print result */
	if (strlen(resultstring) > 0) {
		if ( retval == 0 ) {
			fprintf(stdout, "%s\n", resultstring);
		} else {
			fprintf(stderr, "%s\n", resultstring);
		};
	};

	if (input_is_pipe == 1) {
		if (flush_mode == 1) {
			fflush(stdout);
		};
		goto PIPE_input;
	};

	libipv6calc_db_wrapper_cleanup();

	exit(retval);
};
