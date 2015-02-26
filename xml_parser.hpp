#include <tinyxml2.h>
#include <iostream>
using namespace std;
using namespace tinyxml2;

namespace PrositCore {
enum ACTIONS {OPTIMISE, SOLVE}
enum OptType {NO_OPT, INFINITY_NORM}

class Parser {
  private:
    XMLDocument *doc;
    ACTIONS a;
    OptType o;
    void optimisation_parse(XMLElement *e) throw(Exc);
    void analysis_parse(XMLElement *e) throw(Exc);
    void task_list_parse(XMLElement *e) throw(Exc);
    void verbose_parse(XMLElement *e) throw (Exc);
    double optim_eps;
    double total_bandwidth;
    bool verbose;
  public:
    //static GenericTaskDescriptor * task_parse(XMLElement *task) throw(Exc);
    //static auto_ptr<pmf> distr_parse(XMLElement *task) throw(Exc);
    //static auto_ptr<QoSFun> qosfun_parse(XMLElement *qosfunElement) throw(Exc);

    Parser(const char *name) throw(Exc):
      o(NO_OPT), optim_eps(1e-6), total_bandwidth(1.0)
      verbose(false){
        doc = new XMLDocument();
        if(doc->LoadFile(fname != XML_NO_ERROR)) {
          delete doc;
          doc = 0;
          EXC_PRINT("Cannot open the specified file");
        }
      ot = NO_OPT;
    }
    ~Parser() {
      if(doc)
        delete doc;
    }

    ACTIONS get_act() const {return act;}
    ACTIONS parse() throw (Exc);

    bool set_verbose(bool verbosed) {
      bool current = verbose;
      verbose = verbosed;
      return current;
    }

    bool get_verbose() const { return verbose;};
    OptType get_optimisation_type() const {
      return ot;
    }

    double get_optimisation_epsilon() const {
      return optim_eps;
    }

    double get_total_bandwidth() const {
      return total_bandwidth;
    }
};
}