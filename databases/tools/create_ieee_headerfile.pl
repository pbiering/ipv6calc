#!/usr/bin/perl -w
#
# Project    : ipv6calc
# File       : create_ieee_headerfile.pl
# Version    : $Id: create_ieee_headerfile.pl,v 1.3 2014/03/31 19:48:34 ds6peter Exp $
# Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Creates a header file out of IEEE files
#
# Virtual Machine prefixes from:
# http://www.techrepublic.com/blog/networking/mac-address-scorecard-for-common-virtual-machine-platforms/538

use strict;
use File::stat;
use POSIX qw(strftime);
use Getopt::Std;


my $INFILE;
my $OUTFILE;
my $TYPE;

my %opts;
getopts ("di:o:t:", \%opts);

if (! defined $opts{'i'}) {
	print "ERROR : missing input file (option -i)\n";
	exit 1
};
$INFILE = $opts{'i'};

if (! defined $opts{'o'}) {
	print "ERROR : missing output file (option -o)\n";
	exit 1
}
$OUTFILE = $opts{'o'};

if (! defined $opts{'t'}) {
	print "ERROR : missing type (option -t)\n";
	exit 1
};

$TYPE = $opts{'t'};

my $debug = $opts{'d'};

# set options according to type
if ($TYPE eq "oui") {

} elsif ($TYPE eq "oui36") {

} elsif ($TYPE eq "iab") {

} else {
	print "ERROR : unsupported type: " . $TYPE . "\n";
	exit 1;
};
my $flag_qemu = 0;

print "Create file " . $OUTFILE . " from " . $INFILE . " of type " . $TYPE . "\n";

open(IN, "<$INFILE") || die "Cannot open infile: $INFILE";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq|/*
 * Project       : ipv6calc
 * File          : $OUTFILE
|;
print OUT " * Version       : \$Id";
print OUT ":\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: IEEE
 *
 * Information:
 *  Additional header file for libipv6calc_db_wrapper_BuiltIn.c
 */

|;

# print creation date
my $sb = stat($INFILE);
print OUT "\/\*\@unused\@\*\/ static const char* libieee_" . $TYPE . "_status __attribute__ ((__unused__)) = \"" . uc($TYPE) . "/" . strftime("%Y%m%d", localtime($sb->mtime)) . "\";\n";

# Structure
print OUT qq|

static const s_ieee_$TYPE libieee_${TYPE}[] = {
|;


# Data
my %major_list;
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

	print "DEBUG : parse line: " . $line . "\n" if (defined $debug);

	if ($line =~ /\(hex\)/ ) {
		print "DEBUG : found major entry line: " . $line . "\n" if (defined $debug);

		if ($state != 0) {
			die "Major problem during parsing (out of state)";
		};

		$i++;
		print STDERR $i . "\r";


		# kill spaces
		$line =~ s/[ \t]+/ /g;
                # kill leading spaces
		$line =~ s/^ *//g;
		# kill trailing spaces
		$line =~ s/ *$//g;

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

		# translate umlauts
		$oui =~ s/Ä/AE/g;
		$oui =~ s/Ö/OE/g;
		$oui =~ s/Ü/UE/g;
		$oui =~ s/ä/AE/g;
		$oui =~ s/ö/OE/g;
		$oui =~ s/ü/UE/g;
		$oui =~ s/ß/SS/g;
	
		#print $oui . "\n";

		$oui_major = "0x" . $a . $b . $c;
		$oui_owner = $t3;
		$oui_owner_short = $oui;

		if (! defined $major_list{$oui_major}) {
			$major_list{$oui_major} = 1;
		};

		$state = 1;
		print "DEBUG : found entry: " . $oui_major . "\n" if (defined $debug);
	};

	if ($line =~ /\(base 16\)/) {
		print "DEBUG : found minor entry line: " . $line . "\n" if (defined $debug);

		if ($state != 1) {
			die "Major problem during parsing (out of state)";
		};

		$i++;
		print STDERR $i . "\r";

		# kill spaces
		$line =~ s/[ \t]+/ /g;
                # kill leading spaces
		$line =~ s/^ *//g;
		# kill trailing spaces
		$line =~ s/ *$//g;

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

print "List of major OUIs\n";
for my $key (sort keys %major_list) {
	print $key . "\n";
};
print "Finished\n";
