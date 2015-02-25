UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS= -Wall -g -I/usr/include/eigen3 -I../utils/tinyxml2 
endif

ifeq ($(UNAME), Darwin)
CPPFLAGS= -Wall -g -I/opt/local/include/eigen3 -I../utils/tinyxml2
endif

CPPFLAGS+= -std=c++11 -O2
LDFLAGS+= -std=c++11
LD = g++
CC = g++

VPATH = ../../utils/tinyxml2
LDLIBS = -L./ -lprosit

PROSIT_OBJS = pmf.o cdf.o auxiliary_func.o probability_solver.o task_descriptor.o qbd_rr_solver.o qbd_latouche_solver.o qbd_cr_solver.o qbd_companion_solver.o qbd_analytic_solver.o

LIBS = libprosit.a

all: solver

solver: solver.o libprosit.a
	$(LD) $(LD_FLAGS) solver.o $(LDLIBS) -o solver

libprosit.a: $(PROSIT_OBJS)
	ar rc $@ $(PROSIT_OBJS)
	ar -s $@

.PHONY: clean

clean:
	rm -f *.o *.d *.a

%.d: %.cpp
	$(CC) $(CPPFLAGS) -MM -MF $@ $<

-include $(PROSIT_OBJS:.o=.d)