AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CPPFLAGS = -O2 -Wall -Wfatal-errors
GCC = g++

.SILENT:clean
.SILENT:test
.SILENT:doc

all:
	$(GCC) $(CPPFLAGS) summator_parent.cpp -o summator_parent
	$(GCC) $(CPPFLAGS) summator_children.cpp -o summator_children
clean:
	rm summator_parent summator_children 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

