#ifndef XML_UTILS_HPP
#define XML_UTILS_HPP

#include "task_descriptor.hpp"
#include "xml_parser.hpp"
#include <vector>

namespace PrositCore {
long long  t_start = 0, t_xml_parse_end = 0; //for result time computation

int solve_core(vector<GenericTaskDescriptor*> & v,
                      const vector<double> & probability, 
                      const vector<double> & quality, 
                      const vector<long long> & time);
int solve_display_results(vector<GenericTaskDescriptor*> & v, 
                                 const vector<double> & probability, 
                                 const vector<double> & quality, 
                                 const vector<long long> & time, 
                                 bool show_time);
void solve_execute();
void opt_execute(Parser *p);
///@brief returns the number of task taken into consideration
int get_task_descriptor_vector(vector<GenericTaskDescriptor*> & v);
}
#endif