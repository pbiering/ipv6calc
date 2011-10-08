#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : create-registry-list.pl
# Version    : $Id: create-registry-list.pl,v 1.26 2011/10/08 11:50:13 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which creates IPv4 address assignement header
# Requires:
#  /usr/bin/aggregate
#  XML::Simple


use IPC::Open2;
use XML::Simple;
#use strict;

if (! -x "/usr/bin/aggregate") {
	print STDERR "Missing or cannot execute binary '/usr/bin/aggregate'\n";
	print STDERR " You can get it from here: http://freshmeat.net/projects/aggregate\n";

	exit 1;
};

my $debug_hinttable = 0;


my $OUTFILE = "dbipv4addr_assignment.h";

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time - 48*60*60);

$year = 1900 + $year;
$mon = sprintf "%02d", $mon + 1;
$mday = sprintf "%02d", $mday;

my @files = (
	"../registries/arin/delegated-arin-latest",
	"../registries/ripencc/delegated-ripencc-latest",
	"../registries/apnic/delegated-apnic-latest",
	"../registries/lacnic/delegated-lacnic-latest",
	"../registries/afrinic/delegated-afrinic-latest"
);
#my @files = ( "lacnic/lacnic." . $year . $mon . "01" );

my (@arin, @apnic, @ripencc, @iana, @lacnic, @afrinic);

my (@arin_agg, @apnic_agg, @ripencc_agg, @iana_agg, @lacnic_agg, @afrinic_agg);

my $global_file = "../registries/iana/ipv4-address-space.xml";

my %assignments;

my $max_prefixlength_not_arin = 0;

my %date_created;


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



sub ipv4_to_dec($) {
	my $ipv4 = shift || die "Missing IPv4 address";

	my ($t1, $t2, $t3, $t4) = split /\./, $ipv4;

	my $dec = $t4 + 256 * ( $t3 + 256 * ( $t2 + 256 * $t1 ));

	return ($dec);
};

sub dec_to_ipv4($) {
	my $dec = shift || die "Missing decimal";

 	my $t1 = ($dec & 0xff000000) >> 24;
 	my $t2 = ($dec & 0x00ff0000) >> 16;
 	my $t3 = ($dec & 0x0000ff00) >> 8;
 	my $t4 = ($dec & 0x000000ff);

	my $ipv4 = sprintf "%d.%d.%d.%d", $t1, $t2, $t3, $t4;

	return ($ipv4);
};

sub length_to_dec($) {
	my $length = shift || die "Missing prefix length";

	my $dec = ((2 << $length) - 1) << (32 - $length);

	return ($dec);
};


sub check_in_list($) {
	my $num = shift || die "Missing IPv4 address number";

	foreach my $ipv4num (keys %assignments ) {
		if ( ( $num & $assignments{$ipv4num}->{'mask'} ) == $ipv4num ) {
			return( $assignments{$ipv4num}->{'registry'} );
		};
	};
	return;
};

