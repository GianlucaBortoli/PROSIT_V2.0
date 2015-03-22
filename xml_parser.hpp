#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include "exc.hpp"
#include "task_descriptor.hpp"
#include "qos_function.hpp"
#include <tinyxml2.h>

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
    bool if_solve; //true if the initial tag is "solve", false if "optimisation"
    
  public:
    static GenericTaskDescriptor * task_parse(XMLElement *task) throw(PrositAux::Exc);

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
};

}
#endif