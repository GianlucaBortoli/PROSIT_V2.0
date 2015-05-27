#include "xml_parser.hpp"
#include "task_descriptor.hpp"
#include <memory>

using namespace tinyxml2;
using namespace std;

extern bool verbose_flag; //from xml_solver.cpp
extern bool if_solve; //from xml_solver.cpp

namespace PrositCore {
ACTIONS Parser::parse() throw (PrositAux::Exc) {
  XMLElement *element = doc->FirstChildElement("optimisation");
  if(element) {
    if(verbose_flag)
      cout << "Optimisation parsing chosen" << endl;
    
    if_solve = false;
    optimisation_parse(element); 
    a = OPTIMISE;
    return a;
  }

  element = doc->FirstChildElement("solve");
  if(element) {
    if(verbose_flag)
      cout << "Solve parsing chosen" << endl;
    
    if_solve = true;
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
    EXC_PRINT("Task section missing");
  
  qos_parse(taskElement); //parse & create qos function from first task
  while(taskElement) { //goes through every task tag defined in xml file
    vect.push_back(task_parse(taskElement)); //initialise vector with all tasks
    taskElement = taskElement->NextSiblingElement();
  }
}

//Parses a single task
GenericTaskDescriptor * Parser::task_parse(XMLElement * taskElement) throw(PrositAux::Exc) {
  // See file /test/xml_example.xml for an example of the possible parameters and the
  // structure for the xml file used to call the xml_solver
  GenericTaskDescriptor *td = NULL;

  const char * type;
  const char * schedule;
  const char * name;
  const char * algorithm;
  unsigned int budget;
  unsigned int period;
  unsigned int max_deadline;
  unsigned int delta;
  int step;

  if(!(name = taskElement->Attribute("name")))
    EXC_PRINT("Undefined name for task");
  if(!(type = taskElement->Attribute("type")))
    EXC_PRINT_2("Undefined type for task ", name);
  if(!(schedule = taskElement->Attribute("schedule")))
    EXC_PRINT_2("Undefined schedule for task", name);
  if(!(algorithm = taskElement->Attribute("algorithm")))
    EXC_PRINT_2("Undefined algorithm selected", algorithm);

  XMLElement * internal; //used to parse the first childs parameters of the task tag

  // Parsing single task parameters
  if(strcmp(type, "periodic") == 0 || strcmp(type, "aperiodic") == 0){
    if((schedule = "RR")){ //RR -> resource reservation
      if(verbose_flag)
        cout << type << " RR task defined in XML file" << endl;
      
      internal = taskElement->FirstChildElement("serverBudget"); 
      internal->QueryUnsignedText(&budget); //set server budget
      internal = taskElement->FirstChildElement("serverPeriod");
      internal->QueryUnsignedText(&period); //set server period
      internal = taskElement->FirstChildElement("Delta");
      internal->QueryUnsignedText(&delta); //set granularity
      internal = taskElement->FirstChildElement("maxDeadline");
      internal->QueryUnsignedText(&max_deadline); //set max deadline
      taskElement->FirstChildElement("pmfComputation")->FirstChildElement("step")->QueryIntText(&step);

      if(step == 0)
        EXC_PRINT("Step value for pfm missing");
      if(max_deadline <= 0)
        EXC_PRINT("Maximum deadline not properly set");

      // Pmf creation (loaded directly from file or from given parameters)
      XMLElement *fileComputation, *fileInterarrival;
      std::unique_ptr<PrositAux::beta> comp_time, interr_time;
      const char * fromFile;

      if((fileComputation = taskElement->FirstChildElement("pmfComputation")->FirstChildElement("file"))) {
        //if pmf for computation times is loaded from file
        comp_time = std::unique_ptr<PrositAux::beta>(new PrositAux::beta());
        fromFile = fileComputation->GetText();
        comp_time->load(fromFile);

        if(verbose_flag)  
          printf("Pmf for computation time successfully retrieved from file %s\n", fromFile);
      } else {
        //if created from parameters parsed from xml
        comp_time = std::unique_ptr<PrositAux::beta>(new PrositAux::beta());
        comp_time = comp_time->create_beta_computation(taskElement);

        if(verbose_flag)  
          cout << "Pmf for computation time successfully retrieved from xml" << endl;
      } 

      if((fileInterarrival = taskElement->FirstChildElement("pmfInterarrival")->FirstChildElement("file"))) {
        //if pmf loaded for interarrival times is loaded from file
        interr_time = std::unique_ptr<PrositAux::beta>(new PrositAux::beta());
        fromFile = fileInterarrival->GetText();
        interr_time->load(fromFile);

        if(verbose_flag)  
          printf("Pmf for interarrival time successfully retrieved from file %s\n", fromFile);
      } else {
        //if created from parameters parsed from xml
        interr_time = std::unique_ptr<PrositAux::beta>(new PrositAux::beta());
        interr_time = interr_time->create_beta_interarrival(taskElement);

        cout << "Pmf for interarrival time successfully retrieved from xml" << endl;
      }

      //Dump distribution, if tag is present
      XMLElement * dumpSection;
      const char * fileName;
      if((dumpSection = taskElement->FirstChildElement("pmfComputation")->FirstChildElement("dump"))) {
        fileName = dumpSection->GetText();
        comp_time->dump(fileName);
      }
      if((dumpSection = taskElement->FirstChildElement("pmfInterarrival")->FirstChildElement("dump"))) {
        fileName = dumpSection->GetText();
        interr_time->dump(fileName);
      }

      if(verbose_flag) 
        cout << "Pmf created...now creating RR task descriptor object" << endl;

      // RR task descriptor creation
      td = new ResourceReservationTaskDescriptor(name, 
                                                 std::move(comp_time), 
                                                 std::move(interr_time), 
                                                 budget, 
                                                 period,
                                                 algorithm);
      td->set_deadline_step(period);
      td->set_verbose_flag(verbose_flag ? true : false);
      td->set_delta(delta);
      td->set_step(step);
      for (unsigned int i = 0; i <= max_deadline; i++) {
        td->insert_deadline(i);
      }
      
      if(verbose_flag) 
        cout << "RR task descriptor object created successfully!" << endl;

    } else { //FP -> fixed priority
      EXC_PRINT("Fixed priority tasks not implemented yet");
    }
  } else {
    EXC_PRINT("Task type not recognized");
  }
  
  return td;
}

// Parsing & creating QoS Function
void Parser::qos_parse(XMLElement * taskElement) throw(PrositAux::Exc){
  XMLElement * qosElement;
  XMLElement * internal; 
  // if optimisation -> mandatory
  // if solve        -> optional
  if((qosElement = taskElement->FirstChildElement("qosfun"))) {
    if(qosElement != NULL){//if exists all parameters have to be set, both in solve & optimisation cases 
      const char * type;
      double scale, qos_min, qos_max, offset;

      if(!(type = qosElement->Attribute("type"))) //set type of qos function
        EXC_PRINT("Undefined type for QoS function");
      if (!(internal = qosElement->FirstChildElement("pmin")))
        EXC_PRINT("Minimum not specified");
      internal->QueryDoubleText(&qos_min); //set min
      if (!(internal = qosElement->FirstChildElement("pmax")))
        EXC_PRINT("Maximum not specified");
      internal->QueryDoubleText(&qos_max); //set max
      if (!(internal = qosElement->FirstChildElement("scale")))
        EXC_PRINT("Scaling factor not specified");
      internal->QueryDoubleText(&scale); //set scaling factor
      if ((internal = qosElement->FirstChildElement("offset"))) {
        internal->QueryDoubleText(&offset); //set offset
      } else {
        offset = 0.95; //the offset value in the xml is optional; Default value set
        cout << "Offset value not specified, default is 0.95" << endl;
      }

      if(verbose_flag) 
        printf("Qos function parameters: %f %f %f %f\n", scale, qos_min, qos_max, offset);

      q = new QosFunction(scale, qos_min, qos_max, offset);
    } else if((qosElement == NULL) && (!if_solve)){//if not exists + optimisation tag
      EXC_PRINT("QoS function parameters are mandatory in the optimisation case!");
    } else if((qosElement == NULL) && if_solve){//if not exists + solve tag
      cout << "QoS function element not present in XML for \"solve\" tag" << endl;
    }
  }
}

}