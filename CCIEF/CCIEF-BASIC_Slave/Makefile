Slave_sample: SLMP.o SOCKET.o TIMER.o CCIEF_BASIC_SLAVE.o SLMP_SERVER.o USER_SAMPLE.o
	gcc -o Slave_sample SLMP.o SOCKET.o TIMER.o CCIEF_BASIC_SLAVE.o SLMP_SERVER.o USER_SAMPLE.o

SLMP.o: /root/CCIEF-BASIC_Slave/library/src/SLMP.c
	gcc -I/root/CCIEF-BASIC_Slave/library/include -c /root/CCIEF-BASIC_Slave/library/src/SLMP.c
SOCKET.o: /root/CCIEF-BASIC_Slave/sample/src/SOCKET.c
	gcc -I/root/CCIEF-BASIC_Slave/sample/include -c /root/CCIEF-BASIC_Slave/sample/src/SOCKET.c
TIMER.o: /root/CCIEF-BASIC_Slave/sample/src/TIMER.c
	gcc -I/root/CCIEF-BASIC_Slave/sample/include -c /root/CCIEF-BASIC_Slave/sample/src/TIMER.c
CCIEF_BASIC_SLAVE.o: /root/CCIEF-BASIC_Slave/sample/src/CCIEF_BASIC_SLAVE.c
	gcc -I/root/CCIEF-BASIC_Slave/sample/include -I/root/CCIEF-BASIC_Slave/library/include -c /root/CCIEF-BASIC_Slave/sample/src/CCIEF_BASIC_SLAVE.c
SLMP_SERVER.o: /root/CCIEF-BASIC_Slave/sample/src/SLMP_SERVER.c
	gcc -I/root/CCIEF-BASIC_Slave/sample/include -I/root/CCIEF-BASIC_Slave/library/include -c /root/CCIEF-BASIC_Slave/sample/src/SLMP_SERVER.c
USER_SAMPLE.o: /root/CCIEF-BASIC_Slave/sample/src/USER_SAMPLE.c
	gcc -I/root/CCIEF-BASIC_Slave/sample/include -I/root/CCIEF-BASIC_Slave/library/include -c /root/CCIEF-BASIC_Slave/sample/src/USER_SAMPLE.c