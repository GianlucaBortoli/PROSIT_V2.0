#include "xml_utils.hpp"

extern bool verbose_flag; //from xml_solver.cpp
extern bool silent_flag;

namespace PrositCore {
void solve_core(vector<GenericTaskDescriptor*> &v, 
                      vector<double> &probability, 
                      vector<double> &quality, 
                      vector<long long> &time,
                      int num) {
  long long t_solution_start_i = 0, t_solution_end_i = 0;
  if(verbose_flag)
    cout << "Number of tasks parsed: " << num << endl;
  
  int i = 0;
  PrositCore::QosFunction *q = new PrositCore::QosFunction(0, 0, 1, 0.95); //used to calculate qos quality function
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin() ; (it != v.end()); ++it) {
    t_solution_start_i = PrositAux::my_get_time();

    // Cycle foreach multiples of the server_period until the maximum probability is reached
    // in order to set the probability vector
    for(unsigned int k = 0; k < probability.size(); k++) {
      probability[k] = double((*it)->get_deadline_step() * k);
    }

    t_solution_end_i = PrositAux::my_get_time();
    quality[i] = q->eval(probability[i]);
    time[i] = t_solution_end_i - t_solution_start_i;
    i++;
  }
  delete q;
}
  
void solve_execute() {
  long long t_solution_start = 0, t_solution_end = 0;
  t_solution_start = PrositAux::my_get_time();// start time
  vector<GenericTaskDescriptor*> v; //THIS VECTOR IS STILL TO BE INITIALIZED!
  int num = get_task_descriptor_vector(v);

  vector<double> probability(num);
  vector<double> quality(num);
  vector<long long> time(num);

  solve_core(v, probability, quality, time, num);
  t_solution_end = PrositAux::my_get_time();// end time

  cout << "Analysis results:" << endl;
  cout << "=================================================================================================================================" << endl;
  cout << "=                                                         Results                                                               =" << endl;
  cout << "=================================================================================================================================" << endl;
  int i = 0;
  double Btot_final = 0.0, inf_norm_final = 0.0;
  vector<GenericTaskDescriptor*>::iterator it;
  for (it = v.begin(); it != v.end(); it++) {
    (*it)->display((*it), probability, quality, time, true, i);
    Btot_final += (*it)->Btot;
    inf_norm_final = min<double>(quality[i], (*it)->inf_norm);
    i++;
  }
  cout << "=================================================================================================================================" << endl;
  printf("\tTotal bandwidth: \t\t%25f\n", Btot_final);
  printf("\tInfinity norm value: \t\t%25f\n", inf_norm_final);

  //solve_display_results(v, probability, quality, time, true);
  cout << "=========================================================================================================="<<endl;
  cout << "=                                              Computation time                                         ="<<endl;
  cout << "=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n", 25, t_xml_parse_end - t_start);
  printf("\tSolution time: \t\t\t%*llu \n", 25, t_solution_end - t_solution_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25, t_solution_end - t_start);
  cout << "=========================================================================================================="<<endl;
}

void opt_execute(Parser *p) {
  /*
  long long t_optimisation_start = 0, t_optimisation_set_up = 0, t_optimisation_end = 0;
  t_optimisation_set_up = PrositAux::my_get_time();// optimization setup time
  vector<GenericTaskDescriptor*> v; //THIS VECTOR IS STILL TO BE INITIALIZED!
  int num = get_task_descriptor_vector(v);

  vector<double> probability(num); //vectors for displaying results
  vector<double> quality(num);
  vector<long long> time(num);

  if(verbose_flag)
    cout << "Number of tasks parsed: "<< num << endl;
      
  InfinityNormBudgetOptimiser Opt(v, p->get_optimisation_epsilon(),p->get_total_bandwidth());
  Opt.init_target_bounds();

  if(silent_flag)
    Opt.set_verbose(verbose_flag);
  else
    Opt.set_verbose(p->get_verbose());
    
  t_optimisation_start=PrositAux::my_get_time();// optimization start time
    
  Opt.optimise();
    
  t_optimisation_end=PrositAux::my_get_time();// optimization end time
  
  if (Opt.get_state() != GenericBudgetOptimiser::OK) {
    cerr<<"Optimisation failed"<<endl;
    return 0;
  };
  

  solve_core(v, probability, quality, time, num);
  
  cout<<"Optimisation succeeded."<<endl;
  solve_display_results(v, probability, quality, time, false);
  cout<<"=========================================================================================================="<<endl;
  cout<<"=                                              Computation time                                          ="<<endl;
  cout<<"=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n", 25, t_xml_parse_end - t_start);
  printf("\tOptimisation setup: \t\t%*llu \n", 25, t_optimisation_start - t_optimisation_set_up);
  printf("\tOptimisation time: \t\t%*llu \n", 25, t_optimisation_end - t_optimisation_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25, t_optimisation_end - t_start);
  cout<<"=========================================================================================================="<<endl;
  */
}

int get_task_descriptor_vector(vector<GenericTaskDescriptor*> & v) {
  int num = 0;
  vector<GenericTaskDescriptor*>::iterator it;
  for(it = v.begin(); it !=v.end(); it++)
    num++;

  return num;
}

}