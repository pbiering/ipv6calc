#!/bin/sh

$Id: autogen.sh,v 1.2 2003/11/21 10:38:17 peter Exp $

autoconf || exit 1

./configure --bindir=/usr/bin --mandir=/usr/share/man || exit 1

make clean || exit 1
make || exit 1
make test || exit 1

make -n installonly || exit 1

echo
echo "For installing the binaries, type: make installonly"

