/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.15 2002/03/03 12:55:42 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IPv6 address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h> 


#include "ipv6calc.h"
#include "libipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calchelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"

#include "showinfo.h"

#include "librfc1884.h"
#include "librfc1886.h"
#include "librfc1924.h"
#include "libifinet6.h"
#include "librfc2874.h"
#include "librfc3056.h"
#include "librfc3041.h"
#include "libeui64.h"

long int ipv6calc_debug = 0;


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, lop;
	unsigned long int command = 0;
	unsigned short bit_start = 0, bit_end = 0;

	/* new option style storage */	
	long int inputtype = -1, outputtype = -1;
	
	/* convert storage */
	long int action = -1;

	/* format options storage */
	unsigned int formatoptions = 0;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_macaddr  macaddr;

	/* define short options */
	char *shortopts = "vh?rmabd:iul";

	struct option longopts[] = {
		{"version", 0, 0, 'v'},
		{"debug", 1, 0, 'd'},

		/* help options */
		{"help", 0, 0, 'h'},
		{"examples"     , 0, 0, CMD_printexamples },
		{"printexamples", 0, 0, CMD_printexamples },

		
		/* backward compatibility/shortcut commands */
		{"addr2ip6_int", 0, 0, CMD_addr_to_ip6int },
		{"addr_to_ip6int", 0, 0, CMD_addr_to_ip6int },

		{"addr2ip6_arpa", 0, 0, CMD_addr_to_ip6arpa },
		{"addr_to_ip6arpa", 0, 0, CMD_addr_to_ip6arpa },
		
		{"addr_to_bitstring", 0, 0, CMD_addr_to_bitstring },

		{"addr2compaddr", 0, 0, CMD_addr_to_compressed },
		{"addr_to_compressed", 0, 0, CMD_addr_to_compressed },
		{"addr2uncompaddr", 0, 0, CMD_addr_to_uncompressed },
		{"addr_to_uncompressed", 0, 0, CMD_addr_to_uncompressed },
		
		{"addr_to_base85", 0, 0, CMD_addr_to_base85 },
		{"base85_to_addr", 0, 0, CMD_base85_to_addr },

		{"mac_to_eui64", 0, 0, CMD_mac_to_eui64 },
		
		{ "addr2fulluncompaddr", 0, 0, CMD_addr_to_fulluncompressed },
		{ "addr_to_fulluncompressed", 0, 0, CMD_addr_to_fulluncompressed },
		{ "addr2if_inet6", 0, 0, CMD_addr_to_ifinet6 },
		{ "addr_to_ifinet6", 0, 0, CMD_addr_to_ifinet6 },
		{ "if_inet62addr", 0, 0, CMD_ifinet6_to_compressed },
		{ "ifinet6_to_compressed", 0, 0, CMD_ifinet6_to_compressed },

		{ "eui64_to_privacy", 0, 0, CMD_eui64_to_privacy },
		
		{ "ipv4_to_6to4addr", 0, 0, CMD_ipv4_to_6to4addr },
		
		{ "showinfo", 0, 0, CMD_showinfo },
		{ "show_types", 0, 0, CMD_showinfotypes },

		/* format options */
		{ "maskprefix"           , 0, 0, FORMATOPTION_maskprefix + FORMATOPTION_HEAD },
		{ "masksuffix"           , 0, 0, FORMATOPTION_masksuffix + FORMATOPTION_HEAD },
		
		{ "uppercase"            , 0, 0, FORMATOPTION_printuppercase + FORMATOPTION_HEAD },
		{ "lowercase"            , 0, 0, FORMATOPTION_printlowercase + FORMATOPTION_HEAD },
		
		{ "printstart"           , 1, 0, FORMATOPTION_printstart + FORMATOPTION_HEAD },
		{ "printend"             , 1, 0, FORMATOPTION_printend + FORMATOPTION_HEAD },
		
		{ "printprefix"          , 0, 0, FORMATOPTION_printprefix + FORMATOPTION_HEAD },
		{ "printsuffix"          , 0, 0, FORMATOPTION_printsuffix + FORMATOPTION_HEAD },
		
		{ "printcompressed"      , 0, 0, FORMATOPTION_printcompressed       + FORMATOPTION_HEAD },
		{ "printuncompressed"    , 0, 0, FORMATOPTION_printuncompressed     + FORMATOPTION_HEAD },
		{ "printfulluncompressed", 0, 0, FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD },
		
		{ "machine_readable"     , 0, 0, FORMATOPTION_machinereadable + FORMATOPTION_HEAD },

		/* new options */
		{ "inputtype" , 1, 0, CMD_inputtype  },
		{ "intype"    , 1, 0, CMD_inputtype  },
		{ "outputtype", 1, 0, CMD_outputtype },
		{ "outtype"   , 1, 0, CMD_outputtype },
		{ "action"    , 1, 0, CMD_actiontype },
		{ "actiontype", 1, 0, CMD_actiontype },

		{NULL, 0, 0, 0}
	};                

	if (argc <= 1) {
		printinfo();
		exit(1);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;
			case 'v':
				command |= CMD_printversion;
				break;
			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case CMD_printexamples:
				command |= CMD_printexamples | CMD_printhelp;
				break;


			/* backward compatibility/shortcut commands */
			case 'r':
			case CMD_addr_to_ip6int:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_addr_to_uncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case CMD_addr_to_base85:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_base85;
				break;
				
			case CMD_base85_to_addr:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_base85;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;

			case CMD_mac_to_eui64:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_mac;
				outputtype = FORMAT_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype = FORMAT_ifinet6;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_ipv4_to_6to4addr:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype = FORMAT_ipv4addr;
				outputtype = FORMAT_ipv6addr;
				action = ACTION_ipv4_to_6to4addr;
				break;
				
			case CMD_eui64_to_privacy:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype = FORMAT_iid_token;
				outputtype = FORMAT_iid_token;
				break;

			case 'i':
			case CMD_showinfo:
				inputtype  = FORMAT_ipv6addr;
				command = CMD_showinfo;
				break;

			case CMD_showinfotypes:
				command = CMD_showinfotypes;
				break;

			/* format options */
			case FORMATOPTION_printcompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case FORMATOPTION_printuncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case FORMATOPTION_printprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printprefix;
				break;
				
			case FORMATOPTION_printsuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printsuffix;
				break;
				
			case FORMATOPTION_maskprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_maskprefix;
				break;
				
			case FORMATOPTION_masksuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_masksuffix;
				break;
				
			case 'l':	
			case FORMATOPTION_printlowercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printlowercase;
				break;
				
			case 'u':	
			case FORMATOPTION_printuppercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printuppercase;
				break;
				
			case FORMATOPTION_printstart + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit (1);
				};
				break;
				
			case FORMATOPTION_printend + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit (1);
				};
				break;
				
			case 'm':	
			case FORMATOPTION_machinereadable + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_machinereadable;
				break;

			/* new options */
			case CMD_inputtype:
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got input string: %s\n", DEBUG_function_name, optarg);
				};

				if (strcmp(optarg, "-?") == 0) {
					inputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype < 0) {
					fprintf(stderr, " Input option is unknown: %s\n", optarg);
					exit (1);
				};
				break;	
				
			case CMD_outputtype:
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got output string: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					outputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype < 0) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit (1);
				};
				break;	

			case CMD_actiontype:
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got action string: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					action = -2;
					command = CMD_printhelp;
					break;
				};
				action = ipv6calctypes_checkaction(optarg);
				if (action < 0) {
					fprintf(stderr, " Action option is unknown: %s\n", optarg);
					exit (1);
				};
				break;
				
			default:
				fprintf(stderr, "Usage: (see '%s --command -?' for more help)\n", PROGRAM_NAME);
				printhelp();
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* print help handling */
	if (command == CMD_printhelp) {
		if (outputtype == -2) {
			printhelp_outputtypes(inputtype);
			exit(1);
		} else if (inputtype == -2) {
			printhelp_inputtypes();
			exit(1);
		} else if (action == -2) {
			printhelp_actiontypes();
			exit(1);
		};

		if (command & CMD_printexamples) {
			printhelp_output_dispatcher(outputtype);
			exit(1);
		};
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value: %lx  command: %lx  inputtype: %lx   outputtype: %lx  action: %lx\n", ipv6calc_debug, command, inputtype, outputtype, action); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(1);
	};

	if (command == CMD_printhelp) {
		printhelp();
		exit (1);
	};
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
	};

	/***** automatic action handling *****/
	if (inputtype == FORMAT_mac && outputtype ==FORMAT_eui64) {
		action = ACTION_mac_to_eui64;
	} else if (inputtype == FORMAT_iid_token && outputtype ==FORMAT_iid_token) {;
		action = ACTION_iid_token_to_privacy;
	};

	/***** input type handling *****/
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Start of input type handling\n", DEBUG_function_name);
	};

	switch (inputtype) {
		case -1:
			/* old implementation */
			goto OUTPUT_type;
			break;

		case FORMAT_ipv6addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv4addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = addr_to_ipv4addrstruct(argv[0], resultstring, &ipv4addr);
			argc--;
			break;

		case FORMAT_base85:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = base85_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;
			
		case FORMAT_mac:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = mac_to_macaddrstruct(argv[0], resultstring, &macaddr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			if (argc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
				argc--;
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(argv[0], argv[1], resultstring, &ipv6addr);
				argc -= 2;
			};
			break;

		case FORMAT_iid_token:
			/* Get first interface identifier */
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = identifier_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			if (retval != 0) { break; };
			
			/* Get second token */
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = tokenlsb64_to_ipv6addrstruct(argv[1], resultstring, &ipv6addr2);
			argc--;
			break;
			
		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			exit (1);
			break;
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit (1);
	};
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: End of input type handling\n", DEBUG_function_name);
	};

	/***** postprocessing input *****/
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	if (ipv6addr.flag_valid == 1) {
		/* mask bits */
		if (formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) {
			if (ipv6addr.flag_prefixuse == 1) {
				if (formatoptions & FORMATOPTION_maskprefix) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if (formatoptions & FORMATOPTION_masksuffix) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit (1);
			};
		};
		
		/* start bit */
		if (formatoptions & FORMATOPTION_printstart) {
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
			};
			ipv6addr.bit_start = bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if (formatoptions & FORMATOPTION_printend ) {
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
			};
			ipv6addr.bit_end = bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};
		
		/* prefix+suffix */
		if (formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix) ) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit (1);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if (formatoptions & FORMATOPTION_printprefix ) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if (formatoptions & FORMATOPTION_printsuffix ) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if (formatoptions & FORMATOPTION_printprefix) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%d' lower than given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit (1);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit (1);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if (formatoptions & FORMATOPTION_printsuffix) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than or eqal to given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit (1);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%d' higher than or equal to given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit (1);
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
				exit (1);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit (1);
			};
		};
	};

	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: result of 'inputtype': %d\n", DEBUG_function_name, retval);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit (1);
	};
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: End of postprocessing input\n", DEBUG_function_name);
	};
	
	/***** action *****/
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Start of action\n", DEBUG_function_name);
	};
	
	switch (action) {
		case ACTION_mac_to_eui64:
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(1);
			};
			retval = create_eui64_from_mac(&ipv6addr, &macaddr);
			break;
			
		case ACTION_ipv4_to_6to4addr:
			if (ipv4addr.flag_valid != 1) {
				fprintf(stderr, "No valid IPv4 address given!\n");
				exit(1);
			};
			retval = ipv4addr_to_ipv6to4addr(&ipv6addr, &ipv4addr);
			break;
			
		case ACTION_iid_token_to_privacy:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) {
				fprintf(stderr, "No valid interface identifiers given!\n");
				exit(1);
			};
			retval = librfc3041_calc(&ipv6addr, &ipv6addr2, &ipv6addr3, &ipv6addr4);
			ipv6addr_copy(&ipv6addr, &ipv6addr3);
			ipv6addr_copy(&ipv6addr2, &ipv6addr4);
			break;

		default:
			/* no action */
			break;
	};

	if (retval != 0) {
		fprintf(stderr, "Problem occurs during action\n");
		exit (1);
	};

	if (ipv6calc_debug) {
		fprintf(stderr, "%s: End of action\n", DEBUG_function_name);
	};

