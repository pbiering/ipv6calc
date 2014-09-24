#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/ipv6-assignment
# File       : create-registry-list.pl
# Version    : $Id: create-registry-list.pl,v 1.13 2014/09/24 09:07:57 ds6peter Exp $
# Copyright  : 2005 by Simon Arlott (initial implementation of global file only)
#              2005-2014 by Peter Bieringer <pb (at) bieringer.de> (further extensions)
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates IPv6 address assignement header
#  Uses code from ipv4-assignment

use strict;

use Net::IP;
use Math::BigInt;
use XML::Simple;

use BerkeleyDB;
use POSIX qw(strftime);

my $debug = 0;

# Debugging
# $debug |= 0x0010; #  0x0010: proceed registry files
#  0x0020: proceed global file
#  0x0040: fill data
#  0x0100: subnet mask generation

# $debug |= 0x0200; # parse lines
$debug |= 0x0400; # store

my $OUTFILE = "dbipv6addr_assignment.h";

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time - 48*60*60);

$year = 1900 + $year;
$mon = sprintf "%02d", $mon + 1;
$mday = sprintf "%02d", $mday;

my $global_file = "../registries/iana/ipv6-unicast-address-assignments.xml";

my @files = (
	"../registries/arin/delegated-arin-extended-latest",
	"../registries/ripencc/delegated-ripencc-latest",
	"../registries/apnic/delegated-apnic-latest",
	"../registries/lacnic/delegated-lacnic-latest",
	"../registries/afrinic/delegated-afrinic-latest"
);

my (@arin, @apnic, @ripencc, @iana, @lacnic, @afrinic, @reserved, @s6to4, @s6bone);

my %date_created;


# Generate subnet powers
my %subnet_masks;

print "Generate subnet masks\n";
for (my $i = 0; $i <= 64; $i++) {
	my $mask;
	$mask = Math::BigInt->new("0xffffffffffffffff");

	$mask->blsft(64 - $i);
	#$mask->bnot();

	my $ip_ipv6_int = $mask->copy();
	$ip_ipv6_int->brsft(32);
	$ip_ipv6_int->band("0xffffffff");
	$subnet_masks{$i}->{'mask_00_31'} = sprintf("%08x", $ip_ipv6_int);

	$ip_ipv6_int = $mask->copy();
	$ip_ipv6_int->band("0xffffffff");
	$subnet_masks{$i}->{'mask_32_63'} = sprintf("%08x", $ip_ipv6_int);
	printf "Prefix length %3d: ",  $i if ($debug & 0x100);
	printf "   mask_00_31=" . $subnet_masks{$i}->{'mask_00_31'} . " mask_32_63=" . $subnet_masks{$i}->{'mask_32_63'} . "\n" if ($debug & 0x100);
};

# Fill global assignement (IPv6 should be more hierarchical than IPv4)
sub proceed_global() {
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
		#print $$e2{'prefix'} . ":" . $$e2{'description'} . "\n";

		my $ipv6 = $$e2{'prefix'};
		my $reg = $$e2{'description'};

		# Check for > /32
		my ($addr, $length) = split /\//, $ipv6;
		if ($length > 32) {
			die "Currently unsupported prefix length (>32): $ipv6";
		};

		print "reg=" . $reg . " addr=$ipv6\n" if ($debug & 0x20);

		$reg = uc($reg);
		$reg =~ s/RIPE NCC/RIPENCC/g;

		#print $$e2{'prefix'} . ":" . $reg . "\n";

		if ($reg eq "ARIN" ) {
			push @arin, $ipv6;
		} elsif ($reg eq "APNIC" ) {
			push @apnic, $ipv6;
		} elsif ($reg eq "RIPENCC" ) {
			push @ripencc, $ipv6;
		} elsif ($reg eq "IANA" ) {
			push @iana, $ipv6;
		} elsif ($reg eq "LACNIC" ) {
			push @lacnic, $ipv6;
		} elsif ($reg eq "AFRINIC" ) {
			push @afrinic, $ipv6;
		} elsif ($reg eq "RESERVED" ) {
			push @reserved, $ipv6;
		} elsif ($reg eq "6TO4" ) {
			push @s6to4, $ipv6;
		} elsif ($reg eq "6BONE" ) {
			push @s6bone, $ipv6;
		} else {
			die "Unsupported registry: " . $reg . "\n";
		};
	    };
	};
};


## Main
proceed_global();

