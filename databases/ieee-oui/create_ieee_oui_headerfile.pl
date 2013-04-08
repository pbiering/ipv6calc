#!/usr/bin/perl -w
#
# Project    : ipv6calc
# File       : create_ieee_oui_headerfile.pl
# Version    : $Id: create_ieee_oui_headerfile.pl,v 1.13 2013/04/08 19:34:56 ds6peter Exp $
# Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Creates a header file out of IEEE/oui.txt
#
# Virtual Machine prefixes from:
# http://www.techrepublic.com/blog/networking/mac-address-scorecard-for-common-virtual-machine-platforms/538

use strict;
use File::stat;
use POSIX qw(strftime);

my $INFILE;

my $OUTFILE = "dbieee_oui.h";

my $flag_qemu = 0;

my $debug = 0;

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
print OUT "\/\*\@unused\@\*\/ static const char* libieee_oui_status __attribute__ ((__unused__)) = \"OUI/" . strftime("%Y%m%d", localtime($sb->mtime)) . "\";\n";

# Structure
print OUT qq|

static const s_ieee_oui libieee_oui[] = {
|;


my ($a, $b, $c);
my ($t1, $t2, $t3);
my $oui;

# Data
my $i = 0;
my $m = 0;
while (<IN>) {
	my $line = $_;
	chomp $line;

	$i++;

	if ($line =~ /\(hex\)/ ) {
		# kill additional spaces
		$line =~ s/[ \t]+/ /g;
		# kill leading spaces
		$line =~ s/^ *//g;
		# kill trailing spaces
		$line =~ s/ *$//g;

		$m++;
		print STDERR "$m\r" if ($debug == 0);

		print STDERR "$i: $line\n" if ($debug != 0);

		($t1, $t2, $t3) = split / /, $line, 3;

		print STDERR $i . ": t1=$t1 t2=$t2 t3=$t3\n" if ($debug != 0);
	
		($a, $b, $c) = split /-/, $t1;

		print STDERR $i . ": a=$a b=$b c=$c\n" if ($debug != 0);

		# shorten OUI string
		$oui = uc($t3);

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

		# Append information for special OUIs
		if ($a eq "08" && $b eq "00" && $c eq "27") {
			# 08:00:27
			$t3 .= " (possible VirtualBox VM)";
			$oui .= "-VIRTUAL";
		} elsif ($a eq "00" && $b eq "03" && $c eq "FF") {
			# 00:03:FF
			$t3 .= " (possible Hyper-V, Virtual Server, Virtual PC VM)";
			$oui .= "-VIRTUAL";
		} elsif ($a eq "00" && $b eq "1C" && $c eq "42") {
			# 00:1C:42
			$t3 .= " (possible Paralles Desktop, Workstation, Server, Virtuozzo VM)";
			$oui .= "-VIRTUAL";
		} elsif ($a eq "00" && $b eq "0F" && $c eq "4B") {
			# 00:0F:4B
			$t3 .= " (possible Virtual Iron VM)";
			$oui .= "-VIRTUAL";
		} elsif ($a eq "00" && $b eq "16" && $c eq "3E") {
			# 00:16:3E
			$t3 .= " (possible Xen VM)";
			$oui .= "-VIRTUAL";
		} elsif ($a eq "52" && $b eq "54" && $c eq "00") {
			# 52:54:00
			$t3 .= " (possible QEMU VM)";
			$oui .= "-VIRTUAL";
			$flag_qemu = 1;
		} elsif ( ($a eq "00" && $b eq "50" && $c eq "56") ||
			  ($a eq "00" && $b eq "0C" && $c eq "29") ||
			  ($a eq "00" && $b eq "05" && $c eq "69") ) {
			# 00:50:56
			# 00:0C:29
			# 00:05:69
			$t3 .= " (possible VMware VM)";
			$oui .= "-VIRTUAL";
		};

		# escape "
		$t3 =~ s/"/\\"/g;
	
		print OUT "\t{ 0x" . $a . $b . $c . ", \"$t3\", \"$oui\" },\n";
	};
};


if ($flag_qemu == 0) {
	# print missing qemu
	$a = "52"; $b = "54"; $c = "00"; $t3 = "possible QEMU VM"; $oui = "QEMU-VIRTUAL";
	print OUT "\t{ 0x" . $a . $b . $c . ", \"$t3\", \"$oui\" },\n";
};

print OUT qq|
};
|;

print "Finished\n";
