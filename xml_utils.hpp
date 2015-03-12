#ifndef XML_UTILS_HPP
#define XML_UTILS_HPP

#include "task_descriptor.hpp"
#include "qos_function.hpp"
#include "xml_parser.hpp"
#include <vector>

using namespace PrositCore;

namespace PrositCore {
long long  t_start = 0, t_xml_parse_end = 0; //for result time computation

void solve_core(vector<GenericTaskDescriptor*> & v,
                const vector<double> & probability, 
                const vector<double> & quality, 
                const vector<long long> & time,
                int num);
void solve_execute();
void opt_execute(Parser *p);
///@brief returns the number of task taken into consideration
int get_task_descriptor_vector(vector<GenericTaskDescriptor*> & v);
}
#endif