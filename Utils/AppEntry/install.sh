#! /bin/sh

source=$2
dest=$1
mv -f $source /etc/init.d/$dest || exit 1
chmod +x /etc/init.d/$dest || exit 2
update-rc.d $dest start 80 2 3 4 5 . stop 01 0 1 6 . || exit 3
exit 0