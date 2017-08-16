rm MDSX
rm MDSX_makeFile
rm MDSX_serial
rm MDSX_mqttPub
#gcc -W -g MDSX.c -o MDSX
gcc -W -g MDSX_makeFile.c -o MDSX_makeFile
gcc -W -g MDSX_serial.c -o MDSX_serial
gcc -W -g MDSX_mqttPub.c -o MDSX_mqttPub -lmosquitto

