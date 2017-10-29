#! /bin/sh

svcName=$1
svcDesc=$2
daemonPath=$3
path=$4
startOrder=$5
stopOrder=$6
source=$path/Configuration/tempscript

rm -f $source
sed  "s/@svcName/$svcName/g" $path/Configuration/skeleton | \
sed  "s/@svcDesc/$svcDesc/g" $path/Configuration/skeleton | \
sed  "s/@daemonPath/$daemonPath/g" $path/Configuration/skeleton | \
sed  "s/@path/$path/g" $path/Configuration/skeleton > $source

mv -f $source /etc/init.d/$svcName || exit 1
chmod +x /etc/init.d/$svcName || exit 2
update-rc.d $svcName start $startOrder 2 3 4 5 . stop $stopOrder 0 1 6 . || exit 3
exit 0