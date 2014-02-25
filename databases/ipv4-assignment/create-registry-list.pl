#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : create-registry-list.pl
# Version    : $Id: create-registry-list.pl,v 1.33 2014/02/25 20:49:16 ds6peter Exp $
# Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates IPv4 address assignement header
# Requires:
#  XML::Simple


use IPC::Open2;
use XML::Simple;
use strict;
use warnings;

my $debug = 0;

#$debug |= 0x01;
$debug |= 0x02;
#$debug |= 0x04; # assignments_iana
#$debug |= 0x08; # assignments_iana
$debug |= 0x10; # assignments gap closing


my $OUTFILE = "dbipv4addr_assignment.h";

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time - 48*60*60);

$year = 1900 + $year;
$mon = sprintf "%02d", $mon + 1;
$mday = sprintf "%02d", $mday;

my @files = (
	"../registries/arin/delegated-arin-extended-latest",
	"../registries/ripencc/delegated-ripencc-latest",
	"../registries/apnic/delegated-apnic-latest",
	"../registries/lacnic/delegated-lacnic-latest",
	"../registries/afrinic/delegated-afrinic-latest"
);

my $global_file = "../registries/iana/ipv4-address-space.xml";

my %assignments;
my %assignments_iana;

my %date_created;


# convert a dotted IPv4 address into 32-bit decimal
sub ipv4_to_dec($) {
	my $ipv4 = shift || die "Missing IPv4 address";

	my ($t1, $t2, $t3, $t4) = split /\./, $ipv4;

	my $dec = $t4 + 256 * ( $t3 + 256 * ( $t2 + 256 * $t1 ));

	return ($dec);
};

# convert a 32-bit decimal to dotted IPv4 address
sub dec_to_ipv4($) {
	my $dec = shift;

 	my $t1 = ($dec & 0xff000000) >> 24;
 	my $t2 = ($dec & 0x00ff0000) >> 16;
 	my $t3 = ($dec & 0x0000ff00) >> 8;
 	my $t4 = ($dec & 0x000000ff);

	my $ipv4 = sprintf "%d.%d.%d.%d", $t1, $t2, $t3, $t4;

	return ($ipv4);
};


# find start of ipv4 in %assignments_iana
sub find_start_iana($) {
	my $ipv4 = shift || die "Missing IPv4 address";

	for my $key (keys %assignments_iana) {
		if ($ipv4 < $key) {
			next;
		} elsif ($ipv4 >= ($key + $assignments_iana{$key}->{'distance'})) {
			next;
		} else {
			# match
			return ($key);
		};
	};

	return (undef);
};



# Should't be used, a little bit obsolete
sub proceed_global() {
	my $ipv4; my $length;
	my ($start, $distance);

	# Proceed first global IANA file
	print "Proceed file (XML): " . $global_file . "\n";

	my $xs = XML::Simple->new();
	my $xd = $xs->XMLin($global_file) || die "Cannot open/parse file: $global_file";

	for my $e1 ($xd->{'updated'}) {
		$e1 =~ s/-//go;
		$date_created{'IANA'} = $e1;
		print "Found create date: " . $e1 . "\n";
		last;
	};

	for my $e1 ($xd->{'record'}) {
	    for my $e2 (@$e1) {
		print $$e2{'prefix'} . ":" . $$e2{'designation'} . ":" . $$e2{'status'} . "\n" if ($debug);

		my ($block, $prefixlength) = split /\//, $$e2{'prefix'};

		if ($block =~ /^[0-9]+$/) {
			$start = &ipv4_to_dec(int($block) . ".0.0.0");
		} else {
			die "Currently unsupported block, fix code: " . $block;
		};

		$distance   = 2**(32 - int($prefixlength));
		printf "data  : start=%08x distance=%08x from prefx=%s\n", $start, $distance, $$e2{'prefix'} if ($debug & 0x04);

		my $reg;

		if (uc($$e2{'status'}) eq "RESERVED") {
			$reg = "IANA";
		} else {			
			$reg = uc($$e2{'designation'});
			$reg =~ s/RIPE NCC/RIPENCC/g;
			$reg =~ s/(IANA) .*/$1/g;
			$reg =~ s/.* (RIPENCC)/$1/g;

			if ( ($reg ne "ARIN") && ($reg ne "APNIC") && ($reg ne "RIPENCC") && ($reg ne "IANA") && ($reg ne "LACNIC") && ($reg ne "AFRINIC")) {
				# fallback to whois entry
				if (defined $$e2{'whois'}) {
					$$e2{'whois'} =~ /^whois\.([a-z]+)\..*/;
					$reg = uc($1);

					if (! defined $reg) {
						die "Can't extract registry from whois entry: " . $$e2{'whois'}; 
					};

					if ($reg eq "RIPE") {
						$reg = "RIPENCC";
					};

					if ( ($reg ne "ARIN") && ($reg ne "APNIC") && ($reg ne "RIPENCC") && ($reg ne "IANA") && ($reg ne "LACNIC") && ($reg ne "AFRINIC")) {
						die "Unsupported registry extracted from whois entry: " . $reg;
					};
				} else {
					print "Missing whois entry for: " . $start . " (fallback to ARIN)\n"; 
					$reg = "ARIN";
				};
			};
		};

		printf "store : reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x08);
		$assignments_iana{$start}->{'registry'} = $reg;
		$assignments_iana{$start}->{'distance'} = $distance;
	    };
	};
};

&proceed_global();

