#! /bin/sh

installType=$1
svcName=$2
svcDisplayName=$3
daemonPath=$4
daemonExecutePath=$5
startOrder=$6
stopOrder=$7
svcDepends=$8

if [ "$installType" = "systemV" ]; then
    sed "s?@svcName?$svcName?g" ./Configuration/skeleton-systemV | \
    sed "s?@svcDisplayName?$svcDisplayName?g" | \
    sed "s?@daemonPath?$daemonPath?g" | \
    sed "s?@daemonExecutePath?$daemonExecutePath?g" > ./Configuration/tempscript || exit 1

    mv -f ./Configuration/tempscript /etc/init.d/$svcName || exit 2
    chmod +x /etc/init.d/$svcName || exit 3
    update-rc.d $svcName start $startOrder 2 3 4 5 . stop $stopOrder 0 1 6 . || exit 4
else
    sed "s?@svcName?$svcName?g" ./Configuration/skeleton-systemd | \
    sed "s?@svcDisplayName?$svcDisplayName?g" | \
    sed "s?@daemonPath?$daemonPath?g" | \
    sed "s?@daemonExecutePath?$daemonExecutePath?g" | \
    sed "s?@svcDepends?$svcDepends?g" > ./Configuration/tempservice.service || exit 1

    mv -f ./Configuration/tempservice.service /etc/systemd/system/$svcName.service || exit 2
    systemctl enable $svcName.service || exit 3
fi
exit 0