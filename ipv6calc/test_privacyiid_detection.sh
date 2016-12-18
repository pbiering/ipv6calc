#!/bin/bash
#
# Project    : ipv6calc
# File       : test_privacyiid_detection.sh
# Version    : $Id$
# Copyright  : 2012-2012 by Peter Bieringer <pb (at) bieringer.de>
#
# Test script for detection of a privacy IID

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi


max=1000000
i=0

iid="0224:21ff:fe00:0001"
token="0123456789abcdef"

generate() {
	echo "INFO  : generate IPv6 addresses with privacy IID: $max" >&2
	while [ $i -lt $max ]; do
		iidtoken_new="`./ipv6calc -q -F -A genprivacyiid $iid $token`"
		if [ -z "$iidtoken_new" ]; then
			exit 1
		fi
		iid="${iidtoken_new/ *}"
		token="${iidtoken_new/* }"
		#echo "$iid $token"
		ipv6="2001:0db8:0000:0000:$iid"
		echo "$ipv6"
	 	i=$[ $i + 1 ]
	done
}

analyze-hexvar() {
	f="$1"
	limit_low="$2"
	limit_high="$3"

	if [ -z "$limit_low" ]; then
		echo "ERROR : missing limit_low (arg2)"
		return 1
	fi
	if [ -z "$limit_high" ]; then
		echo "ERROR : missing limit_high (arg3)"
		return 1
	fi

	echo "INFO : analyze file (hex digit variance): $f" >&2

	# hexvariance
	cat $f | ./ipv6calc --print-iid-var | awk -v limit_high=$limit_high -v limit_low=$limit_low '{
		h = $4;

		if (h >= limit_low && h <= limit_high) {
			if (h > max || max == 0) { max = h; };
			if (h < min || min == 0) { min = h; };

			print $1 " " h " min=" min " max=" max;
		};
	}'
}

analyze-digitblock() {
	f="$1"
	limit_low="$2"
	limit_high="$3"
	size="$4"

	if [ -z "$limit_low" ]; then
		echo "ERROR : missing limit_low (arg2)"
		return 1
	fi
	if [ -z "$limit_high" ]; then
		echo "ERROR : missing limit_high (arg3)"
		return 1
	fi
	if [ -z "$size" ]; then
		echo "ERROR : missing size (arg4)"
		return 1
	fi

	echo "INFO : analyze file (digit blocks): $f" >&2

	cat $f | ./ipv6calc --print-iid-var | awk -v limit_high=$limit_high -v limit_low=$limit_low -v size=$size '{
		h = $8;

		split(h, db, ",");

		f = db[size];

		if (f >= limit_low && f <= limit_high) {
			if (init != 1) {
				min = f;
				max = f;
				init = 1;
			};
			if (f > max) { max = f; };
			if (f < min) { min = f; };

			print $1 " " h " " size " f:" f " min=" min " max=" max;
		};
	}'
}

analyze-digitamount() {
	f="$1"
	limit_low="$2"
	limit_high="$3"
	digit="$4"

	if [ -z "$limit_low" ]; then
		echo "ERROR : missing limit_low (arg2)"
		return 1
	fi
	if [ -z "$limit_high" ]; then
		echo "ERROR : missing limit_high (arg3)"
		return 1
	fi
	if [ -z "$digit" ]; then
		echo "ERROR : missing digit (arg4)"
		return 1
	fi

	echo "INFO : analyze file (digit amount): $f" >&2

	cat $f | ./ipv6calc --print-iid-var | awk -v limit_high=$limit_high -v limit_low=$limit_low -v digit=$digit '{
		h = $10;

		split(h, db, ",");

		if (digit == "-1") {
			# all
			flag = 1;
			for (i = 1; i <= 16; i++) {
				f = db[i];
				if (f >= limit_low && f <= limit_high) {
					if (init != 1) {
						min = f;
						max = f;
						init = 1;
					};
					if (f > max) { max = f; };
					if (f < min) { min = f; };

				} else {
					flag = 0;
				};
			};
			if (flag == 1) {
				print $1 " " h " " size " f:" f " min=" min " max=" max;
			};
		} else {
			f = db[digit];

			if (f >= limit_low && f <= limit_high) {
				if (init != 1) {
					min = f;
					max = f;
					init = 1;
				};
				if (f > max) { max = f; };
				if (f < min) { min = f; };

				print $1 " " h " " size " f:" f " min=" min " max=" max;
			};
		};

	}'
}

