#include "xml_parser.hpp"
#include "task_descriptor.hpp"
#include "exc.hpp"
#include <tinyxml2.h>
#include <iostream>
using namespace std;
using namespace tinyxml2;

namespace PrositCore {
ACTIONS Parser::parse() throw (PrositAux::Exc) {
  XMLElement *element = doc->FirstChildElement("optimisation");
  if(element) {
    optimisation_parse(element); 
    a = OPTIMISE;
    return a;
  };

  element = doc->FirstChildElement("solve");
  if (element) {
    analysis_parse(element);
    a = SOLVE;
    return a;
  };

  EXC_PRINT("error in xml format");
}

void Parser::optimisation_parse(XMLElement *optElement) throw(PrositAux::Exc) {
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

void Parser::verbose_parse(XMLElement * optElement) throw(PrositAux::Exc) {
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

void Parser::task_list_parse(XMLElement * optElement) throw(PrositAux::Exc) {
  XMLElement * taskElement = optElement->FirstChildElement("task");
  if (!taskElement) 
    EXC_PRINT("opimisation task section missing");
  
  while (taskElement) {
    Parser::task_parse(taskElement);
    taskElement = taskElement->NextSiblingElement();
  };
}

void Parser::analysis_parse(XMLElement *e) throw(PrositAux::Exc) {
  verbose_parse(e);
  task_list_parse(e);
}

/*
auto_ptr<PrositAux::pmf> Parser::distr_parse(XMLElement * distrElement) throw(PrositAux::Exc) {
  const char * type_name;
  if(!(type_name = distrElement->Attribute("type"))) 
    EXC_PRINT("undefined type for distribution");
  
  DistrFactory::DistrParameters * p = DistrFactory::distr_factory.parse_parameters(type_name, distrElement);
  auto_ptr<PrositAux::pmf> td =  DistrFactory::distr_factory.create_instance(type_name, p);
  delete p;
  return td;
}

auto_ptr<QosFunction> Parser::qosfun_parse(XMLElement * qosfunElement) throw(Exc) {
  const char * type_name;
  if(!(type_name = qosfunElement->Attribute("type"))) 
    EXC_PRINT("undefined type for QoS function");
  
  QoSFactory::QoSFunParameters * p = QoSFactory::qos_fun_factory.parse_parameters(type_name, qosfunElement);
  auto_ptr<QosFunction> td =  QoSFactory::qos_fun_factory.create_instance(type_name, p);
  delete p;
  
  return td;
}

GenericTaskDescriptor * Parser::task_parse(XMLElement * taskElement) throw(Exc) {
  const char * task_name;
  const char * type_name;

  if(!(task_name = taskElement->Attribute("name")))
    EXC_PRINT("name undefined for task");
  if(!(type_name = taskElement->Attribute("type")))
    EXC_PRINT_2("type undefined for task",task_name);
  
  TaskFactory::GenericTaskParameters * p = TaskFactory::task_descriptor_factory.parse_parameters(type_name, task_name, taskElement);
  GenericTaskDescriptor *td =  TaskFactory::task_descriptor_factory.create_instance(type_name, task_name, p);
  delete p;
  
  XMLElement * internal;
  if(internal = taskElement->FirstChildElement("QoSMax")) {
    double qos_min, qos_max;
    internal->QueryDoubleText(&qos_max);

    if (!(internal = taskElement->FirstChildElement("QoSMin")))
      EXC_PRINT_2(" QoSmax defined but QoSMin undefined for task ", task_name);

    internal->QueryDoubleText(&qos_min);
    TaskFactory::task_descriptor_factory.set_task_target_qos_bounds(task_name, qos_min,qos_max);
  }
  return td;
}
*/

}