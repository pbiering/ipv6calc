#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : create-registry-list.pl
# Version    : $Id$
# Copyright  : 2002-2017 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates IPv4 address assignement header
# Requires:
#  XML::Simple

use strict;
use warnings;

use IPC::Open2;
use XML::Simple;

use Getopt::Std;
use BerkeleyDB;
use POSIX qw(strftime);

my $progname = $0;

my $debug = 0;

#$debug |= 0x01;
#$debug |= 0x02;
#$debug |= 0x04; # assignments_iana
#$debug |= 0x08; # assignments_iana
#$debug |= 0x10; # assignments gap closing
#$debug |= 0x100; # assignments LISP
#$debug |= 0x1000; # TLD store

sub help {
	print qq#
Usage: $progname [-S <SRC-DIR>] [-D <DST-DIR>] -H|B [-A] [-d <debuglevel>]
	-S <SRC-DIR>	source directory
	-D <DST-DIR>	destination directory
	-H		create header file(s)
	-B		create Berkeley DB file(s)
	-A		atomic operation (generate .new and move on success)
	-d <debuglevel> debug level
	-h		this online help

#;
	exit 0;
};

# parse options
our ($opt_h, $opt_S, $opt_D, $opt_B, $opt_H, $opt_A, $opt_d);
getopts('d:hS:D:BHA') || help();

if (defined $opt_h) {
	help();
};

if (defined $opt_d) {
	$debug = $opt_d;
};

unless (defined $opt_H || defined $opt_B) {
	print "WARN  : nothing to do (missing -H|-B)\n";
	exit 1;
};


## locations
my $file_dst_h;
my $dir_src;
my $dir_dst;
my $global_file;
my $lisp_file;
my @files;

## source file handling
if (defined $opt_S) {
	$dir_src = $opt_S;
	$dir_src .= "/" if ($dir_src !~ /\/$/o);

	$global_file = $dir_src . "ipv4-address-space.xml";
	$lisp_file = $dir_src . "site-db";

	@files = (
		$dir_src . "delegated-arin-extended-latest",
		$dir_src . "delegated-ripencc-latest",
		$dir_src . "delegated-apnic-latest",
		$dir_src . "delegated-lacnic-latest",
		$dir_src . "delegated-afrinic-latest"
	);
} else {
	# default
	$dir_src = "../registries/";

	$global_file = $dir_src . "iana/ipv4-address-space.xml";
	$lisp_file = $dir_src . "lisp/site-db";

	@files = (
		$dir_src . "arin/delegated-arin-extended-latest",
		$dir_src . "ripencc/delegated-ripencc-latest",
		$dir_src . "apnic/delegated-apnic-latest",
		$dir_src . "lacnic/delegated-lacnic-latest",
		$dir_src . "afrinic/delegated-afrinic-latest"
	);
};

## destination file handling
if (defined $opt_D) {
	$dir_dst = $opt_D;

} else {
	# default
	$dir_dst = ".";
};

$file_dst_h = $dir_dst . "/dbipv4addr_assignment.h";
my $file_dst_db_reg = $dir_dst . "/ipv6calc-external-ipv4-registry.db";
my $file_dst_db_cc  = $dir_dst . "/ipv6calc-external-ipv4-countrycode.db";

my $file_dst_db_reg_orig;
my $file_dst_db_cc_orig;

print "INFO  : destination file for header: " . $file_dst_h  . "\n" if (defined $opt_H);
print "INFO  : destination file for DB (Registry): " . $file_dst_db_reg . "\n" if (defined $opt_B);
print "INFO  : destination file for DB (CountryCode): " . $file_dst_db_cc  . "\n" if (defined $opt_B);

my %assignments;
my %assignments_iana;

my %ip_countrycode;

my %date_created;


# convert a dotted IPv4 address into 32-bit decimal
sub ipv4_to_dec {
	my $ipv4 = shift || die "Missing IPv4 address";

	my ($t1, $t2, $t3, $t4) = split /\./, $ipv4;

	my $dec = $t4 + 256 * ( $t3 + 256 * ( $t2 + 256 * $t1 ));

	return ($dec);
};

# convert a 32-bit decimal to dotted IPv4 address
sub dec_to_ipv4 {
	my $dec = shift;

 	my $t1 = ($dec & 0xff000000) >> 24;
 	my $t2 = ($dec & 0x00ff0000) >> 16;
 	my $t3 = ($dec & 0x0000ff00) >> 8;
 	my $t4 = ($dec & 0x000000ff);

	my $ipv4 = sprintf "%d.%d.%d.%d", $t1, $t2, $t3, $t4;

	return ($ipv4);
};