# Should't be used, a little bit obsolete
sub proceed_global() {
	my $ipv4; my $length;
	my ($ipv4_start, $ipv4_end);

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
		#print $$e2{'prefix'} . ":" . $$e2{'designation'} . "\n";

		my ($block, $length) = split /\//, $$e2{'prefix'};
		$ipv4_start = int($block);
		$ipv4_end = int($block);

		my $reg = uc($$e2{'designation'});
		$reg =~ s/RIPE NCC/RIPENCC/g;
		$reg =~ s/(IANA) .*/$1/g;
		$reg =~ s/.* (RIPENCC)/$1/g;

		if ( ($reg ne "ARIN") && ($reg ne "APNIC") && ($reg ne "RIPENCC") && ($reg ne "IANA") && ($reg ne "LACNIC") && ($reg ne "AFRINIC")) {
			$reg = "ARIN"; # default now
			# die "Unsupported registry: " . $reg\n";
		};

		#print $$e2{'prefix'} . ":" . $reg . "\n";

		for ($ipv4 = $ipv4_start; $ipv4 <= $ipv4_end; $ipv4++) {
			$ipv4 = $ipv4 . ".0.0.0";
	
			if ($reg eq "ARIN" ) {
				#print "Push ARIN: " . $ipv4 . "/" . $length . "\n";
				push @arin, $ipv4 . "/" . $length;
			} elsif ($reg eq "APNIC" ) {
				#print "Push APNIC: " . $ipv4 . "/" . $length . "\n";
				push @apnic, $ipv4 . "/" . $length;
			} elsif ($reg eq "RIPENCC" ) {
				#print "Push RIPENCC: " . $ipv4 . "/" . $length . "\n";
				push @ripencc, $ipv4 . "/" . $length;
			} elsif ($reg eq "IANA" ) {
				#print "Push IANA: " . $ipv4 . "/" . $length . "\n";
				push @iana, $ipv4 . "/" . $length;
			} elsif ($reg eq "LACNIC" ) {
				#print "Push LACNIC: " . $ipv4 . "/" . $length . "\n";
				push @lacnic, $ipv4 . "/" . $length;
			} elsif ($reg eq "AFRINIC" ) {
				#print "Push AFRINIC: " . $ipv4 . "/" . $length . "\n";
				push @afrinic, $ipv4 . "/" . $length;
			} else {
				die "Unsupported registry";	
			};
		};
	    };
	};
};

&proceed_global();

foreach my $file (@files) {
	print "Proceed file: " . $file . "\n";

	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	my $ipv4;
	my $length;
	my $flag_proceeded;
	while (<FILE>) {
		$line = $_;
		chomp $line;

		# catch date line
		if ($line =~ /^2\|([^\|]+)\|.*\|([0-9]{8})\|[^\|]*$/o) {
			$date_created{uc($1)} = $2;
			print "Found create date: " . $2 . "\n";
			next;
		};

		# skip not proper lines
		if ( ! ( $line =~ /\|ipv4\|/o ) ) { next; };
		if ( $line =~ /\|\*\|/o ) { next; };

		#print $line . "\n";

		my ($reg, $tld, $token, $ipv4, $numbers, $date, $status) = split /\|/, $line;

		if ( $token ne "ipv4" ) { next; };

		$reg = uc($reg);
		$reg =~ s/\wRIPE\w/RIPENCC/g;

		if ( $reg ne "ARIN" && $reg ne "APNIC" && $reg ne "RIPENCC" && $reg ne "IANA" && $reg ne "LACNIC" && $reg ne "AFRINIC") {
			print "Unsupported registry: " . $reg . "\n";
			next;
		};

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


		# convert IPv4 address to decimal
		my $ipv4_dec = &ipv4_to_dec($ipv4);

		my $check_length;
		$flag_proceeded = 0;
		# check numbers maching
		for ($check_length = 1; $check_length <= 32; $check_length++) {
			if ( $subnet_powers{$check_length}->{'numbers'} == $numbers ) {
				# case 1: numbers = 2^x
				
				if ( ( $ipv4_dec & $subnet_powers{$check_length}->{'mask'} ) == $ipv4_dec ) {
					# case 1a: easy, subnet(numbers) matches given network
					push @$parray, $ipv4 . "/" . $check_length;
					#printf "%s/%d=%s (case 1a)\n", $ipv4, $check_length, $reg;
					$flag_proceeded = 1;
					
					last;
				} else {
					#printf "%s/%d=%s (case 1b)\n", $ipv4, $check_length, $reg;
					$check_length++;
					$flag_proceeded = 2;
					last;
				};
			} elsif ( $subnet_powers{$check_length}->{'numbers'} < $numbers ) {
				# case 2: numbers != 2^x
				#printf "%s=%s (case 2: %d)\n", $ipv4, $reg, $numbers;
				$flag_proceeded = 3;
				last;
			};
		};

		if ($flag_proceeded == 1) {
			# next one...
			next;
		} elsif ($flag_proceeded == 0) {
			die "Shouldn't happen";
		};

		# now the harder work...
		my $newnumbers = $numbers;
		while ($newnumbers > 0) {
			#printf "Newnumbers: %d   Length: %d\n", $newnumbers, $check_length;

			while ( $newnumbers < $subnet_powers{$check_length}->{'numbers'} ) {
				$check_length++;
			};

			if ( ( $ipv4_dec & (~ $subnet_powers{$check_length}->{'mask'}) ) == 0 ) {
				push @$parray, $ipv4 . "/" . $check_length;
				#printf "%s/%d=%s (partially catch case 1b or 2: %d)\n", &dec_to_ipv4($ipv4_dec), $check_length, $reg, $subnet_powers{$check_length}->{'numbers'};
				$newnumbers -= $subnet_powers{$check_length}->{'numbers'};
				$ipv4_dec += $subnet_powers{$check_length}->{'numbers'};

				next;
			} else {
				$check_length++;
				if ($check_length > 32) {
					die "Shouldn't happen";
				};
			};
		};
	};

	close(FILE);
};

