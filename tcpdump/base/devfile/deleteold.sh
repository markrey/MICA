#!/bin/sh
DVAL=`/bin/df /dev/mmcblk1p1 | /usr/bin/tail -1 | /bin/sed 's/^.* \([0-9]*\)%.*$/\1/'`
if [ $DVAL -gt 90 ]; then
  echo "Disk usage alert: $DVAL %" 
  ls -tr /tmp/sdcard/*.cap | head -1
  rm `ls -tr /tmp/sdcard/*.cap | head -1`
fi

