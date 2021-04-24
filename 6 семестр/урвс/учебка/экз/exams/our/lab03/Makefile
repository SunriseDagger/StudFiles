SOURCENAME = tablegen.c
TARGETNAME = tablegen
AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CFLAGS = -O2 -Wall -Wfatal-errors
GCC = cc

.SILENT:clean
.SILENT:test
.SILENT:doc

all:
	$(GCC) $(CFLAGS) $(SOURCENAME) -o $(TARGETNAME)
clean:
	rm $(TARGETNAME) file.txt file1.txt 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