## proceed files of each registry
foreach my $file (@files) {
	print "Proceed file: " . $file . "\n";


	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	my $flag_proceeded;
	my $flag_found_date = 0;

	my $start = -1;
	my $distance;

	my ($reg, $tld, $token, $ipv4, $numbers, $date, $status, $other);

	while (<FILE>) {
		$line = $_;
		chomp $line;

		# catch date line
		if ($line =~ /^2(\.[0-9])?\|([^\|]+)\|.*\|([0-9]{8})\|[^\|]*$/o) {
			$date_created{uc($2)} = $3;
			print "Found create date: " . $3 . "\n";
			$flag_found_date = 1;
			next;
		};

		# skip not proper lines
		if ( ! ( $line =~ /\|ipv4\|/o ) ) { next; };
		if ( $line =~ /\|\*\|/o ) { next; };

		#print $line . "\n";

		($reg, $tld, $token, $ipv4, $numbers, $date, $status, $other) = split /\|/, $line;

		if ( $token ne "ipv4" ) { next; };

		$reg = uc($reg);
		$reg =~ s/\wRIPE\w/RIPENCC/g;

		if ( $reg ne "ARIN" && $reg ne "APNIC" && $reg ne "RIPENCC" && $reg ne "IANA" && $reg ne "LACNIC" && $reg ne "AFRINIC") {
			print "Unsupported registry: " . $reg . "\n";
			next;
		};

		if ($reg eq "ARIN" ) {
		} elsif ($reg eq "APNIC" ) {
		} elsif ($reg eq "RIPENCC" ) {
		} elsif ($reg eq "IANA" ) {
		} elsif ($reg eq "LACNIC" ) {
		} elsif ($reg eq "AFRINIC" ) {
		} else {
			die "Unsupported registry: " . $reg;
		};

		# convert IPv4 address to decimal
		my $ipv4_dec = &ipv4_to_dec($ipv4);

		printf "data  : reg=%-10s ipv4 =%08x numbers=%08x\n", $reg, $ipv4_dec, $numbers if ($debug & 0x01);

		if ($start == -1) {
			# set start & distance
			printf "init  : reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x01);
			$start = $ipv4_dec;
			$distance = $numbers;

		} elsif (($start + $distance) == $ipv4_dec) {
			# extend distance
			printf "extend: reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x01);
			$distance += $numbers;

		} else {
			printf "store : reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x02);
			$assignments{$start}->{'distance'} = $distance;
			$assignments{$start}->{'registry'} = $reg;

			# set new start
			printf "init  : reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x01);
			$start = $ipv4_dec;
			$distance = $numbers;
		};
	};

	close(FILE);

	# store last found entry
	printf "store : reg=%-10s start=%08x distance=%08x\n", $reg, $start, $distance if ($debug & 0x02);
	$assignments{$start}->{'distance'} = $distance;
	$assignments{$start}->{'registry'} = $reg;

	if ($flag_found_date != 1) {
		die("no date line found, unsupported file format");
	};
};

## check whether gaps can be closed with IANA assignment
#			my $start_iana = &find_start_iana($start);
#			my $ipv4_dec_iana = &find_start_iana($ipv4_dec);
#
#			if (defined $start_iana && defined $ipv4_dec_iana) {
#				if ($start_iana == $ipv4_dec_iana) {
#					# same IANA assignment
#					$distance = $ipv4_dec + $numbers - $start;
#
#					printf "extend: reg=%-10s start=%08x distance=%08x (IANA based gap closing)\n", $reg, $start, $distance if ($debug & 0x10);
#					next;
#				};
#			};


## Create header file


print "Create outfile now: " . $OUTFILE . "\n";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq| /*
 * Project       : ipv6calc
 * File          : dbipv4_assignment.h
|;
print OUT " * Version       : \$I";
print OUT "d:\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: RIPE NCC, APNIC, ARIN, LACNIC, AFRINIC
 *
 * Information:
 *  Additional header file for databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 */

#include "libipv6calc.h"

|;

# print creation dates
my $string = "";
for my $reg (sort keys %date_created) {
	if (length($string) > 0) {
		$string .= " ";
	};
	$string .= $reg . "/" . $date_created{$reg};
};
print OUT "\/\*\@unused\@\*\/ static const char* dbipv4addr_registry_status __attribute__ ((__unused__)) = \"$string\";\n";

# Main data structure
print OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment[] = {
|;

printf OUT "\t//first     , last      , registry  \n";

foreach my $ipv4 (sort { $a <=> $b } keys %assignments) {
	my $distance = $assignments{$ipv4}->{'distance'};
	my $registry = $assignments{$ipv4}->{'registry'};

	printf OUT "\t{ 0x%08x, 0x%08x, REGISTRY_%-10s }, // %-15s - %-15s\n", $ipv4, ($ipv4 + $distance - 1), $registry, &dec_to_ipv4($ipv4), &dec_to_ipv4($ipv4 + $distance - 1);
};

print OUT qq|};
|;

# IANA assignment
print OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment_iana[] = {
|;

printf OUT "\t//first     , last      , registry  \n";

foreach my $ipv4 (sort { $a <=> $b } keys %assignments_iana) {
	my $distance = $assignments_iana{$ipv4}->{'distance'};
	my $registry = $assignments_iana{$ipv4}->{'registry'};

	printf OUT "\t{ 0x%08x, 0x%08x, REGISTRY_%-10s }, // %-15s - %-15s\n", $ipv4, ($ipv4 + $distance - 1), $registry, &dec_to_ipv4($ipv4), &dec_to_ipv4($ipv4 + $distance - 1);
};

print OUT qq|};
|;

print "Finished\n";
