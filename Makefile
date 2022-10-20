CC=g++
CFLAGS=-std=c++17 -g2

INCLUDES=protocol.hpp magic.hpp
EXES=initiator responder
FIFO=fifo

all: $(EXES)

%: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ $<

fifo:
	mkfifo $(FIFO)

sim: $(EXES) $(FIFO)
	./responder $(FIFO) &
	./initiator $(FIFO) &

clean:
	rm -rf *.o
	rm $(FIFO)
	rm $(EXES)
