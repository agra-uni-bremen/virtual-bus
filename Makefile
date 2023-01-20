CC=g++
CFLAGS=-std=c++17 -g2 -Wall

INCLUDES=protocol.hpp magic.hpp
OBJECTS=protocol.o initiator.o responder.o
EXES=initiator-cli responder-cli

all: $(EXES)

.PRECIOUS: %.o # lol, make deletes intermediate files

%.o: %.cpp %.hpp $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<

%-cli: %-cli.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -pthread -o $@ $< $(OBJECTS)

pipe_right pipe_left:
	#mkfifo fifo
	#socat -d -d  pipe:pipe_left pipe:pipe_right &
	socat -d -d pty,raw,echo=0,link=./pipe_left pty,raw,echo=0,link=./pipe_right &

sim: $(EXES) pipe_right pipe_left
	./responder-cli pipe_right &
	./initiator-cli pipe_left &

sim-stop:
	killall socat || true
	killall responder-cli || true
	killall initator-cli || true

clean:
	killall socat
	rm $(EXES) $(OBJECTS)
