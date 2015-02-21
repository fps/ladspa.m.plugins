PREFIX ?= /usr/local

INSTALL ?= install

CXXFLAGS ?= -O3 -march=native -DNDEBUG  -mfpmath=sse -Wall

.PHONY: install all clean 

all: ladspam-0-osc-plugins.so

install: all
	$(INSTALL) -d $(PREFIX)/lib/ladspa
	$(INSTALL) ladspam-0-osc-plugins.so $(PREFIX)/lib/ladspa

ladspam-0-osc-plugins.so: ladspam-osc-plugins.cc
	g++ $(CXXFLAGS) -std=c++11 -shared -fPIC -o ladspam-0-osc-plugins.so ladspam-osc-plugins.cc

clean:
	rm -f ladspam-0-osc-plugins.so
	
