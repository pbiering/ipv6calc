#!/usr/bin/perl -w
#
# Project    : ipv6calc/lib
# File       : create_ieee_oui_headerfile.pl
# Version    : $Id: create_ieee_oui_headerfile.pl,v 1.1 2002/03/24 16:53:23 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Creates a header file out of IEEE/oui.txt

use strict;

my $INFILE = "oui.txt";

my $OUTFILE = "dbieee_oui.h";

print "Create dbieee_oui.h automatically\n";

open(IN, "<$INFILE") || die "Cannot open infile: $INFILE";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq|
/*
 * Project    : ipv6calc
 * File       : libieeeoui.h
 * Version    : Generated $now_string
 *
 * Information:
 *  Additional header file for libieee.c
 */

|;

# Structure
print OUT qq|

static const s_ieee_oui libieee_oui[] = {
|;


# Data
my $i = 0;
while (<IN>) {
	my $line = $_;
	chomp $line;

	if ($line =~ /\(hex\)/ ) {

		$i++;
		print STDERR $i . "\r";


		# kill spaces
		$line =~ s/[ \t]+/ /g;
		#print $line . "\n";
		my ($t1, $t2, $t3) = split / /, $line, 3;
	
		my ($a, $b, $c) = split /-/, $t1;

		# shorten OUI string
		my $oui = uc($t3);

		# replace '(' ')' '&'
		$oui =~ s/[\(\)\&\']/ /ig;

		# remove unimportand information
		$oui =~ s/\bINC[\.]*\b//ig;
		$oui =~ s/\bLTD[\.]*\b//ig;
		$oui =~ s/\bLIMITED\b//ig;
		$oui =~ s/\bCO[\.]*\b//ig;
		$oui =~ s/\bCORP[\.]*\b//ig;
		$oui =~ s/\bCOMP[\.]\b//ig;
		$oui =~ s/\bGMBH\b//ig;
		$oui =~ s/\bCORPORATION\b//ig;
		$oui =~ s/\bS[\.]*A[\.]*\b//ig;
		$oui =~ s/\bAG\b/ELECTRONIC/ig;
		$oui =~ s/\bKG\b//ig;
		$oui =~ s/\bBV\b//ig;
		$oui =~ s/\bINT'L\b/INTERNATIONAL/ig;
		$oui =~ s/\bCOMMUNICATIONS\b/COMMUNICATION/ig;
		$oui =~ s/\bCORPOTATION\b/CORPORATION/ig;
		$oui =~ s/\bINTERNAIONAL\b/INTERNATIONAL/ig;

		# remove some unneeded text
		$oui =~ s/\bINTERNATIONAL\b//ig;
		$oui =~ s/\bTECHNOLOGY\b//ig;
		$oui =~ s/\bCOMPUTER\b//ig;
		$oui =~ s/\bSYSTEMS\b//ig;
		$oui =~ s/\bENTERPRISE\b//ig;
		$oui =~ s/\bCORPORATION\b//ig;
		$oui =~ s/\bELECTRONIC\b//ig;

		# remove ',' '.'
		$oui =~ s/[,\.;]/ /ig;

		# remove leading and trailing spaces
		$oui =~ s/^\s+//ig;
		$oui =~ s/\s+$//ig;

		# convert spaces to '-'
		$oui =~ s/\s+/-/ig;
	
		#print $oui . "\n";
	
		print OUT "\t{ 0x" . $a . $b . $c . ", \"$t3\", \"$oui\" },\n";
	};
};

print OUT qq|
};
|;

print "Finished\n";
