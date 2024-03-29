#!/usr/bin/perl -w
#
# Project    : ipv6calc
# File       : create_ieee_headerfile.pl
# Copyright  : 2002-2023 by Peter Bieringer <pb (at) bieringer.de>
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
getopts ("cdi:o:t:l:", \%opts);

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

} elsif ($TYPE eq "oui28") {

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

# normalize
sub normalize($) {
	my $oui = shift;
		$oui = uc($oui);

		$oui =~ s/"//ig;

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

		# translate umlauts
		$oui =~ s/Ä/AE/g;
		$oui =~ s/Ö/OE/g;
		$oui =~ s/Ü/UE/g;
		$oui =~ s/ä/AE/g;
		$oui =~ s/ö/OE/g;
		$oui =~ s/ü/UE/g;
		$oui =~ s/ß/SS/g;

		# remove non-ascii chars
		$oui =~ s/[^[:ascii:]]//g;

		# remove ? (to avoid trigraphs issues)
		$oui =~ s/\?//g;

		# remove \ (to avoid misinterpretion by compiler)
		$oui =~ s/\\//g;
	
		# Some final cleanup
		$oui =~ s/-INT-L$//ig;
		$oui =~ s/-S-L$//ig;
		$oui =~ s/-A-S$//ig;
		$oui =~ s/-LLC$//ig;
		$oui =~ s/-S-P-A$//ig;
		$oui =~ s/-S-R-L$//ig;
		$oui =~ s/-AND$//ig;
		$oui =~ s/-E-V$//ig;
		$oui =~ s/-BHD$//ig;
		$oui =~ s/-SDN$//ig;
		$oui =~ s/-MBH$//ig;

		# remove trailing '+'
		$oui =~ s/\++$//ig;

		# remove trailing '-'
		$oui =~ s/-+$//ig;

		# reduce more than one '-'
		$oui =~ s/-+/-/ig;
	return($oui);
};

# Data
my %major_list;
my $oui_major;
my $oui_owner;
my $oui_owner_short;
my $oui_minor_begin;
my $oui_minor_end;

my $state = 0;

my %map;

my $i = 0;
while (<IN>) {
	my $line = $_;
	chomp $line;

	print "DEBUG : parse line: " . $line . "\n" if (defined $debug);


	if (defined $opts{'c'}) {
		# CSV parser
		if ($line !~ /^MA-(L|M|S),([0-9A-F]+),(.*)$/o && $line !~ /^(IAB),([0-9A-F]+),(.*)$/o) {
			print "DEBUG : skip line: " . $line . "\n" if (defined $debug);
			next;
		};

		my $type = $1;
		my $prefix = $2;
		my $company_raw = $3;

		# extract company
		my $company;
		if ($company_raw =~ /^"([^"]+)",(.*)$/o) {
			$company = $1;
		} else {
			$company_raw =~ /^([^,]+),(.*)/o;
			$company = $1;
		};

		$company =~ s/"//ig;

		$company =~ s/\\/\\\\/ig; # escape \

                # kill leading spaces
		$company =~ s/^ *//g;
		# kill trailing spaces
		$company =~ s/ *$//g;

		print "DEBUG : extract from: " . $company_raw . " -> " . $company . "\n" if (defined $debug);

		$map{$prefix}->{'short'} = normalize($company);
		$map{$prefix}->{'long'} = $company;

		$major_list{substr($prefix, 0, 6)} = 1;

		# Append information for special OUIs
		if ($prefix eq "080027") {
			# 08:00:27
			$map{$prefix}->{'long'} .= " (possible VirtualBox VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		} elsif ($prefix eq "0003FF") {
			# 00:03:FF
			$map{$prefix}->{'long'} .= " (possible Hyper-V, Virtual Server, Virtual PC VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		} elsif ($prefix eq "001C42") {
			# 00:1C:42
			$map{$prefix}->{'long'} .= " (possible Paralles Desktop, Workstation, Server, Virtuozzo VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		} elsif ($prefix eq "000F4B") {
			# 00:0F:4B
			$map{$prefix}->{'long'} .= " (possible Virtual Iron VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		} elsif ($prefix eq "00163E") {
			# 00:16:3E
			$map{$prefix}->{'long'} .= " (possible Xen VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		} elsif ($prefix eq "525400") {
			# 52:54:00
			$map{$prefix}->{'long'} .= " (possible QEMU VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
			$flag_qemu = 1;
		} elsif ( ($prefix eq "005056") ||
			  ($prefix eq "000C29") ||
			  ($prefix eq "000569") ) {
			# 00:50:56
			# 00:0C:29
			# 00:05:69
			$map{$prefix}->{'long'} .= " (possible VMware VM)";
			$map{$prefix}->{'short'} .= "-VIRTUAL";
		};
	} else {
		die("TXT parser no longer supported");
	}; # TXT parser
};