OUTPUT_type: /* temporary solutions */

	/***** output type *****/
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Start of output handling\n", DEBUG_function_name);
	};
	
	/* catch showinfo */	
	if (command == CMD_showinfo) {
		if (ipv6addr.flag_valid == 1) {
			retval = showinfo_ipv6addr(&ipv6addr, formatoptions);
	       	} else if (ipv4addr.flag_valid == 1) {
		       	fprintf(stderr, "Showinfo of IPv4 address currently not implemented!\n");
			retval = 1;
	       	} else if (macaddr.flag_valid == 1) {
		       	fprintf(stderr, "Showinfo of MAC address currently not implemented!\n");
			retval = 1;
		} else {
		       	fprintf(stderr, "No valid IPv6 address given!\n");
			retval = 1;
		};
		if (retval != 0) {
			fprintf(stderr, "Problem occurs during selection of showinfo\n");
			exit (1);
		};
		goto RESULT_print;
	} else if (command == CMD_showinfotypes) {
		showinfo_availabletypes();
			exit (1);
	};


	switch (outputtype) {
		case -1:
			/* old implementation */
			break;

		case FORMAT_base85:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			retval = ipv6addrstruct_to_base85(&ipv6addr, resultstring);
			break;
				
		case FORMAT_bitstring:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Call 'librfc2874_addr_to_bitstring'\n", DEBUG_function_name);
			};
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, formatoptions);
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			switch (outputtype) {
				case FORMAT_revnibbles_int:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.int.");
					break;
				case FORMAT_revnibbles_arpa:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.arpa.");
					break;
			};
			break;
			
		case FORMAT_ifinet6:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			retval = libifinet6_ipv6addrstruct_to_ifinet6(&ipv6addr, resultstring);
			break;

		case FORMAT_ipv6addr:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			if (formatoptions & FORMATOPTION_printuncompressed) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else if (formatoptions & FORMATOPTION_printfulluncompressed) {
				retval = libipv6addr_ipv6addrstruct_to_fulluncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;
			
		case FORMAT_eui64:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(1); };
			formatoptions |= FORMATOPTION_printsuffix;
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_iid_token:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) { fprintf(stderr, "No valid IPv6 addresses given!\n"); exit(1); };
			/* get interface identifier */
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring2, formatoptions | FORMATOPTION_printsuffix);
			if (retval != 0) { break; };
			
			/* get token */
			retval = libipv6addr_ipv6addrstruct_to_tokenlsb64(&ipv6addr2, resultstring3, formatoptions);
			
			/* cat together */
			sprintf(resultstring, "%s %s", resultstring2, resultstring3);
			break;


		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			exit(1);
			break;
	};

	if (ipv6calc_debug) {
		fprintf(stderr, "%s: End of output handling\n", DEBUG_function_name);
	};

RESULT_print:
	/* print result */
	if (retval == 0) {
		fprintf(stdout, "%s\n", resultstring);
	} else {
		fprintf(stderr, "%s\n", resultstring);
	};
	exit(retval);
};
#undef DEBUG_function_name
