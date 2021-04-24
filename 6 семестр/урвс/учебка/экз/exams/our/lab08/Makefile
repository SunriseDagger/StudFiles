AUTOTEST = ./autotest.sh
DOXYGENCFG = ./doxygen.cfg
CPPFLAGS = -O2 -Wall -Wfatal-errors
GCC = g++

.SILENT:clean test doc
.PHONY:all clean test doc

all:
	$(GCC) $(CPPFLAGS) -lpthread -std=c++0x src_client/main.cpp -o chat_client
	$(GCC) $(CPPFLAGS) -Isrc_server src_server/cserverchat.cpp src_server/Csock.cpp src_server/main.cpp -o chat_server
clean:
	rm chat_client chat_server 2>/dev/null || true
test:
	$(AUTOTEST) 2>/dev/null
doc:
	doxygen $(DOXYGENCFG)

