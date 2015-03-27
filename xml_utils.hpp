#ifndef XML_UTILS_HPP
#define XML_UTILS_HPP

#include "task_descriptor.hpp"
#include "qos_function.hpp"
#include "xml_parser.hpp"
#include <vector>

using namespace PrositCore;

namespace PrositCore {
void solve_core(vector<GenericTaskDescriptor*> & v,
                vector<double> & probability, 
                vector<double> & quality, 
                vector<long long> & time,
                int num,
                Parser *p);
void solve_execute(Parser *p);
void opt_execute(Parser *p);
}
#endif