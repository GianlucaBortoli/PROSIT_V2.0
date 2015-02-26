#include "xml_parser.hpp"
#include "task_descriptor.hpp"
#include <tinyxml2.h>
#include <iostream>
using namespace std;
using namespace tinyxml2;

namespace PrositCore {
ACTIONS Parser::parse() throw (Exc) {
  XMLElement *element = doc->FirstChildElement("optimisation");
  
  if(element) {
    optimisation_parse(element); 
    act = OPTIMISE;
    return act;
  };

  element = doc->FirstChildElement("solve");
  if (element) {
    analysis_parse(element);
    act = SOLVE;
    return act;
  };

  EXC_PRINT("error in xml format");
}

void Parser::optimisation_parse(XMLElement *optElement) throw(Exc) {
  const char *s;
  string opt_method;
  
  if (!(s = optElement->Attribute("method"))) 
    EXC_PRINT("optimisation method undefined");
  
  opt_method = s;
  
  if (opt_method != "infinity") 
    EXC_PRINT("optimisation method unknown");
  
  o = INFINITY_NORM;
  verbose_parse(optElement);
  
  XMLElement *el = optElement->FirstChildElement("epsilon");
  if(el) {
    el->QueryDoubleText(&optim_eps);
  } else {
    optim_eps = 1e-8; // default 
  }

  el = optElement->FirstChildElement("totalBandwidth");
  if(el) {
    el->QueryDoubleText(&total_bandwidth);
  } else {
    total_bandwidth = 1.0; // default
  }

  task_list_parse(optElement);
}

void Parser::verbose_parse(XMLElement * optElement) throw(Exc) {
  const char * s;
  string verbose_flag;
  
  if ((s = optElement->Attribute("verbose"))) {
    verbose_flag = s;
    
    if (verbose_flag == "true"){
      verbose = true;
    } else {
      if (verbose_flag == "false") {
        verbose = false;
      } else {
        EXC_PRINT("bad setting for verbose flag");
      }
    } 
  }
}

void Parser::task_list_parse(XMLElement * optElement) throw(Exc) {
  XMLElement * taskElement = optElement->FirstChildElement("task");
  if (!taskElement) 
    EXC_PRINT("opimisation task section missing");
  
  while (taskElement) {
    Parser::task_parse (taskElement);
    taskElement = taskElement->NextSiblingElement();
  };
}

void Parser::analysis_parse(XMLElement *e) throw (Exc) {
  verbose_parse(e);
  task_list_parse(e);
}

}