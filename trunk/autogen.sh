#!/bin/sh

echo "Just type 'scons'"
exit 0

aclocal-1.7
automake-1.7 -f --copy --add-missing
autoheader
autoconf

# EOF #