sub proceed_array($$) {
	my $parray = shift || die "missing array pointer";
	my $parray_agg = shift || die "missing array pointer";

	scalar(@$parray) == 0 && die "array empty!";

	print "Start proceeding array with 'aggregate' (Entries: " . scalar(@$parray) . ")\n";

	my $pid = open2(AGGREGATE_READ, AGGREGATE_WRITE, "aggregate -t") || die "cannot for: $!";
	
	foreach my $entry (@$parray) {
		# filter out longer prefix length



		print AGGREGATE_WRITE $entry . "\n";
	};
	close(AGGREGATE_WRITE);

	while (<AGGREGATE_READ>) {
		my $line = $_;
		chomp $line;
		push @$parray_agg, $line;
	};

	close(AGGREGATE_READ);

	print "End proceeding array with 'aggregate' (Entries: " . scalar(@$parray_agg) . ")\n";
};



print "Aggregate RIPENCC\n";
&proceed_array(\@ripencc, \@ripencc_agg);

print "Aggregate APNIC\n";
&proceed_array(\@apnic, \@apnic_agg);

print "Aggregate IANA\n";
&proceed_array(\@iana, \@iana_agg);

print "Aggregate LACNIC\n";
&proceed_array(\@lacnic, \@lacnic_agg);

print "Aggregate AFRINIC\n";
&proceed_array(\@afrinic, \@afrinic_agg);

if (1 == 0) {
	# Look for maximum used prefix length
	my ($net, $length);
	for my $entry (@ripencc_agg, @apnic_agg, @iana_agg) {
		my ($net, $length) = split /\//, $entry;
		if ($length > $max_prefixlength_not_arin) {
			$max_prefixlength_not_arin = $length;
		};
	};

	print "Maximum used prefix length by not ARIN: " . $max_prefixlength_not_arin . "\n";

	## Run filter of ARIN entries
	print "Run filter on ARIN entries\n";
	# 1. overwrite prefix length and network
	for (my $i = 0; $i < $#arin; $i++) {
		my ($net, $length) = split /\//, $arin[$i];
		if ($length > $max_prefixlength_not_arin) {
			$arin[$i] = &dec_to_ipv4(&ipv4_to_dec($net) & $subnet_powers{$max_prefixlength_not_arin}->{'mask'}) . "\/" . $max_prefixlength_not_arin;
		};
	};
	# 2. remove duplicates
	my @arin_new;
	push @arin_new, $arin[0];	
	for (my $i = 1; $i < $#arin; $i++) {
		if ($arin[$i] eq $arin[$i - 1]) {
			next;
		} else {
			push @arin_new, $arin[$i];	
		};
	};
	print "End of filter on ARIN entries\n";
};

print "Aggregate ARIN (this can take some time...)\n";
&proceed_array(\@arin, \@arin_agg);


# Create header file


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
 *  Additional header file for libipv4addr.c
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
print OUT "static const char* dbipv4addr_registry_status = \"$string\";\n";


# Create hash
my %data;

