CC=g++
CFLAGS=-std=c++17 -g2 -O0

INCLUDES=protocol.hpp magic.hpp
EXES=initiator responder
all: $(EXES)

%: %.cpp %.hpp $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ $<

pipe_right pipe_left:
	#mkfifo fifo
	#socat -d -d  pipe:pipe_left pipe:pipe_right &
	socat -d -d pty,raw,echo=0,link=./pipe_left pty,raw,echo=0,link=./pipe_right &

sim: $(EXES) pipe_right pipe_left
	./responder pipe_right &
	./initiator pipe_left &

clean:
	rm -rf *.o
	rm $(FIFO)
	rm $(EXES)
