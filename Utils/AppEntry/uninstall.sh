#! /bin/sh

name=$1
service $name stop
result="$?"
test "$result" -eq 0 -o "$result" -eq 1 || exit 1
update-rc.d -f $name remove || exit 2
rm -f /etc/init.d/$name || exit 3
exit 0