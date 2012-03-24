CXX=g++
CXXFLAGS=
LDFLAGS=
ifeq ($(DEBUG),)
	CXXFLAGS=-O3
else
	ifeq ($(PROFILE),)
		CXXFLAGS=-ggdb -DDEBUG
	else
		CXXFLAGS=-pg
	endif
endif

ifneq ($(BOOST_ROOT),)
	CXXFLAGS+=-I$(BOOST_ROOT)/include
	LDFLAGS+=-L$(BOOST_ROOT)/lib -Wl,-rpath=$(BOOST_ROOT)/lib
endif

CXXFLAGS+=-Wall -Wextra --std=gnu++0x

all: stattest

stattest: stattest.o stat.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

stat.o: stat.cpp stat.h

stattest.o: stattest.cpp stat.h

clean:
	$(RM) stattest stat.o stattest.o