foreach my $file (@files) {
	print "Proceed file: " . $file . "\n";

	my $counter = 0;

	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	my %cache;
	my $flag_found_date = 0;
	my $version;

	while (<FILE>) {
		$line = $_;
		chomp $line;

		$counter++;

		# catch date line
		if ($line =~ /^2(\.[0-9])?\|([^\|]+)\|.*\|([0-9]{8})\|[^\|]*$/o) {
			$date_created{uc($2)} = $3;
			print "Found create date: " . $3 . "\n";
			$flag_found_date = 1;
			next;
		};

		# skip not proper lines
		if ( ! ( $line =~ /\|ipv6\|/o ) ) { next; };
		if ( $line =~ /\|\*\|/o ) { next; };

		print $line . "\n" if ($debug & 0x0200);

		my ($reg, $tld, $token, $ipv6, $prefixlen, $date, $status, $other) = split /\|/o, $line;

		if ( $token ne "ipv6" ) { next; };

		$reg = uc($reg);
		$reg =~ s/\wRIPE\w/RIPENCC/og;

		# get registry array
		my $parray;

		if ($reg eq "ARIN" ) {
			$parray = \@arin;
		} elsif ($reg eq "APNIC" ) {
			$parray = \@apnic;
		} elsif ($reg eq "RIPENCC" ) {
			$parray = \@ripencc;
		} elsif ($reg eq "IANA" ) {
			$parray = \@iana;
		} elsif ($reg eq "LACNIC" ) {
			$parray = \@lacnic;
		} elsif ($reg eq "AFRINIC" ) {
			$parray = \@afrinic;
		} else {
			die "Unsupported registry: " . $reg;
		};

		print "check: reg=" . $reg . " ipv6=" . $ipv6 . "/" . $prefixlen . "\n" if ($debug & 0x10);

		# Check for already included in range:
		my $ip_ipv6 = new Net::IP($ipv6) || die "Can't create IPv6 object from ipv6=$ipv6";

		my $flag = 0;
		my $ip_ipv6_check;
		my $test;
Label_restart:
		for (my $i = 0; $i < scalar(@$parray); $i++) {
			my $ipv6_check = $$parray[$i];

			print "check: check against ipv6=" . $ipv6_check if ($debug & 0x10);

			if (defined $cache{$ipv6_check}) {
				$ip_ipv6_check = $cache{$ipv6_check};
			} else {
				$ip_ipv6_check = new Net::IP($ipv6_check) || die "Can't create IPv6 object from ipv6=$ipv6_check";
				$cache{$ipv6_check} = $ip_ipv6_check;
			};

			$test = $ip_ipv6->overlaps($ip_ipv6_check);

			if ($test == $Net::IP::IP_IDENTICAL) {
				print " IDENTICAL\n" if ($debug & 0x10);
				$flag = 1;
				last;
			} elsif ($test == $Net::IP::IP_A_IN_B_OVERLAP) {
				print " INCLUDED\n" if ($debug & 0x10);
				$flag = 1;
				last;
			} elsif ($test == $Net::IP::IP_B_IN_A_OVERLAP) {
				# check value is included in ipv6 range
				print " REMOVE\n" if ($debug & 0x10);
				goto("Label_restart");
			} else {
				print " NO MATCH\n" if ($debug & 0x10);
			};
		};

		if ($flag == 1) {
			next;
		};

		# Check for > /64
		if ($prefixlen > 64) {
			die "Currently unsupported prefix length (>64): $ipv6/$prefixlen";
		};

		$ip_ipv6 = new Net::IP($ipv6);
		print "store: reg=" . $reg . " ipv6=" . $ip_ipv6->ip() . "/" . $prefixlen . "\n" if ($debug & 0x0400);

		# Push into array
		push @$parray, $ip_ipv6->ip() . "/" . $prefixlen;
	};

	close(FILE);

	if ($flag_found_date != 1) {
		die("no date line found, unsupported file format");
	};
};



# Create hash
my %data;

sub fill_data($$) {
	my $parray = shift || die "missing array pointer";
	my $reg = shift || die "missing registry";

	print "Fill data for registry: $reg\n";

	foreach my $entry (sort @$parray) {
		my ($ipv6, $length) = split /\//o, $entry;

		my $ip_ipv6 = new Net::IP($entry);

		print " ipv6=$entry" if ($debug & 0x40);

		my $ip_ipv6_int = $ip_ipv6->intip();
		my $ip_ipv6_int_32_63 = $ip_ipv6_int->copy();

		# MSB 00-31 
		$ip_ipv6_int->brsft(96);
		my $ipv6_hex_00_31 = sprintf("%08x", $ip_ipv6_int);

		# MSB 32-63
		$ip_ipv6_int_32_63->brsft(64);
		$ip_ipv6_int_32_63->band("0xffffffff");
		my $ipv6_hex_32_63 = sprintf("%08x", $ip_ipv6_int_32_63);

		$data{$ipv6}->{'ipv6_00_31'} = $ipv6_hex_00_31;
		$data{$ipv6}->{'ipv6_32_63'} = $ipv6_hex_32_63;

		my $mask_00_31 = $subnet_masks{$ip_ipv6->prefixlen()}->{'mask_00_31'};
		my $mask_32_63 = $subnet_masks{$ip_ipv6->prefixlen()}->{'mask_32_63'};

		$data{$ipv6}->{'mask_00_31'} = $mask_00_31;
		$data{$ipv6}->{'mask_32_63'} = $mask_32_63;
		$data{$ipv6}->{'mask_length'} = $length;
		$data{$ipv6}->{'reg'} = $reg;
		print " ipv6_hex_00_31=$ipv6_hex_00_31 ipv6_hex_32_63=$ipv6_hex_32_63 mask_00_31=$mask_00_31 mask_00_31=$mask_32_63 hex reg=$reg\n" if ($debug & 0x40);
	};
};

