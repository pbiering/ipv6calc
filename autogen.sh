#!/bin/sh

$Id: autogen.sh,v 1.1 2003/11/21 10:31:21 peter Exp $

autoconf

./configure --bindir=/usr/bin --mandir=/usr/share/man

make clean
make
make test

make -n installonly

echo
echo "For installing the binaries, type: make installonly"

