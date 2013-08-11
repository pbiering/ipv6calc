#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/as-assignment
# File       : create-registry-list.pl
# Version    : $Id: create-asn-registry-list.pl,v 1.1 2013/08/11 16:42:10 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates AS Number assignement header

my $debug = 0;

my $OUTFILE = "dbasn_assignment.h";

my $file_asn = "../registries/iana/as-numbers.txt";

my %assignments;
my $entry = 0;

my %date_created;

sub proceed_file($$) {
	$file = $_[0];
	$type = $_[1];

	print "Proceed file: " . $file . " with type " . $type . "\n";

	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	while (<FILE>) {
		$line = $_;
		chomp $line;

		if ($line =~ /^\s*([0-9]{4}-[0-9]{2}-[0-9]{2})\s*$/) {
			$date_created{'IANA'} = $1;
			$date_created{'IANA'} =~ s/-//go;
			print "Found create date: " . $date_created{'IANA'} . "\n";
		};

		my ($Number, $Description, $Whois, $Reference, $RegistrationDate);

		if ($type eq "csv") {
			($Number, $Description, $Whois, $Reference, $RegistrationDate) = split /,/, $line;

			# skip not proper lines
			next if (! defined $Number);
			next if (! defined $Description);
			next if ($Description !~ /^Assigned by (\S+)$/o);
			$reg = $1;

		} elsif ($type eq "txt") {
			next if ($line !~ /^\s*([0-9-]+)\s*Assigned by (\S+)/o);
			$Number = $1;
			$reg = $2;
		} else {
			die "unsupported type: " . $type;
		};

		#print $line . "\n";

		$reg = uc($reg);
		$reg =~ s/RIPENCC/RIPE/;

		if ( $reg ne "ARIN" && $reg ne "APNIC" && $reg ne "RIPE" && $reg ne "IANA" && $reg ne "LACNIC" && $reg ne "AFRINIC") {
			print "Unsupported registry: " . $reg . "\n";
			next;
		};

		my ($start, $stop) = split /-/, $Number;

		if ((! defined $stop) || ($stop eq "")) {
			$stop = $start;
		};

		$assignments{$entry}->{'start'} = $start;
		$assignments{$entry}->{'stop'} = $stop;
		$assignments{$entry}->{'reg'} = $reg;
		$entry++;
	};

	close(FILE);
};

proceed_file($file_asn  , "txt");

# Create header file

print "Create outfile now: " . $OUTFILE . "\n";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq| /*
 * Project       : ipv6calc
 * File          : dbasn_assignment.h
|;
print OUT " * Version       : \$I";
print OUT "d:\$\n";
print OUT qq| * Generated     : $now_string
 * Data copyright: IANA
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
print OUT "\/\*\@unused\@\*\/ static const char* dbasn_registry_status __attribute__ ((__unused__)) = \"$string\";\n";

my %data_hint;

# Main data structure
print OUT qq|
static const s_asn_assignment dbasn_assignment[] = {
|;

for ($i = 0; $i < $entry; $i++) {
	printf OUT "\t{ %10d, %10d, REGISTRY_%-10s },\n", $assignments{$i}->{'start'}, $assignments{$i}->{'stop'}, $assignments{$i}->{'reg'};
};

print OUT qq|
};
|;


print "Finished\n";