&fill_data(\@apnic  , "APNIC");
&fill_data(\@ripencc, "RIPENCC");
&fill_data(\@arin   , "ARIN");
&fill_data(\@lacnic , "LACNIC");
&fill_data(\@afrinic, "AFRINIC");
&fill_data(\@iana   , "IANA");
&fill_data(\@s6to4  , "6TO4");
&fill_data(\@s6bone , "6BONE");

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
 * Data copyright: IANA ARIN RIPENCC APNIC LACNIC AFRINIC
 *
 * Information:
 *  Additional header file for databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 */

#include "databases/lib/libipv6calc_db_wrapper_BuiltIn.h"

|;

# print creation dates
my $string = "";
for my $reg (sort keys %date_created) {
	if (length($string) > 0) {
		$string .= " ";
	};
	$string .= $reg . "/" . $date_created{$reg};
};
print OUT "\/\*\@unused\@\*\/ static const char* dbipv6addr_registry_status __attribute__ ((__unused__)) = \"$string\";\n";

print OUT "\/\*\@unused\@\*\/ static const time_t dbipv6addr_registry_unixtime __attribute__ ((__unused__)) = " . time . ";\n";

# Main data structure
print OUT qq|
static const s_ipv6addr_assignment dbipv6addr_assignment[] = {
|;


foreach my $ipv6 (sort keys %data) {
	printf OUT "\t{ 0x%s, 0x%s, 0x%s, 0x%s, %3d, REGISTRY_%-10s },\n", $data{$ipv6}->{'ipv6_00_31'}, $data{$ipv6}->{'ipv6_32_63'}, $data{$ipv6}->{'mask_00_31'}, $data{$ipv6}->{'mask_32_63'}, $data{$ipv6}->{'mask_length'}, $data{$ipv6}->{'reg'};
};

print OUT qq|};
|;

close(OUT);

print "Finished\n";


print "Start creation of DB file\n";

# external database
my $type = "2026"; # BuiltIn Registry->IPv4
my $date = $string;
$now_string = strftime "%Y%m%d-%H%M%S%z", gmtime;
my $info = "dbusage=ipv6calc;dbformat=1;dbdate=$date;dbtype=" . $type . ";dbproto=6;dbcreated=$now_string";
my $filename = "ipv6calc-external-registry-ipv6.db";

if (-f $filename) {
        unlink($filename) || die "Can't delete old file: $filename";
};

print "INFO  : create db from input: IPv6=$filename\n";

my %h_info;

tie %h_info, 'BerkeleyDB::Hash', -Filename => $filename, -Subname => 'info', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename: $! $BerkeleyDB::Error\n";

$h_info{'dbusage'} = "ipv6calc";
$h_info{'dbformat'} = "1"; # ';' separated values
$h_info{'dbdate'} = $date;
$h_info{'dbtype'} = $type;
$h_info{'dbcreated'} = $now_string;
$h_info{'dbcreated_unixtime'} = time + 1;


untie %h_info;

my @a;

tie @a, 'BerkeleyDB::Recno', -Filename => $filename, -Subname => 'data', -Flags => DB_CREATE || die "Cannot open file $filename: $! $BerkeleyDB::Error\n";

foreach my $ipv6 (sort keys %data) {
	push @a, sprintf("%s;%s;%s;%s;%d;REGISTRY_%s", $data{$ipv6}->{'ipv6_00_31'}, $data{$ipv6}->{'ipv6_32_63'}, $data{$ipv6}->{'mask_00_31'}, $data{$ipv6}->{'mask_32_63'}, $data{$ipv6}->{'mask_length'}, $data{$ipv6}->{'reg'});
};

untie @a;

print "INFO  : db created from input: IPv6=$filename\n";

