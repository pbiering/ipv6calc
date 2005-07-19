#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/ipv6-assignment
# File       : create-registry-list.pl
# Version    : $Id: create-registry-list.pl,v 1.1 2005/07/19 15:48:33 peter Exp $
# Copyright  : 2005 by Simon Arlott
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates IPv6 address assignement header
#  Uses code from ipv4-assignment

#use strict;

my $OUTFILE = "dbipv6addr_assignment.h";

my $global_file = "ipv6-unicast-address-assignments";

my %assignments;

# Generate subnet powers
my %subnet_powers;

print STDERR "Generate subnet powers\n";
for (my $i = 32; $i >= 1; $i--) {
	my $power = 1 << (32 - $i);

	my $dec;
	if ($i == 32) {
		$dec = 0xffffffff;
	} else {
		$dec = ((2 << $i) - 1) << (32 - $i);
	};

	#print STDERR "Power " . $i . ":". $power;
	#printf STDERR "   subnet mask: %8x\n", $dec;
	$subnet_powers{$i}->{'numbers'} = $power;
	$subnet_powers{$i}->{'mask'} = $dec;
};

# Create header file

print "Create outfile now: " . $OUTFILE . "\n";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq| /*
 * Project       : ipv6calc
 * File          : dbipv6_assignment.h
|;
print OUT " * Version       : \$I";
print OUT "d:\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: IANA
 *
 * Information:
 *  Additional header file for libipv6addr.c
 */

|;

# Main data structure
print OUT qq|
static const s_ipv6addr_assignment dbipv6addr_assignment[] = {
|;


print "Proceed file: " . $global_file . "\n";

open(FILE, "<$global_file") || die "Cannot open file: $global_file";

my $line;
my $block; my $length;
while (<FILE>) {
	$line = $_;
	chomp $line;
	my $reg;

	if ( $line =~ /^[0-9A-F]{4}:/ ) {
		($block, $reg, my $dummy) = split /\s+/, $line;

		($block, $length) = split /\//, $block;
	} else {
		# skip not proper lines
		next;
	};

	$reg = uc($reg);
	$reg =~ s/RIPE/RIPENCC/g;

	if ( ($reg ne "ARIN") && ($reg ne "APNIC") && ($reg ne "RIPENCC") && ($reg ne "IANA")
		&& ($reg ne "LACNIC") && ($reg ne "6BONE") && ($reg ne "6TO4") ) {
		#print "Unsupported registry: " . $reg . "\n";
		next;
	};

	#printf "%s/%d=%s", $block, $length, $reg . "\n";
	$block =~ s/://g;
	printf OUT "\t{ 0x%su, 0x%8xu, \"%s\" },\n", $block, $subnet_powers{$length}->{'mask'}, $reg;
};
close(FILE);

print OUT qq|};
|;

close(OUT);

print "Finished\n";

