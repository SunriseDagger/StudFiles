CC=g++
CFLAGS = -Wall
LIBS=-lm

all: calculate
		
calculate: calculate.cpp utils.cpp
	$(CC) $(CFLAGS) calculate.cpp utils.cpp -o ../calculate 

.PHONY: clean

clean:
	rm -f calculate calculate.o
