all: libnet

libnet: utils.o
	ar -cvq libnet.a *.o

utils.o: Utils.h
	g++ -c Utils.cpp
