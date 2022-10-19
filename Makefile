CC=g++
CFLAGS=-std=c++17 -g2

INCLUDES=protocol.hpp magic.hpp

all: initiator responder

initiator: initiator.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ $<

responder: responder.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ $<	

clean:
	rm -rf *.o
	rm initiator responder
