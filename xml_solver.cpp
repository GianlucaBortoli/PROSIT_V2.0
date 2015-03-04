///@file Solver for computation of stady state probabilities from XML
#include "pmf.hpp"
#include "auxiliary_func.hpp"
#include "qbd_rr_solver.hpp"
#include "qbd_companion_solver.hpp"
#include "qbd_analytic_solver.hpp"
#include "exc.hpp"
#include "xml_parser.hpp"
#include "xml_utils.hpp"
#include "qos_function.hpp"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <getopt.h>
#include <memory>
#include <tinyxml2.h>

bool verbose_flag; //used also in xml_utils.cpp
bool silent_flag; //used also in xml_utils.cpp
bool help_flag;

static int opts_parse(int argc, char *argv[]) {
  int opt;
  static struct option long_options[] = {
      {"verbose", no_argument, 0, 'v'},
      {"silent",  no_argument, 0, 's'},
      {"help",    no_argument, 0, 'h'},
      {0, 0, 0, 0},
  };

  //defaults values
  verbose_flag = false;
  silent_flag = false;
  help_flag = false;

  while ((opt = getopt_long(argc, argv, ":vhs",long_options,0)) != -1) {
    switch (opt) {
    case 'v': 
      verbose_flag=true;
      silent_flag = true;
      break;
    case 's':
      silent_flag = true;
      verbose_flag = false;
      break;
    case 'h':
      PrositAux::help_xml();
      break;
    default: /* ?~@~Y??~@~Y */
      EXC_PRINT("opts_parse parameters incorrect");
    }
  }
  return optind;
}

int main(int argc, char *argv[]) {
  double scale = 0; //defaults values
  double pmin = 0; 
  double pmax = 1; 
  double offset = 0.95;

  PrositCore::QosFunction *q = new PrositCore::QosFunction(scale, pmin, pmax, offset); //function for qos to be respected

  try {
    int opt = opts_parse(argc, argv);
    if( argc-optind != 1 )
      EXC_PRINT("one input file requested");

    PrositCore::Parser *p = new PrositCore::Parser(argv[opt]);
    p->parse();
    if(verbose_flag)
      cout << "XML file succesfully parsed" << endl;
   
    switch(p->get_act()) {
    case PrositCore::ACTIONS::OPTIMISE: 
      PrositCore::opt_execute(p); 
      break;
    case PrositCore::ACTIONS::SOLVE:
      PrositCore::solve_execute();
      break;
    default:
      EXC_PRINT("Action not currently recognised");
    }

    delete p;
    delete q;
  } catch (PrositAux::Exc &e) {
    cerr << "Exception caught" << endl;
    e.what();
  }
}