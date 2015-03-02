///@file Solver for computation of stady state probabilities from XML
#include "pmf.hpp"
#include "auxiliary_func.hpp"
#include "qbd_rr_solver.hpp"
#include "qbd_companion_solver.hpp"
#include "qbd_analytic_solver.hpp"
#include "exc.hpp"
#include "xml_parser.hpp"
#include "xml_utils.hpp"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <getopt.h>
#include <memory>

bool verbose_flag;
bool silent_flag;
bool help_flag;
long long  t_start = 0, t_xml_parse_end = 0;

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
  int i;
  long long t_start = 0, t_end;
  PrositAux::pmf *c = new PrositAux::pmf(Nc, 0); //computation time
  PrositAux::pmf *u = new PrositAux::pmf(Nz, 0); //interarrival time

  int opt;
  try {
    int opt = opts_parse(argc, argv);
    if( argc-optind != 1 )
      EXC_PRINT("one input file requested");

    t_start = my_get_time(); //start parsing time, used to calculate results

    XMLParser::Parser *p = new XMLParser::Parser(argv[opt]);
    p->parse();
    if(verbose_flag)
      cout<<"XML file succesfully parsed"<<endl;
   
    t_xml_parse_end = my_get_time(); //end parting time, used to calculate results
    int res;

    switch(p->get_act()) {
    case  XMLParser::OPTIMISE: 
      //res = opt_execute(p); 
      break;
    case XMLParser::SOLVE:
      //res = solve_execute();
      break;
    default:
      EXC_PRINT("action not currently recognised");
    };

    delete p;
  } catch (PrositAux::Exc &e) {
    cerr << "Exception caught" << endl;
    e.what();
  }
}