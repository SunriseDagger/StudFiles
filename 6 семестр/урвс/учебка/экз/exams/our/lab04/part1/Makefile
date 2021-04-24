SOURCENAME = summator.cpp
TARGETNAME = summator
AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CPPFLAGS = -O2 -Wall -Wfatal-errors
GCC = g++

.SILENT:clean
.SILENT:test
.SILENT:doc

all:
	$(GCC) $(CPPFLAGS) $(SOURCENAME) -o $(TARGETNAME)
clean:
	rm $(TARGETNAME) 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

