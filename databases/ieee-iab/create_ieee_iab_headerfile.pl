#!/usr/bin/perl -w
#
# Project    : ipv6calc
# File       : create_ieee_iab_headerfile.pl
# Version    : $Id: create_ieee_iab_headerfile.pl,v 1.4 2011/10/08 11:50:13 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Modified copy of create_ieee_oui_headerfile.pl
#
# Creates a header file out of IEEE/iab.txt

use strict;
use File::stat;
use POSIX qw(strftime);

my $INFILE;

my $OUTFILE = "dbieee_iab.h";

$INFILE = shift;

if (! defined $INFILE) { $INFILE = "iab.txt" };

print "Create dbieee_iab.h automatically\n";

open(IN, "<$INFILE") || die "Cannot open infile: $INFILE";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq|/*
 * Project       : ipv6calc
 * File          : dbieee_iab.h
|;
print OUT " * Version       : \$Id";
print OUT ":\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: IEEE
 *
 * Information:
 *  Additional header file for libieee.c
 */

|;

# print creation date
my $sb = stat($INFILE);
print OUT "static const char* libieee_iab_status = \"IAB/" . strftime("%Y%m%d", localtime($sb->mtime)) . "\";\n";

# Structure
print OUT qq|

static const s_ieee_iab libieee_iab[] = {
|;


# Data
my $oui_major;
my $oui_owner;
my $oui_owner_short;
my $oui_minor_begin;
my $oui_minor_end;

my $state = 0;

my $i = 0;
while (<IN>) {
	my $line = $_;
	chomp $line;

	if ($line =~ /\(hex\)/ ) {
		#print $line . "\n";

		if ($state != 0) {
			die "Major problem during parsing (out of state)";
		};

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
		$oui =~ s/[\(\)\&\',]/ /ig;

		# remove unimportant information
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

		# Replace some text
		$oui =~ s/\b3 Com\b/3COM/ig;
		$oui =~ s/\b3Com Europe\b/3COM/ig;
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
		$oui =~ s/\bHF1-06\b//ig;

		# remove ',' '.'
		$oui =~ s/[,\.;]/ /ig;

		# remove leading and trailing spaces
		$oui =~ s/^\s+//ig;
		$oui =~ s/\s+$//ig;

		# convert spaces to '-'
		$oui =~ s/\s+/-/ig;

		# remove '"'
		$oui =~ s/"//ig;
		$t3 =~ s/"//ig;

		# Some final cleanup
		$oui =~ s/-INT-L//ig;
		$oui =~ s/-B-V//ig;

		# remove trailling '-'
		$oui =~ s/-+$//ig;
	
		#print $oui . "\n";

		$oui_major = "0x" . $a . $b . $c;
		$oui_owner = $t3;
		$oui_owner_short = $oui;

		$state = 1;
	};

	if ($line =~ /\(base 16\)/) {
		#print $line . "\n";

		if ($state != 1) {
			die "Major problem during parsing (out of state)";
		};

		$i++;
		print STDERR $i . "\r";

		# kill spaces
		$line =~ s/[ \t]+/ /g;
		#print $line . "\n";
		$line =~ /^([0-9A-Fa-f]+)-([0-9A-Fa-f]+) /;

		if (! defined $1 || ! defined $2) {
			die "Major problem during parsing (no begin or end)";
		};
		$oui_minor_begin = "0x" . $1;
		$oui_minor_end = "0x" . $2;

		$state = 2;
	};

	if ($state == 2) {
		print OUT "\t{ " . $oui_major . ", " . $oui_minor_begin . ", " . $oui_minor_end . ", \"" . $oui_owner . "\", \"" . $oui_owner_short . "\" },\n";
		$state = 0;
	};
};

print OUT qq|
};
|;

print "Finished\n";
