#!/usr/bin/perl -w
#
# Project    : ipv6calc
# File       : create_ieee_oui_headerfile.pl
# Version    : $Id: create_ieee_oui_headerfile.pl,v 1.10 2011/10/08 11:50:13 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Creates a header file out of IEEE/oui.txt

use strict;
use File::stat;
use POSIX qw(strftime);

my $INFILE;

my $OUTFILE = "dbieee_oui.h";

$INFILE = shift;

if (! defined $INFILE) { $INFILE = "oui.txt" };

print "Create dbieee_oui.h automatically\n";

open(IN, "<$INFILE") || die "Cannot open infile: $INFILE";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq|/*
 * Project       : ipv6calc
 * File          : dbieee_oui.h
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
print OUT "static const char* libieee_oui_status = \"OUI/" . strftime("%Y%m%d", localtime($sb->mtime)) . "\";\n";

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

		# remove ',' '.', '"'
		$oui =~ s/[,\.;"]/ /ig;

		# remove leading and trailing spaces
		$oui =~ s/^\s+//ig;
		$oui =~ s/\s+$//ig;

		# convert spaces to '-'
		$oui =~ s/\s+/-/ig;

		# Some final cleanup
		$oui =~ s/-INT-L//ig;
		$oui =~ s/-B-V//ig;

		# remove trailling '-'
		$oui =~ s/-+$//ig;

		# escape "
		$t3 =~ s/"/\\"/g;
	
		print OUT "\t{ 0x" . $a . $b . $c . ", \"$t3\", \"$oui\" },\n";
	};
};

print OUT qq|
};
|;

print "Finished\n";
