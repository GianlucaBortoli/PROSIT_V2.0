#include "xml_parser.hpp"

#define Nc 2500000 //used for interarrival and computation time pmf
#define Nz 1000000

using namespace tinyxml2;

namespace PrositCore {
ACTIONS Parser::parse() throw (PrositAux::Exc) {
  XMLElement *element = doc->FirstChildElement("optimisation");
  if(element) {
    optimisation_parse(element); 
    a = OPTIMISE;
    return a;
  }

  element = doc->FirstChildElement("solve");
  if(element) {
    analysis_parse(element);
    a = SOLVE;
    return a;
  }
  EXC_PRINT("Error in xml format");
}

//Called when "optimization" tag is used
void Parser::optimisation_parse(XMLElement *optElement) throw(PrositAux::Exc) {
  const char *s;
  string opt_method;
  
  if (!(s = optElement->Attribute("method"))) 
    EXC_PRINT("optimisation method undefined");
  
  opt_method = s;
  
  if (opt_method != "infinity") 
    EXC_PRINT("optimisation method unknown");
  
  o = INFINITY_NORM;
  
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

//Called when "solve" tag is used
void Parser::analysis_parse(XMLElement *e) throw(PrositAux::Exc) {
  task_list_parse(e);
}

//Parses the whole list of all tasks in the XML file provided by the user
void Parser::task_list_parse(XMLElement * optElement) throw(PrositAux::Exc) {
  XMLElement * taskElement = optElement->FirstChildElement("task");
  if (!taskElement) 
    EXC_PRINT("Opimisation task section missing");
  
  while (taskElement) {
    Parser::task_parse(taskElement);
    taskElement = taskElement->NextSiblingElement();
  }
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
*/

//Parses a single task
GenericTaskDescriptor * Parser::task_parse(XMLElement * taskElement) throw(PrositAux::Exc) {
  // See file /test/xml_example.xml for an example of the possible parameters and the
  // structure for the xml file used to call the xml_solver
  PrositCore::GenericTaskDescriptor *td;

  const char * type;
  const char * schedule;
  const char * name;
  unsigned int budget;
  unsigned int period;

  if(!(type = taskElement->Attribute("type")))
    EXC_PRINT_2("Undefined type for task ", name);
  if(!(schedule = taskElement->Attribute("schedule")))
    EXC_PRINT_2("Undefined schedule for task ", name);
  if(!(name = taskElement->Attribute("name")))
    EXC_PRINT("Undefined name for task");

  XMLElement * internal; //used to parse the first childs parameters

  if((type = "periodic")){ //periodic task
    if((schedule = "RR")){ //RR -> resource reservation
      internal = taskElement->FirstChildElement("serverBudget"); 
      internal->QueryUnsignedText(&budget); //set server budget
      internal = taskElement->FirstChildElement("serverPeriod");
      internal->QueryUnsignedText(&period); //set server period

      PrositAux::pmf *c = new PrositAux::pmf(Nc, 0); //computation
      PrositAux::pmf *i = new PrositAux::pmf(Nz, 0); //interarrival

      //TODO: initialize comp_time & interr_time

      std::unique_ptr<PrositAux::pmf> comp_time(c);
      std::unique_ptr<PrositAux::pmf> interr_time(i);

      if(!(td = dynamic_cast<ResourceReservationTaskDescriptor *>(td)))
        EXC_PRINT_2("Impossible to cast task GenericTaskDescriptor to ResourceReservationTaskDescriptor for task ", 
                td->get_name());

      td = new ResourceReservationTaskDescriptor(name, comp_time, interr_time, budget, period);

    } else { //FP -> fixed priority
      EXC_PRINT("Fixed priority tasks not implemented yet");
    }
  }
  
  //Parsing Qos Function
  if((internal = taskElement->FirstChildElement("QoSMax"))) {
    double qos_min, qos_max;
    internal->QueryDoubleText(&qos_max);

    if (!(internal = taskElement->FirstChildElement("QoSMin")))
      EXC_PRINT_2("QoSmax defined but QoSMin undefined for task ", name);

    internal->QueryDoubleText(&qos_min);
    PrositCore::QosFunction q(0, qos_min, qos_max, 0);
  }
  return td;
}

}