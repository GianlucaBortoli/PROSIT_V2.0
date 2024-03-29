#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include "exc.hpp"
#include "task_descriptor.hpp"
#include "qos_function.hpp"
#include <tinyxml2.h>
#include <vector>

using namespace tinyxml2;

namespace PrositCore {
enum ACTIONS {OPTIMISE, SOLVE};
enum OptType {NO_OPT, INFINITY_NORM};

class Parser {
  private:
    XMLDocument *doc;
    ACTIONS a;
    OptType o;
    void optimisation_parse(XMLElement *e) throw(PrositAux::Exc);
    void analysis_parse(XMLElement *e) throw(PrositAux::Exc);
    void task_list_parse(XMLElement *e) throw(PrositAux::Exc);
    double optim_eps;
    double total_bandwidth;
    bool verbose;
    vector<GenericTaskDescriptor*> vect; //vector with all parsed tasks

  public:
    QosFunction * q; //pointer to qos function
    GenericTaskDescriptor * task_parse(XMLElement * taskElement) throw(PrositAux::Exc);
    void qos_parse(XMLElement * taskElement) throw(PrositAux::Exc);

    Parser(const char *name) throw(PrositAux::Exc):
      o(NO_OPT), optim_eps(1e-6), total_bandwidth(1.0),
      verbose(false){
        doc = new XMLDocument();
        if(doc->LoadFile(name) != XML_NO_ERROR) {
          delete doc;
          doc = 0;
          EXC_PRINT("Cannot open the specified file");
        }
      o = NO_OPT;
    }

    ~Parser() {
      if(doc)
        delete doc;
    }

    ACTIONS get_act() const {return a;}
    ACTIONS parse() throw (PrositAux::Exc);

    bool set_verbose(bool verbosed) {
      bool current = verbose;
      verbose = verbosed;
      return current;
    }

    bool get_verbose() const {return verbose;}
    OptType get_optimisation_type() const {return o;}
    double get_optimisation_epsilon() const {return optim_eps;}
    double get_total_bandwidth() const {return total_bandwidth;}
    vector<GenericTaskDescriptor*> get_vector(){return vect;}
};
}
#endif