UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS = -Wall -g -I /usr/include/eigen3 -I ./utils/tinyxml2 
endif

ifeq ($(UNAME), Darwin)
CPPFLAGS = -Wall -g -I /opt/local/include/eigen3 -I ./utils/tinyxml2
endif

CPPFLAGS += -std=c++11
LD = g++
CC = g++
LDLIBS = -L./ -lprosit

PROSIT_OBJS = pmf.o cdf.o auxiliary_func.o probability_solver.o task_descriptor.o \
qbd_rr_solver.o qbd_latouche_solver.o qbd_cr_solver.o qbd_companion_solver.o \
qbd_analytic_solver.o xml_parser.o xml_utils.o qos_function.o ./utils/tinyxml2/tinyxml2.o

all: solver xml_solver

solver: solver.o libprosit.a
	$(LD) solver.o $(LDLIBS) -o solver

xml_solver: xml_solver.o libprosit.a
	$(LD) xml_solver.o $(LDLIBS) -o xml_solver

libprosit.a: $(PROSIT_OBJS)
	ar rc $@ $(PROSIT_OBJS)
	ar -s $@

.PHONY: clean rmtest

clean: rmtest
	rm -f *.o *.d *.a solver xml_solver
	@#Removes all beta_computation/interarrival.txt dumped my xml solver 
	find . -name \beta_computation.txt -exec rm -f {} +
	find . -name \beta_interarrival.txt -exec rm -f {} +

rmtest:
	rm -f test/my_*.txt test/results_*.txt test/check_diffs_*.log
	find ./test -name 'xml_test_[0-9]*_normal.xml' -exec rm -f {} +
	find ./test -name 'xml_test_[0-9]*_beta.xml' -exec rm -f {} +

%.d: %.cpp
	@$(CC) $(CPPFLAGS) -MM -MF $@ $<

-include $(PROSIT_OBJS:.o=.d)
