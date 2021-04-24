.SILENT:clean test
.PHONY:all clean test deneb atom
all:
	g++ cbss.cpp csrs.cpp main.cpp -lgomp -lpthread -fopenmp -O3 -Wall -Wextra -o mo_lab04_var04
deneb:
	g++ cbss.cpp csrs.cpp main.cpp -lgomp -lpthread -fopenmp -O3 -Wall -Wextra -march=barcelona -mfpmath=sse -msse -msse2 -msse3 -msse4a -ffast-math -o mo_lab04_var04
atom:
	g++ cbss.cpp csrs.cpp main.cpp -lgomp -lpthread -fopenmp -O3 -Wall -Wextra -march=atom -mfpmath=sse -msse -msse2 -msse3 -mssse3 -ffast-math -mno-8bit-idiv -o mo_lab04_var04
clean:
	rm mo_lab04_var04 2>/dev/null || true
test:
	./mo_lab04_var04

