#!/bin/sh

if [ ! -e /tmp/sdcard ]; then
  mkdir /tmp/sdcard
  mount /dev/mmcblk1p1 /tmp/sdcard
  ln -s /tmp/sdcard /var/www/html/sdcard
fi

if [ ! -e /tmp/flashusb ]; then
  mkdir /tmp/flashusb
  mount /dev/sda1 /tmp/flashusb
  ln -s /tmp/flashusb /var/www/html/flashusb
fi
mkdir /var/www/html/Dairy
cp /var/www/html/tmp/*.* /tmp
ln -s /tmp/currentDataTable.php /var/www/html/currentDataTable.php
ln -s /tmp/trendMistLv.php /var/www/html/trendMistLv.php
ln -s /tmp/currentMistLv.php /var/www/html/currentMistLv.php
ln -s /tmp/alarm.log /var/www/html/alarm.log
ln -s /tmp/memo.txt /var/www/html/memo.txt
ln -s /tmp/trendGraph.php /var/www/html/trendGraph.php
ln -s /tmp/currentGraph.php /var/www/html/currentGraph.php
