#!/usr/bin/perl -w

use strict;

#
# Project    : ipv6calc
# File       : create_ieee_oui_headerfile.pl
# Version    : $Id: create_ieee_oui_headerfile.pl,v 1.1 2002/03/11 21:52:25 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Creates a header file out of IEEE/oui.txt

my $INFILE = "samplecode/ieee/oui.txt";

my $OUTFILE = "libieeeoui.h";

open(IN, "<$INFILE") || die "Cannot open infile: $INFILE";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
print OUT qq|
/*
 * Project    : ipv6calc
 * File       : libieeeoui.h
 * Version    : Generated
 *
 * Information:
 *  Additional header file for libieee.c
 */

|;

# Structure
print OUT qq|
/* Structure */
#include "libieee.h"

static const s_ieee_oui libieee_oui[] {
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
	
		print OUT "\t{ 0x" . $a . $b . $c . ", \"$t3\" },\n";
	};
};

print OUT qq|
};
|;
