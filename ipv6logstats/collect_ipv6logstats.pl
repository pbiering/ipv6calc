#!/usr/bin/perl -w
#
# Project    : ipv6calc/logstats
# File       : collect_ipv6logstats.pl
# Version    : $Id: collect_ipv6logstats.pl,v 1.1 2003/11/22 15:02:44 peter Exp $
# Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
#
# Collect ipv6logstats data

my $dir = "./examples-data";

my $file_pattern = "^data-[0-9]{6}.txt\$";

my %data;

my @token_list = ("ALL", "IPv4", "IPv6");


## Functions ##

sub read_file($) {
	my $token;

	if ( ! -r $_[0] ) {
		die "Cannot read: " . $_[0];
	};

	open(FILE, $_[0]) || die "Cannot open file: " . $_[0];

	print STDERR "INFO: Read content of file: " . $_[0] . "\n";
	while (<FILE>) {
		chomp $_;
		my ($t, $v) = split / +/, $_;
		#print STDERR "DEBUG: found token: " . $t . "(" . $v . ")\n";
		if ($t =~ /^\*/) {
			# special token/value pair
			if ($t eq "*Token") {
				$token = $v;
				next;
			};
		} else {
			if (! defined $token) {
				die "Missing '*Token' in data file";
			};
			# values
			$data{$token}->{$t} = $v;
		};
	};
	close FILE;
};


sub print_all_data() {
	# Print header
	print "#HEADER ";
	foreach my $token (sort keys %data) {
		foreach my $t (sort keys %{$data{$token}} ) {
			print $t . " ";
		};
		print "\n";
		last;
	};

	# Print data
	foreach my $token (sort keys %data) {

		print $token . " ";
		foreach my $t (sort keys %{$data{$token}} ) {
			print $data{$token}->{$t} . " ";
		};
		print "\n";
	};
};

sub print_selected_data() {
	if (1 == 0) {
		# Print header
		print "#HEADER ";
		foreach my $token (sort keys %data) {
			foreach my $t (@token_list) {
				print $t . " ";
			};
			print "\n";
			last;
		};
	};

	# Print data
	foreach my $token (sort keys %data) {

		print $token . " ";
		foreach my $t (@token_list) {
			print $data{$token}->{$t} . " ";
		};
		print "\n";
	};
};

## Main


if ( ! -d $dir ) {
	die "Missing data directory: $dir";
};

opendir(DIR, $dir) || die "Cannot read directory: $dir";
my @files = grep { /$file_pattern/ } readdir(DIR);
closedir DIR;

print STDERR "INFO: number of files matching pattern '$file_pattern' found: " . $#files . "\n";

foreach my $file (@files) {
	read_file($dir . "/" . $file);
};
print_selected_data();
