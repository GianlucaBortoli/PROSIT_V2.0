#include "xml_utils.hpp"
#include "task_descriptor.hpp"
extern bool verbose_flag; //from xml_solver.cpp
extern bool silent_flag;

namespace PrositCore {
void solve_execute() {
  long long t_solution_start = 0, t_solution_end = 0;
  t_solution_start = PrositAux::my_get_time();// start time
  vector<GenericTaskDescriptor*> v;
  int num = GenericTaskDescriptor::get_task_descriptor_vector(v);

  vector<double> probability(num);
  vector<double> quality(num);
  vector<long long> time(num);

  solve_core(v, probability, quality, time);
  t_solution_end = PrositAux::my_get_time();// end time

  solve_display_results(v, probability, quality, time, true);
  cout<<"=========================================================================================================="<<endl;
  cout<<"=                                              Computation time                                         ="<<endl;
  cout<<"=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n", 25, t_xml_parse_end - t_start);
  printf("\tSolution time: \t\t\t%*llu \n", 25, t_solution_end - t_solution_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25, t_solution_end - t_start);
  cout<<"=========================================================================================================="<<endl;
}

void opt_execute(Parser *p) {
  long long t_optimisation_start = 0, t_optimisation_set_up = 0, t_optimisation_end = 0;
  t_optimisation_set_up = PrositAux::my_get_time();// start time
  vector<GenericTaskDescriptor*> v;
  int num = GenericTaskDescriptor::get_task_descriptor_vector(v);

  vector<double> probability(num); //vectors for displaying results
  vector<double> quality(num);
  vector<long long> time(num);

  if(verbose_flag)
    cout << "Number of tasks parsed: "<< num << endl;
    
  /*  
  InfinityNormBudgetOptimiser Opt(v, p->get_optimisation_epsilon(),p->get_total_bandwidth());
  Opt.init_target_bounds();

  if(silent_flag)
    Opt.set_verbose(verbose_flag);
  else
    Opt.set_verbose(p->get_verbose());
    
  t_optimisation_start=PrositAux::my_get_time();
    
  Opt.optimise();
    
  t_optimisation_end=PrositAux::my_get_time();
  
  if (Opt.get_state() != GenericBudgetOptimiser::OK) {
    cerr<<"Optimisation failed"<<endl;
    return 0;
  };
  */

  solve_core(v, probability, quality, time);
  
  cout<<"Optimisation succeeded."<<endl;
  solve_display_results(v, probability, quality, time, false);

  cout<<"=========================================================================================================="<<endl;
  cout<<"=                                              Computation time                                         ="<<endl;
  cout<<"=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n", 25, t_xml_parse_end - t_start);
  printf("\tOptimisation setup: \t\t%*llu \n", 25, t_optimisation_start - t_optimisation_set_up);
  printf("\tOptimisation time: \t\t%*llu \n", 25, t_optimisation_end - t_optimisation_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25, t_optimisation_end - t_start);
  cout<<"=========================================================================================================="<<endl;
}

int solve_core(vector<GenericTaskDescriptor*> &v, 
                      vector<double> &probability, 
                      vector<double> &quality, 
                      vector<long long> &time) {
  long long t_solution_start_i = 0, t_solution_end_i = 0;

  int num = GenericTaskDescriptor::get_task_descriptor_vector(v);
  if(verbose_flag)
    cout << "Number of tasks parsed: " << num << endl;
  
  int i = 0;
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin() ; (it != v.end()); ++it) {
    t_solution_start_i = PrositAux::my_get_time();
    probability[i] = (*it)->compute_probability();
    t_solution_end_i = PrositAux::my_get_time();
    quality[i] = QosFunction::eval(probability[i]);
    time[i] = t_solution_end_i - t_solution_start_i;
    i++;
  };
  return v.size();
};

int solve_display_results(vector<GenericTaskDescriptor*> &v, 
                                 const vector<double> &probability, 
                                 const vector<double> &quality, 
                                 const vector<long long> &time, 
                                 bool show_time) {
  cout<<"Analysis results."<<endl;
  cout<<"================================================================================================================================="<<endl;
  cout<<"=                                                         Results                                                               ="<<endl;
  cout<<"================================================================================================================================="<<endl;
  double Btot=0;
  int i = 0;
  double inf_norm=1e38;
  if(show_time)
    printf("%20s%20s%20s%20s%20s%20s\n", "Name","Budget","Bandwidth","Probability","Quality","Time");
  else
    printf("%20s%20s%20s%20s%20s\n", "Name","Budget","Bandwidth","Probability","Quality");
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin() ; (it != v.end()); ++it) {
    printf("%20s%20d%20f%20f%20f", (*it)->get_name().c_str(), (*it)->get_budget(),double((*it)->get_budget())/double((*it)->get_server_period()),probability[i], quality[i]);
    if(show_time)
      printf("%*llu\n",20,time[i]);
    else
      printf("\n");
    Btot += double((*it)->get_budget())/double((*it)->get_server_period());
    inf_norm=min<double>(quality[i],inf_norm);    
    i++;
  }
  cout<<"================================================================================================================================="<<endl;
  printf("\tTotal bandwidth: \t\t%25f\n",Btot);
  printf("\tInfinity norm value: \t\t%25f\n",inf_norm);
  
  return v.size();
}

}