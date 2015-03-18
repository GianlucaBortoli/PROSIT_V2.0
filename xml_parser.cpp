#include "xml_parser.hpp"
#include <memory>

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

//Parses a single task
GenericTaskDescriptor * Parser::task_parse(XMLElement * taskElement) throw(PrositAux::Exc) {
  // See file /test/xml_example.xml for an example of the possible parameters and the
  // structure for the xml file used to call the xml_solver
  PrositCore::GenericTaskDescriptor *td = NULL;

  const char * type;
  const char * schedule;
  const char * name;
  unsigned int budget;
  unsigned int period;

  if(!(name = taskElement->Attribute("name")))
    EXC_PRINT("Undefined name for task");
  if(!(type = taskElement->Attribute("type")))
    EXC_PRINT_2("Undefined type for task ", name);
  if(!(schedule = taskElement->Attribute("schedule")))
    EXC_PRINT_2("Undefined schedule for task", name);

  XMLElement * internal; //used to parse the first childs parameters

  /////////////////////////////////
  //     Parsing single task
  ////////////////////////////////
  if((type = "periodic")){
    if((schedule = "RR")){ //RR -> resource reservation
      internal = taskElement->FirstChildElement("serverBudget"); 
      internal->QueryUnsignedText(&budget); //set server budget
      internal = taskElement->FirstChildElement("serverPeriod");
      internal->QueryUnsignedText(&period); //set server period

      /////////////////////////////////////////////////////////////////////////////
      //   Beta distribution for computation & interarrival time initialisation
      /////////////////////////////////////////////////////////////////////////////
      std::unique_ptr<PrositAux::beta> comp_time(new PrositAux::beta());
      comp_time->create_beta_computation(taskElement);
      std::unique_ptr<PrositAux::beta> interr_time(new PrositAux::beta());
      interr_time->create_beta_interarrival(taskElement);

      // RR task descriptor creation
      if(!(td = dynamic_cast<ResourceReservationTaskDescriptor *>(td)))
        EXC_PRINT_2("Impossible to cast GenericTaskDescriptor to ResourceReservationTaskDescriptor for task ", 
                td->get_name());

      td = new ResourceReservationTaskDescriptor(name, 
                                                 std::move(comp_time), 
                                                 std::move(interr_time), 
                                                 budget, 
                                                 period);

    } else { //FP -> fixed priority
      EXC_PRINT("Fixed priority tasks not implemented yet");
    }
  } else { //Aperiodic
    EXC_PRINT("Aperiodic tasks not implemented yet");
  }
  /////////////////////////////////
  //     Parsing QoS Function
  ////////////////////////////////
  if((internal = taskElement->FirstChildElement("qosfun"))) {
    const char * type;
    double scale, qos_min, qos_max;

    if(!(type = taskElement->Attribute("type"))) //set type of qos function
      EXC_PRINT("Undefined type for QoS function");

    if (!(internal = taskElement->FirstChildElement("pmin")))
      EXC_PRINT("Minimum not specified");
    internal->QueryDoubleText(&qos_min); //set min

    if (!(internal = taskElement->FirstChildElement("pmax")))
      EXC_PRINT("Maximum not specified");
    internal->QueryDoubleText(&qos_max); //set max

    if (!(internal = taskElement->FirstChildElement("scale")))
      EXC_PRINT("Scaling factor not specified");
    internal->QueryDoubleText(&scale); //set scale factor

    PrositCore::QosFunction q(scale, qos_min, qos_max, 0);
  }
  return td;
}

}