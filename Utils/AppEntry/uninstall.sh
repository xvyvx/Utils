#! /bin/sh

serviceType=$1
name=$2
if [ "$serviceType" = "systemV" ]; then
    service $name stop
    result="$?"
    test "$result" -eq 0 -o "$result" -eq 1 || exit 1
    update-rc.d -f $name remove || exit 2
    rm -f /etc/init.d/$name || exit 3
else
    systemctl stop $name.service
    systemctl disable $name.service || exit 1
    rm -f /etc/systemd/system/$name.service || exit 2
fi
exit 0