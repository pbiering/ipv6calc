#!/bin/bash

perl -pi -e 's/^(my \$program_version).*$/\1 = "\@PACKAGE_VERSION\@";/' ipv6calcweb.cgi.in
perl -pi -e 's/^(my \$program_copyright).*$/\1 = "(P) & (C) 2002-\@COPYRIGHT_YEAR\@ by Peter Bieringer";/' ipv6calcweb.cgi.in