sub fill_data($$) {
	my $parray = shift || die "missing array pointer";
	my $reg = shift || die "missing registry";

	foreach my $entry (sort @$parray) {
		my ($ipv4, $length) = split /\//, $entry;

		my $ipv4_hex = sprintf("%08x", &ipv4_to_dec($ipv4));
		my $mask_hex = sprintf("%08x", &length_to_dec($length));
		my $mask_length = sprintf("%2d", $length);
		
		$data{$ipv4_hex}->{'ipv4'} = &ipv4_to_dec($ipv4);
		$data{$ipv4_hex}->{'mask'} = &length_to_dec($length);
		$data{$ipv4_hex}->{'mask_hex'} = $mask_hex;
		$data{$ipv4_hex}->{'mask_length'} = $mask_length;
		$data{$ipv4_hex}->{'reg'} = $reg;
	};
};

&fill_data(\@apnic_agg, "APNIC");
&fill_data(\@ripencc_agg, "RIPENCC");
&fill_data(\@arin_agg, "ARIN");
&fill_data(\@lacnic_agg, "LACNIC");
&fill_data(\@afrinic_agg, "AFRINIC");
&fill_data(\@iana_agg, "IANA");

my %data_hint;

# Main data structure
print OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment[] = {
|;


my $i = 0;
foreach my $ipv4_hex (sort keys %data) {
	printf OUT "\t{ 0x%s, 0x%s, %2d, \"%s\" },\n", $ipv4_hex, $data{$ipv4_hex}->{'mask_hex'}, $data{$ipv4_hex}->{'mask_length'}, $data{$ipv4_hex}->{'reg'};

	printf "ipv4_hex=0x%s, mask_hex=0x%s, reg=\"%s\"", $ipv4_hex, $data{$ipv4_hex}->{'mask_hex'}, $data{$ipv4_hex}->{'reg'} if ($debug_hinttable);

	# Get hint range
	if (($data{$ipv4_hex}->{'mask'} & 0xff000000) == 0xff000000) {
		# Mask is between /8 and /32 
		printf " hint: mask >= /8" if ($debug_hinttable);
		my $octet_leading = substr($ipv4_hex, 0, 2);
		if (! defined $data_hint{$octet_leading}->{'start'}) {
			# set start and end
			$data_hint{$octet_leading}->{'start'} = $i;
			$data_hint{$octet_leading}->{'end'} = $i;
			printf " new to: 0x%s\n", $octet_leading if ($debug_hinttable);
		} else {
			# extend end
			$data_hint{$octet_leading}->{'end'} = $i;
			printf " append to: 0x%s\n", $octet_leading if ($debug_hinttable);
		};
	} else {
		# Mask is between /1 and /7, more work...
		printf " hint: mask < /8" if ($debug_hinttable);
		my $count = (($data{$ipv4_hex}->{'mask'} & 0xff000000) >> 24) ^ 0xff;
		printf " count: %d", $count if ($debug_hinttable);
		for (my $j = 0; $j <= $count; $j++) {
			my $octet_leading = sprintf("%02x", ($data{$ipv4_hex}->{'ipv4'} >> 24) +  $j);
			if (! defined $data_hint{$octet_leading}->{'start'}) {
				# set start and end
				$data_hint{$octet_leading}->{'start'} = $i;
				$data_hint{$octet_leading}->{'end'} = $i;
				printf " hint: new to 0x%s", $octet_leading if ($debug_hinttable);
			} else {
				# extend end
				$data_hint{$octet_leading}->{'end'} = $i;
				printf " hint: append to 0x%s", $octet_leading if ($debug_hinttable);
			};
		};
		printf "\n" if ($debug_hinttable);
	};

	$i++;
};

print OUT qq|
};
|;

# Hint table data structure
print OUT qq|
static const s_ipv4addr_assignment_hint dbipv4addr_assignment_hint[256] = {
|;

for (my $j = 0; $j < 256; $j++) {
	my $string = sprintf("%02x", $j);
	my $value_start;
	my $value_end;

	if (defined $data_hint{$string}) {
		$value_start = $data_hint{$string}->{'start'};
		$value_end = $data_hint{$string}->{'end'};
	} else {
		$value_start = -1;
		$value_end = -1;
	};

	printf OUT "\t{ 0x%s, %d , %d },\n", $string, $value_start, $value_end;
};

print OUT qq|
};
|;


print "Finished\n";
