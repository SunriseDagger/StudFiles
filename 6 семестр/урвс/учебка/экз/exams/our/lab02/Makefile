SOURCENAME = lsd.c
TARGETNAME = lsd
AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CFLAGS = -O2 -Wall
GCC = cc

.SILENT:clean
.SILENT:test
.SILENT:doc

all:
	$(GCC) $(CFLAGS) $(SOURCENAME) -o $(TARGETNAME)
clean:
	rm $(TARGETNAME) 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

