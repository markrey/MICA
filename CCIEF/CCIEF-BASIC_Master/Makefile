Master_sample: SLMP.o SOCKET.o TIMER.o CCIEF_BASIC_SLAVES.o CCIEF_BASIC_MASTER.o MQTT_PUB.o
	gcc -o Master_sample SLMP.o SOCKET.o TIMER.o CCIEF_BASIC_SLAVES.o CCIEF_BASIC_MASTER.o MQTT_PUB.o -lmosquitto

SLMP.o: /root/CCIEF-BASIC_Master/library/src/SLMP.c
	gcc -I/root/CCIEF-BASIC_Master/library/include -c /root/CCIEF-BASIC_Master/library/src/SLMP.c
SOCKET.o: /root/CCIEF-BASIC_Master/sample/src/SOCKET.c
	gcc -I/root/CCIEF-BASIC_Master/sample/include -c /root/CCIEF-BASIC_Master/sample/src/SOCKET.c
TIMER.o: /root/CCIEF-BASIC_Master/sample/src/TIMER.c
	gcc -I/root/CCIEF-BASIC_Master/sample/include -c /root/CCIEF-BASIC_Master/sample/src/TIMER.c
CCIEF_BASIC_SLAVES.o: /root/CCIEF-BASIC_Master/sample/src/CCIEF_BASIC_SLAVES.c
	gcc -I/root/CCIEF-BASIC_Master/sample/include -I/root/CCIEF-BASIC_Master/library/include -c /root/CCIEF-BASIC_Master/sample/src/CCIEF_BASIC_SLAVES.c
CCIEF_BASIC_MASTER.o: /root/CCIEF-BASIC_Master/sample/src/CCIEF_BASIC_MASTER.c
	gcc -I/root/CCIEF-BASIC_Master/sample/include -I/root/CCIEF-BASIC_Master/library/include -c /root/CCIEF-BASIC_Master/sample/src/CCIEF_BASIC_MASTER.c
MQTT_PUB.o: /root/CCIEF-BASIC_Master/sample/src/MQTT_PUB.c
	gcc -lmosquitto -I/root/CCIEF-BASIC_Master/sample/include -I/root/CCIEF-BASIC_Master/library/include -c /root/CCIEF-BASIC_Master/sample/src/MQTT_PUB.c
