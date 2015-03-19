///@file Solver for computation of stady state probabilities from XML
#include "auxiliary_func.hpp"
#include "exc.hpp"
#include "pmf.hpp"
#include "qbd_analytic_solver.hpp"
#include "qbd_companion_solver.hpp"
#include "qbd_rr_solver.hpp"
#include "qos_function.hpp"
#include "xml_parser.hpp"
#include "xml_utils.hpp"
#include <iostream>
#include <stdio.h>
#include <getopt.h>
#include <tinyxml2.h>

using namespace PrositCore;

bool verbose_flag; //used also in xml_utils.cpp & xml_parser.cpp
bool silent_flag; //used also in xml_utils.cpp
long long  t_start = 0, t_xml_parse_end = 0; //for result time computation, used also in xml_utils.cpp

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

  while ((opt = getopt_long(argc, argv, ":vhs",long_options,0)) != -1) {
    switch (opt) {
    case 'v': 
      verbose_flag = true;
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
  try {
    t_start = PrositAux::my_get_time(); //start parsing
    int opt = opts_parse(argc, argv);
    if(argc-optind != 1)
      EXC_PRINT("one input file requested");

    Parser *p = new Parser(argv[opt]);
    p->parse();
    if(verbose_flag)
      cout << "XML file succesfully parsed" << endl;
    t_xml_parse_end = PrositAux::my_get_time(); //end parsing   
    
    switch(p->get_act()) {
      case ACTIONS::OPTIMISE: 
        //opt_execute(p); 
        EXC_PRINT("Action not currently supported");
        break;
      case ACTIONS::SOLVE:
        solve_execute();
        break;
      default:
        EXC_PRINT("Action not recognised");
    }

    delete p;
  } catch (PrositAux::Exc &e) {
    cerr << "Exception caught" << endl;
    e.what();
  }
}