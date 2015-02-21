PREFIX ?= /usr/local

INSTALL ?= install

CXXFLAGS ?= -O3 -march=native -DNDEBUG  -mfpmath=sse -Wall

.PHONY: install all clean 

all: ladspam-0-plugins.so

install: all
	$(INSTALL) $(LADSPAM_HEADERS) $(INCLUDE_PATH)

ladspam-0-plugins.so: ladspam-plugins.cc
	g++ $(CXXFLAGS) -std=c++11 -shared -fPIC -o ladspam-0-plugins.so ladspam-plugins.cc

clean:
	rm -f ladspam-0-plugins.so
	
