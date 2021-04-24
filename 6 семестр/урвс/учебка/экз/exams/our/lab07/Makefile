AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CPPFLAGS = -O2 -Wall -Wfatal-errors
GCC = g++

.SILENT:clean test doc
.PHONY:all clean test doc

all:
	$(GCC) $(CPPFLAGS) ur_lab07_var02.cpp -o ur_lab07_var02
clean:
	rm ur_lab07_var02 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