analyze-digitdelta() {
	f="$1"
	limit_low="$2"
	limit_high="$3"
	digit="$4"

	if [ -z "$limit_low" ]; then
		echo "ERROR : missing limit_low (arg2)"
		return 1
	fi
	if [ -z "$limit_high" ]; then
		echo "ERROR : missing limit_high (arg3)"
		return 1
	fi
	if [ -z "$digit" ]; then
		echo "ERROR : missing digit (arg4)"
		return 1
	fi

	echo "INFO : analyze file (digit delta): $f limit_low=$limit_low limit_high=$limit_high digit=$digit" >&2

	cat $f | ./ipv6calc --print-iid-var | awk -v limit_high=$limit_high -v limit_low=$limit_low -v digit=$digit '{
		h = $12;

		split(h, db, ",");

		if (digit == "-1" || digit == "-2") {
			# all
			flag = 1;
			for (i = 1; i <= 31; i++) {
				f = db[i];
				if (f >= limit_low && f <= limit_high) {
					if (init != 1) {
						min = f;
						max = f;
						init = 1;
					};
					if (f > max) { max = f; };
					if (f < min) { min = f; };
					if (digit == "-2") {
						flag = 0;
					};
				} else {
					if (digit == "-1") {
						flag = 0;
					};
				};
			};
			if ((digit == "-1" && flag == 1) || (digit == "-2" && flag == 0)) {
				print $1 " " h " " size " f:" f " min=" min " max=" max;
			};
		} else {
			f = db[digit];

			if (f >= limit_low && f <= limit_high) {
				if (init != 1) {
					min = f;
					max = f;
					init = 1;
				};
				if (f > max) { max = f; };
				if (f < min) { min = f; };

				print $1 " " h " " size " f:" f " min=" min " max=" max;
			};
		};

	}'
}

analyze() {
	for f in $*; do
		# hexvariance
		echo "INFO : analyze file (hex digit variance): $f" >&2
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $4;
			if (init != 1) {
				min = h;
				max = h;
				init = 1;
			};
			if (h > max) { max = h; };
			if (h < min) { min = h; };

			print $1 " " h " min=" min " max=" max;
		}' | tail -1

		# linear_least_square
		echo
		echo "INFO : analyze file (linear least square fit): $f"
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $6;
			if (init != 1) {
				min = h;
				max = h;
				init = 1;
			};
			if (h > max) { max = h; };
			if (h < min) { min = h; };

			print $1 " " h " min=" min " max=" max;
		}' | tail -1

		# digit blocks
		echo
		echo "INFO : analyze file (digit blocks): $f"
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $8;

			split(h, db, ",");

			for (i = 1; i <= 16; i++) {
				if (init[i] != 1) {
					min[i] = h;
					max[i] = h;
					init[i] = 1;
				};
				if (db[i] > max[i]) { max[i] = db[i]; };
				if (db[i] < min[i]) { min[i] = db[i]; };

				if (db[i] > 0) {
					count[i]++;
				};
			};

			printf "%s ", $1;
			for (i = 1; i <= 16; i++) {
				printf "%d:%d/%d (%d) ", i, min[i], max[i], count[i];
			};
			printf "\n";
		}' | tail -1

		# digit amount
		echo
		echo "INFO : analyze file (digit amount): $f"
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $10;

			split(h, db, ",");

			for (i = 1; i <= 16; i++) {
				if (init[i] != 1) {
					min[i] = h;
					max[i] = h;
					init[i] = 1;
				};
				if (db[i] > max[i]) { max[i] = db[i]; };
				if (db[i] < min[i]) { min[i] = db[i]; };

				if (max[i] > maxall || maxall == 0) { maxall = max[i]; };

				if (db[i] > 0) {
					count[i]++;
				};
			};

			printf "%s ", $1;
			for (i = 1; i <= 16; i++) {
				printf "%d:%d/%d (%d) ", i-1, min[i], max[i], count[i];
			};
			printf "[maxall:%d]\n", maxall;
		}' | tail -1

		# digit delta
		echo
		echo "INFO : analyze file (digit delta): $f"
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $12;

			split(h, db, ",");

			for (i = 1; i <= 31; i++) {
				if (init[i] != 1) {
					min[i] = h;
					max[i] = h;
					init[i] = 1;
				};
				if (db[i] > max[i]) { max[i] = db[i]; };
				if (db[i] < min[i]) { min[i] = db[i]; };

				if (max[i] > maxall || maxall == 0) { maxall = max[i]; };

				if (db[i] > 0) {
					count[i]++;
				};
			};

			printf "%s ", $1;
			for (i = 1; i <= 31; i++) {
				printf "%d:%d/%d (%d) ", i-1, min[i], max[i], count[i];
			};
			printf "[maxall:%d] ", maxall;

			#printf "Digit-Delta-Min: ";
			for (i = 1; i <= 31; i++) {
				printf "%2d", min[i];
				if (i < 31) { printf ","; };
			};
			printf "|";
			#printf "Digit-Delta-Max: ";
			for (i = 1; i <= 31; i++) {
				printf "%2d", max[i];
				if (i < 31) { printf ","; };
			};
			printf "\n";
		}' | tail -1

		# digitdeltaamount
		echo
		echo "INFO : analyze file (digit delta amount): $f" >&2
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $14;
			if (init != 1) {
				min = h;
				max = h;
				init = 1;
			};
			if (h > max) { max = h; };
			if (h < min) { min = h; };

			print $1 " " h " min=" min " max=" max;
		}' | tail -1

	done

}

case $1 in
    generate)
	generate
	exit 0
	;;
    analyze)
	analyze $2 $3
	exit 0
	;;
    analyze-hexvar)
	analyze-hexvar $2 $3 $4
	exit 0
	;;
    analyze-digitblock)
	analyze-digitblock $2 $3 $4 $5
	exit 0
	;;
    analyze-digitamount)
	analyze-digitamount $2 $3 $4 $5
	exit 0
	;;
    analyze-digitdelta)
	analyze-digitdelta $2 $3 $4 $5
	exit 0
	;;
    *)
	echo "Usage: `basename $0` generate|analyze <file1> <file2>"
	exit 1
	;;
esac