# find start of ipv4 in %assignments_iana
sub find_start_iana {
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


## global IANA assignment, last resort fallback
sub proceed_global {
	my $ipv4; my $length;
	my ($start, $distance);

	# Proceed first global IANA file
	print "INFO  : proceed file (XML): " . $global_file . "\n";

	my $xs = XML::Simple->new();
	my $xd = $xs->XMLin($global_file) || die "Cannot open/parse file: $global_file";

	for my $e1 ($xd->{'updated'}) {
		$e1 =~ s/-//go;
		$date_created{'IANA'} = $e1;
		print "DEBUG : found create date: " . $e1 . "\n";
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
					print "Missing whois entry for: " . &dec_to_ipv4($start) . " (fallback to ARIN)\n"; 
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

## LISP
sub proceed_lisp {
	# 1: map-server
	# 2: site - site name
	# 3: instance
	# 4: eid - eid prefix
	# 5: registered - yes or no
	# 6: who - ip addr of who last registered, blank & -- mean none
	# 7: proxy - yes or no (or blank)
	# 8: ttl - in what ever format the map-server reports it
	# 9: nrloc - how many rlocs in field 10
	# 10: rlocs - seperated by ';' and with (up|down)
	# 11: timestamp - when data was collected, in UTC 
	#
	# Example:
	# cisco-sjc-mr-ms-1|akennedy-xtr|0|153.16.9.64/28|no||no||0||2017.0223.0600
	# cisco-sjc-mr-ms-1|akennedy-xtr|0|2610:d0:1233::/48|no||no||0||2017.0223.0600

	my $ipv4; my $length;
	my ($start, $distance);

	print "INFO  : proceed LISP data file (TXT): " . $lisp_file . "\n";

	open(my $FILE, "<", $lisp_file) || die "Cannot open file: $lisp_file";

	my ($map, $site, $instance, $eid, $registered, $who, $proxy, $ttl, $nrloc, $rloc, $timestamp);
	my $reg = "LISP";
	my $line;
	my $line_number = 0;

	while (<$FILE>) {
		$line = $_;
		$line_number++;
		chomp $line;

		printf "LISP read   : $line\n" if ($debug & 0x100);

		($map, $site, $instance, $eid, $registered, $who, $proxy, $ttl, $nrloc, $rloc, $timestamp) = split /\|/, $line;

		if ($line_number == 1) {
			$timestamp =~ /^([0-9]{4})\.([0-9]{4})/o;
			$date_created{$reg} = $1 . $2;
		};

		if ($eid !~ /^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})\/([0-9]{1,2})$/o) {
			# not IPv4
			next;
		};

		printf "LISP process: $line\n" if ($debug & 0x100);

		printf "LISP convert: IPv4=$1 mask=$2\n" if ($debug & 0x100);

		# convert IPv4 address to decimal
		my $start_reg = &ipv4_to_dec($1);
		my $distance_reg = 2 ** (32 - $2);

		printf "LISP check  : IPv4=$1 mask=$2\n" if ($debug & 0x100);
		if (
			(($start_reg & 0xffff0000) != 0x99100000)
		) {
			# not in assigned LISP space
			printf "LISP skip  : IPv4=$start_reg mask=$distance_reg\n" if ($debug & 0x100);
			next;
		};


		printf "LISP store  : reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x100);
		$assignments{$start_reg}->{'distance'} = $distance_reg;
		$assignments{$start_reg}->{'registry'} = $reg;
		$assignments{$start_reg}->{'info'} = $site;
	};

	close($FILE);
};

&proceed_global();

## proceed files of each registry
foreach my $file (@files) {
	print "INFO  : proceed file: " . $file . "\n";


	open(my $FILE, "<", $file) || die "Cannot open file: $file";

	my $line;
	my $flag_proceeded;
	my $flag_found_date = 0;

	my $start_reg = -1;
	my $start_tld = -1;
	my $distance_reg;
	my $distance_tld;
	my $cc_store = undef;

	my ($reg, $cc, $token, $ipv4, $numbers, $date, $status, $other);

	while (<$FILE>) {
		$line = $_;
		chomp $line;

		# catch date line
		if ($line =~ /^2(\.[0-9])?\|([^\|]+)\|.*\|([0-9]{8})\|[^\|]*$/o) {
			$date_created{uc($2)} = $3;
			print "DEBUG : found create date: " . $3 . "\n";
			$flag_found_date = 1;
			next;
		};

		# skip not proper lines
		if ( ! ( $line =~ /\|ipv4\|/o ) ) { next; };
		if ( $line =~ /\|\*\|/o ) { next; };

		#print $line . "\n";

		($reg, $cc, $token, $ipv4, $numbers, $date, $status, $other) = split /\|/, $line;

		if ( $token ne "ipv4" ) { next; };

		$reg = uc($reg);
		$reg =~ s/\wRIPE\w/RIPENCC/g;

		$cc = uc($cc);

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

		if ($start_reg == -1) {
			# set start & distance
			$start_reg = $ipv4_dec;
			$distance_reg = $numbers;
			printf "init  : reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x01);

		} elsif (($start_reg + $distance_reg) == $ipv4_dec) {
			# extend distance
			$distance_reg += $numbers;
			printf "extend: reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x01);

		} else {
			printf "store : reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x02);
			$assignments{$start_reg}->{'distance'} = $distance_reg;
			$assignments{$start_reg}->{'registry'} = $reg;

			# set new start
			$start_reg = $ipv4_dec;
			$distance_reg = $numbers;
			printf "init  : reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x01);
		};

		## IP -> TLD (CountryCode)
		if ($cc !~ /^[A-Z][A-Z]$/o) {
			# no valid TLD
			if ($start_tld != -1) {
				$ip_countrycode{$start_tld}->{'distance'} = $distance_tld;
				$ip_countrycode{$start_tld}->{'cc'} = $cc_store;
				printf "store : tld=%-10s start=%08x distance=%08x\n", $cc_store, $start_tld, $distance_tld if ($debug & 0x1000);
			};
			$cc_store = undef;
			$start_tld = -1;
			next;
		};

		printf "data  : tld=%-10s ipv4 =%08x numbers=%08x\n", $cc, $ipv4_dec, $numbers if ($debug & 0x01);

		if ($start_tld == -1) {
			# set start & distance
			$start_tld = $ipv4_dec;
			$distance_tld = $numbers;
			$cc_store = $cc;
			printf "init  : tld=%-10s start=%08x distance=%08x\n", $cc, $start_tld, $distance_tld if ($debug & 0x01);
		} elsif ((($start_tld + $distance_tld) == $ipv4_dec) && ($cc_store eq $cc)) {
			# extend distance
			$distance_tld += $numbers;
			printf "extend: tld=%-10s start=%08x distance=%08x\n", $cc, $start_tld, $distance_tld if ($debug & 0x01);
		} else {
			$ip_countrycode{$start_tld}->{'distance'} = $distance_tld;
			$ip_countrycode{$start_tld}->{'cc'} = $cc_store;
			printf "store : tld=%-10s start=%08x distance=%08x\n", $cc, $start_tld, $distance_tld if ($debug & 0x1000);

			# set new start
			$start_tld = $ipv4_dec;
			$distance_tld = $numbers;
			$cc_store = $cc;
			printf "init  : tld=%-10s start=%08x distance=%08x\n", $cc, $start_tld, $distance_tld if ($debug & 0x01);
		};
	};

	close($FILE);


	if ($start_reg != -1) {
		# store last found entry
		$assignments{$start_reg}->{'distance'} = $distance_reg;
		$assignments{$start_reg}->{'registry'} = $reg;
		printf "store : reg=%-10s start=%08x distance=%08x\n", $reg, $start_reg, $distance_reg if ($debug & 0x02);
	};

	if ($start_tld != -1) {
		# store last found entry
		$ip_countrycode{$start_tld}->{'distance'} = $distance_tld;
		$ip_countrycode{$start_tld}->{'cc'} = $cc;
		printf "store : tld=%-10s start=%08x distance=%08x\n", $cc, $start_tld, $distance_tld if ($debug & 0x02);
	};

	if ($flag_found_date != 1) {
		die("no date line found, unsupported file format");
	};
};

&proceed_lisp();

my $now_string = strftime "%Y%m%d-%H%M%S%z %Z", localtime;
my $string = "";
for my $reg (sort keys %date_created) {
	if (length($string) > 0) {
		$string .= " ";
	};
	$string .= $reg . "/" . $date_created{$reg};
};


## Create header file IP->Registry
if (defined $opt_H) {
	print "INFO  : create outfile now: " . $file_dst_h . "\n";
	open(my $OUT, ">", $file_dst_h) || die "Cannot open outfile: $file_dst_h";

	# Header
	print $OUT qq|/*
 * Project       : ipv6calc
 * File          : dbipv4_assignment.h
|;
	print $OUT " * Version       : \$I";
	print $OUT "d:\$\n";
	print $OUT qq| * Generated     : $now_string
 * Data copyright: RIPE NCC, APNIC, ARIN, LACNIC, AFRINIC, LISP
 *
 * Information:
 *  Additional header file for databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 */

#include "databases/lib/libipv6calc_db_wrapper_BuiltIn.h"

|;

	# print creation dates
	print $OUT "static const char* dbipv4addr_registry_status __attribute__ ((__unused__)) = \"$string\";\n";

	print $OUT "static const time_t dbipv4addr_registry_unixtime __attribute__ ((__unused__)) = " . time . ";\n";

	# Main data structure
	print $OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment[] = {
|;

	printf $OUT "\t//first     , last      , registry  \n";

	foreach my $ipv4 (sort { $a <=> $b } keys %assignments) {
		my $distance = $assignments{$ipv4}->{'distance'};
		my $registry = $assignments{$ipv4}->{'registry'};

		printf $OUT "\t{ 0x%08x, 0x%08x, REGISTRY_%-10s }, // %-15s - %s\n", $ipv4, ($ipv4 + $distance - 1), $registry, &dec_to_ipv4($ipv4), &dec_to_ipv4($ipv4 + $distance - 1);
	};

	print $OUT qq|};
	|;


	## IANA assignment
	print $OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment_iana[] = {
|;

	printf $OUT "\t//first     , last      , registry  \n";

	foreach my $ipv4 (sort { $a <=> $b } keys %assignments_iana) {
		my $distance = $assignments_iana{$ipv4}->{'distance'};
		my $registry = $assignments_iana{$ipv4}->{'registry'};

		printf $OUT "\t{ 0x%08x, 0x%08x, REGISTRY_%-10s }, // %-15s - %s\n", $ipv4, ($ipv4 + $distance - 1), $registry, &dec_to_ipv4($ipv4), &dec_to_ipv4($ipv4 + $distance - 1);
	};

	print $OUT "};\n";


	## INFO table
	print $OUT qq|
static const s_ipv4addr_info dbipv4addr_info[] = {
|;

	printf $OUT "\t//first     , last      , registry  \n";

	foreach my $ipv4 (sort { $a <=> $b } keys %assignments) {
		next if (! defined $assignments{$ipv4}->{'info'});

		my $distance = $assignments{$ipv4}->{'distance'};
		my $info = $assignments{$ipv4}->{'info'};

		printf $OUT "\t{ 0x%08x, 0x%08x, %-30s }, // %-15s - %s\n", $ipv4, ($ipv4 + $distance - 1), "\"" . $info . "\"", &dec_to_ipv4($ipv4), &dec_to_ipv4($ipv4 + $distance - 1);
	};

	print $OUT "};\n";


	## END
	print "INFO  : finished creation of header file: " . $file_dst_h . "\n";
};

## Create DB file
if (defined $opt_B) {
	## IPv4->Registry
	print "INFO  : start creation of DB file IPv4->Registry: " . $file_dst_db_reg . "\n";

	# external database
	my $type = "2024"; # External IPv4->Registry
	my $date = $string;

	if (defined $opt_A) {
		$file_dst_db_reg_orig = $file_dst_db_reg;
		$file_dst_db_reg .= ".new";
	} else {
		if (-f $file_dst_db_reg) {
			unlink($file_dst_db_reg) || die "Can't delete old file: $file_dst_db_reg";
		};
	};

	print "INFO  : create db from input: IPv4=$file_dst_db_reg\n";

	my %h_ipv4_info;

	tie %h_ipv4_info, 'BerkeleyDB::Btree', -Filename => $file_dst_db_reg, -Subname => 'info', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $file_dst_db_reg: $! $BerkeleyDB::Error\n";

	$h_ipv4_info{'dbusage'} = "ipv6calc";
	$h_ipv4_info{'dbformat'} = "1"; # ';' separated values
	$h_ipv4_info{'dbdate'} = $date;
	$h_ipv4_info{'dbtype'} = $type;
	$h_ipv4_info{'dbcreated'} = $now_string;
	$h_ipv4_info{'dbcreated_unixtime'} = time + 1;

	untie %h_ipv4_info;

	my @a_ipv4;

	tie @a_ipv4, 'BerkeleyDB::Recno', -Filename => $file_dst_db_reg, -Subname => 'data', -Flags => DB_CREATE || die "Cannot open file $file_dst_db_reg: $! $BerkeleyDB::Error\n";

	foreach my $ipv4 (sort { $a <=> $b } keys %assignments) {
		my $distance = $assignments{$ipv4}->{'distance'};
		my $registry = $assignments{$ipv4}->{'registry'};

		push @a_ipv4, sprintf("%08x;%08x;%s",$ipv4, ($ipv4 + $distance - 1), "REGISTRY_" . $registry);
	};

	untie @a_ipv4;

	my @a_ipv4_iana;

	tie @a_ipv4_iana, 'BerkeleyDB::Recno', -Filename => $file_dst_db_reg, -Subname => 'data-iana', -Flags => DB_CREATE || die "Cannot open file $file_dst_db_reg: $! $BerkeleyDB::Error\n";

	foreach my $ipv4 (sort { $a <=> $b } keys %assignments_iana) {
		my $distance = $assignments_iana{$ipv4}->{'distance'};
		my $registry = $assignments_iana{$ipv4}->{'registry'};

		push @a_ipv4_iana, sprintf("%08x;%08x;%s", $ipv4, ($ipv4 + $distance - 1), "REGISTRY_" . $registry);
	};

	untie @a_ipv4_iana;

	print "INFO  : db created from input IPv4->Registry: $file_dst_db_reg\n";

	if (defined $opt_A) {
		rename $file_dst_db_reg, $file_dst_db_reg_orig;
		if ($? != 0) {
			print "ERROR : can't rename file to: $file_dst_db_reg_orig ($!) - delete: $file_dst_db_reg\n";
			unlink $file_dst_db_reg;
		} else {
			print "INFO  : successful rename file to: $file_dst_db_reg_orig\n";
		};
	};


	#### IPv4->CountryCode
	print "INFO  : start creation of DB file: IPv4->CountryCode: " . $file_dst_db_cc . "\n";

	# external database
	$type = "2034"; # External IPv4->CountryCode
	$date = $string;
	$now_string = strftime "%Y%m%d-%H%M%S%z", gmtime;
	my $info = "dbusage=ipv6calc;dbformat=1;dbdate=$date;dbtype=" . $type . ";dbproto=4;dbcreated=$now_string";

	if (defined $opt_A) {
		$file_dst_db_cc_orig = $file_dst_db_cc;
		$file_dst_db_cc .= ".new";
	} else {
		if (-f $file_dst_db_cc) {
			unlink($file_dst_db_cc) || die "Can't delete old file: $file_dst_db_cc";
		};
	};

	print "INFO  : create db from input: IPv4=$file_dst_db_cc\n";

	my %h_info;

	tie %h_info, 'BerkeleyDB::Btree', -Filename => $file_dst_db_cc, -Subname => 'info', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $file_dst_db_cc: $! $BerkeleyDB::Error\n";

	$h_info{'dbusage'} = "ipv6calc";
	$h_info{'dbformat'} = "1"; # ';' separated values
	$h_info{'dbdate'} = $date;
	$h_info{'dbtype'} = $type;
	$h_info{'dbcreated'} = $now_string;
	$h_info{'dbcreated_unixtime'} = time + 1;


	untie %h_info;

	my @a;

	tie @a, 'BerkeleyDB::Recno', -Filename => $file_dst_db_cc, -Subname => 'data', -Flags => DB_CREATE || die "Cannot open file $file_dst_db_cc: $! $BerkeleyDB::Error\n";

	foreach my $ipv4 (sort { $a <=> $b } keys %ip_countrycode) {
		my $distance = $ip_countrycode{$ipv4}->{'distance'};
		my $cc = $ip_countrycode{$ipv4}->{'cc'};

		push @a, sprintf("%08x;%08x;%s", $ipv4, ($ipv4 + $distance - 1), $cc);
	};

	untie @a;

	print "INFO  : DB file created from input IPv4->CountryCode: " . $file_dst_db_cc . "\n";

	if (defined $opt_A) {
		rename $file_dst_db_cc, $file_dst_db_cc_orig;
		if ($? != 0) {
			print "ERROR : can't rename file to: $file_dst_db_cc_orig ($!) - delete: $file_dst_db_cc\n";
			unlink $file_dst_db_cc;
		} else {
			print "INFO  : successful rename file to: $file_dst_db_cc_orig\n";
		};
	};
};