if (scalar(keys %map) > 0) {
	if ($TYPE eq "oui" && ($flag_qemu == 0)) {
		# add missing entry
		$map{"525400"}->{'long'} = "possible QEMU VM";
		$map{"525400"}->{'short'} = "QEMU-VIRTUAL";
	};

	for my $prefix (sort { $a cmp $b } keys %map) {
		my $oui_major = "0x" . substr($prefix, 0, 6);
		my $oui_minor_begin = "0x" . substr($prefix, 6) . "0" x (6 - length(substr($prefix, 6)));
		my $oui_minor_end   = "0x" . substr($prefix, 6) . "F" x (6 - length(substr($prefix, 6)));
		my $oui_owner = $map{$prefix}->{'long'};
		my $oui_owner_short = $map{$prefix}->{'short'};

		if ($TYPE eq "oui") {
			print OUT "\t{ " . $oui_major . ", \"" . $oui_owner . "\", \"" . $oui_owner_short . "\" },\n";
		} else {
			print OUT "\t{ " . $oui_major . ", " . $oui_minor_begin . ", " . $oui_minor_end . ", \"" . $oui_owner . "\", \"" . $oui_owner_short . "\" },\n";
		};
	};
};

print OUT qq|
};
|;

my @missing;

my %defines = (
	"oui36" => "IEEE_OUI36",
	"oui28" => "IEEE_OUI28",
	"iab"   => "IEEE_IAB",
);

if ($TYPE eq "oui36" || $TYPE eq "iab" || $TYPE eq "oui28") {
	my @lines;
	if (defined $opts{'l'}) {
		# read file
		open my $handle, '<', $opts{'l'} || die("Can't open file: " . $opts{'l'});
		chomp(@lines = <$handle>);
		close $handle;
		print "Lines of " . $opts{'l'} . ": " . $#lines . "\n";
	};

	my $problem = 0;
	print "List of major OUIs\n" if ($TYPE eq "oui36" || $TYPE eq "iab");
	for my $key (sort keys %major_list) {
		print $key . "\n" if ($TYPE eq "oui36" || $TYPE eq "iab");
		if (defined $opts{'l'}) {
			if (grep /0x$key/, @lines) {
				# ok
			} else {
				print "Missing OUI ($TYPE) 0x$key in " . $opts{'l'} . " (-> append there in array 'ieee_mapping' and retry)\n";
				$problem = 1;
				push @missing, "0x" . $key;
			};
		};
	};
	if ($problem == 1) {
		print "Check NOT successfully of major OUI ($TYPE) entries in " . $opts{'l'} . ": " . scalar(%major_list) . "\n";
		if (scalar (@missing) > 0) {
			# retrieve latest
			my $last;
			my $lc = 0;
			my $lc_last = 0;
			foreach my $line (@lines) {
				$lc++;
				if ($line =~ /\s+$defines{$TYPE},\s+(0x[0-9a-f]{4})/) {
					$last = hex($1);
					$lc_last = $lc
				};
			};

			#print "last=$last\n";
			print "Add following to " . $opts{'l'} . " after line " . $lc_last . "\n";

			print "\t// " . uc($TYPE) . " (" . strftime("%Y-%m-%d", localtime) . ")\n";

			foreach my $entry (@missing) {
				$last++;
				printf "\t{ " . $entry . ",\t" . $defines{$TYPE} . ", 0x%04x },\n", $last;
			};
		};
		exit 1;
	};
	print "Check successfully major OUI entries in " . $opts{'l'} . ": " . scalar(%major_list) . "\n";
};

if (scalar(keys %major_list) == 0) {
	print "Finished with problems (list empty)\n";
	exit 1;
};
print "Finished successfully, entries: " . scalar(keys %map) . "\n";
