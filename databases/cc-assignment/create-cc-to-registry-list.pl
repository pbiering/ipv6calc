#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/cc-assignment
# File       : create-cc-to-registry-list.pl
# Version    : $Id: create-cc-to-registry-list.pl,v 1.2 2013/10/15 06:21:41 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates CountryCode -> Registry map (as header file)


use IPC::Open2;
#use strict;

my $debug_hinttable = 0;
my $debug = 0;


my $OUTFILE = "db_cc_reg_assignment.h";

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time - 48*60*60);

$year = 1900 + $year;
$mon = sprintf "%02d", $mon + 1;
$mday = sprintf "%02d", $mday;

my @files = (
	"../registries/apnic/delegated-apnic-latest",
	"../registries/lacnic/delegated-lacnic-latest",
	"../registries/afrinic/delegated-afrinic-latest",
	"../registries/ripencc/delegated-ripencc-latest",
	"../registries/arin/delegated-arin-extended-latest",
);

my %assignments;
my %assignments_overall;

my %date_created;

foreach my $file (@files) {
	print "Proceed file: " . $file . "\n";

	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	my $ipv4;
	my $length;
	my $flag_proceeded;
	my $flag_found_date = 0;
	while (<FILE>) {
		$line = $_;
		chomp $line;

		# catch date line
		if ($line =~ /^2(\.[0-9])?\|([^\|]+)\|.*\|([0-9]{8})\|[^\|]*$/o) {
			$date_created{uc($2)} = $3;
			print "Found create date: " . $3 . "\n";
			next;
		};

		# skip not proper lines
		if ( ! ( $line =~ /\|(ipv4|ipv6)\|/o ) ) { next; };
		if ( $line =~ /\|\*\|/o ) { next; };

		#print $line . "\n";

		my ($reg, $tld, $token, $ipv4, $numbers, $date, $status, $other) = split /\|/, $line;

		if ($token !~ /^(ipv4|ipv6)$/o ) { next; };

		$tld = uc($tld);

		$reg = uc($reg);
		$reg =~ s/RIPENCC/RIPE/o;

		if ($reg !~ /^(ARIN|APNIC|RIPE|IANA|LACNIC|AFRINIC)$/o) {
			print "Unsupported registry: " . $reg . "\n";
			next;
		};

		# increment counter
		$assignments{$tld}->{$reg}++;
	};

	close(FILE);

	if ($flag_found_date != 1) {
		die("no date line found, unsupported file format");
	};
};


## Biggest wins
foreach my $tld (sort keys %assignments) {
	my $reg;

	# find biggest one
	my $entries = scalar(keys $assignments{$tld});

	if ($entries == 1) {
		$reg = (keys $assignments{$tld})[0];
	} else {
		# check for biggest one
		my $max = 0;

		for $reg_entry (keys $assignments{$tld}) {
			if ($assignments{$tld}->{$reg_entry} > $max) {
				$max = $assignments{$tld}->{$reg_entry};
				$reg = $reg_entry;
			};
		};
	};

	$assignments_overall{$tld} = $reg;
};


# Create header file


print "Create outfile now: " . $OUTFILE . "\n";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq| /*
 * Project       : ipv6calc
 * File          : db_cc_reg_assignment.h
|;
print OUT " * Version       : \$I";
print OUT "d:\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: RIPE NCC, APNIC, ARIN, LACNIC, AFRINIC
 *
 * Information:
 *  Additional header file for databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 */

|;

# print creation dates
my $string = "";
for my $reg (sort keys %date_created) {
	if (length($string) > 0) {
		$string .= " ";
	};
	$string .= $reg . "/" . $date_created{$reg};
};
print OUT "static const char *db_cc_registry_status = \"$string\";\n";

# Main data structure
print OUT qq|
static const s_cc_reg_assignment cc_reg_assignment[] = {
|;

foreach my $tld (sort keys %assignments_overall) {
	printf OUT "\t{ \"%s\", REGISTRY_%-8s },\n", $tld, $assignments_overall{$tld};
};

print OUT "};\n";

# Indexed data structure
print OUT qq|
static const s_cc_index_reg_assignment cc_index_reg_assignment[] = {
|;

for (my $i = 0; $i < 1024; $i++) {
	# convert index into cc
	my $c1 = ($i % 26) + ord('A');
	my $c2 = int($i / 26);
	if ($c2 > 9) {
		$c2 = $c2 + ord('A') - 10;
	} else {
		$c2 = $c2 + ord('0');
	};

	my $cc = chr($c1) . chr($c2);

	my $reg;
	if (defined $assignments_overall{$cc}) {
		$reg = $assignments_overall{$cc};
	} else {
		$reg = "UNKNOWN";
	};

	my $cc_print;
	if ($i < 26*36) {
		$cc_print = $cc;
	} else {
		$cc_print = "reserved-special";
	};
		
	printf OUT "\t{ REGISTRY_%-8s }, // %4d  %s\n", $reg, $i, $cc_print;
};

print OUT "};";

print "Finished\n";
