#include "xml_utils.hpp"
#include "task_descriptor.hpp"

namespace PrositCore {
static int solve_core(vector<GenericTaskDescriptor*> &v) {
  vector<double> probability, quality;
  vector<long long> time;
  long long t_solution_start_i = 0, t_solution_end_i = 0;

  int num = GenericTaskDescriptor::get_task_descriptor_vector(v);
  if(verbose_flag)
    cout << "Number of tasks parsed: " << num << endl;
  
  int i = 0;
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin() ; (it != v.end()); ++it) {
    ProbPeriodicTaskDescriptor * p = dynamic_cast<ProbPeriodicTaskDescriptor *>(*it);
    if(!p) {
      cerr << "Sorry, analysis works only for probabilistic periodic tasks" << endl;
      return 0;
    }
    t_solution_start_i = my_get_time();
    probability[i] = (*it).compute_probability();
    t_solution_end_i = my_get_time();
    quality[i] = p->QoS_from_prob(probability[i]);
    time[i] = t_solution_end_i - t_solution_start_i;
    i++;
  };
  return v.size();
};

static int solve_display_results(vector<GenericTaskDescriptor*> &v, 
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

static int solve_execute() {
  /*long long t_solution_start=0, t_solution_end=0;
  t_solution_start = my_get_time();
  vector<GenericTaskDescriptor*> v;
  int num = TaskFactory::task_descriptor_factory.get_task_descriptor_vector(v);

  vector<double> probability(num);
  vector<double> quality(num);
  vector<long long> time(num);


  solve_core(v,probability, quality, time);
  t_solution_end = my_get_time();
  solve_display_results(v, probability, quality, time,true);
  cout<<"=========================================================================================================="<<endl;
  cout<<"=                                              Computation time                                         ="<<endl;
  cout<<"=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n",25,t_xml_parse_end  - t_start);
  printf("\tSolution time: \t\t\t%*llu \n",25,t_solution_end-t_solution_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25,t_solution_end-t_start);
  cout<<"=========================================================================================================="<<endl;
  return 1;*/
}

static int opt_execute(Parser *p) {
  /*long long t_optimisation_start=0, t_optimisation_set_up = 0, t_optimisation_end=0;

  t_optimisation_set_up = my_get_time();
  vector<GenericTaskDescriptor*> v;
  int num = TaskFactory::task_descriptor_factory.get_task_descriptor_vector(v);
  if(verbose_flag)
    cout<<"Number of tasks parsed: "<<num<<endl;
    
  InfinityNormBudgetOptimiser Opt(v, p->get_optimisation_epsilon(),p->get_total_bandwidth());
  Opt.init_target_bounds();
  if(verbose_set)
    Opt.set_verbose(verbose_flag);
  else
    Opt.set_verbose(p->get_verbose());
    
  t_optimisation_start=my_get_time();
    
  Opt.optimise();
    
  t_optimisation_end=my_get_time();
  vector<double> probability(num);
  vector<double> quality(num);
  vector<long long> time(num);
  if (Opt.get_state() != GenericBudgetOptimiser::OK) {
    cerr<<"Optimisation failed"<<endl;
    return 0;
  };
  
  solve_core(v, probability, quality, time);
  
  cout<<"Optimisation succeeded."<<endl;
  solve_display_results(v, probability, quality, time, false);

  cout<<"=========================================================================================================="<<endl;
  cout<<"=                                              Computation time                                         ="<<endl;
  cout<<"=========================================================================================================="<<endl;
  printf("\tParsing time: \t\t\t%*llu \n",25,t_xml_parse_end  - t_start);
  printf("\tOptimisation setup: \t\t%*llu \n",25,t_optimisation_start-t_optimisation_set_up);
  printf("\tOptimisation time: \t\t%*llu \n",25,t_optimisation_end-t_optimisation_start);
  printf("\tTotal time: \t\t\t%*llu \n", 25,t_optimisation_end-t_start);
  cout<<"=========================================================================================================="<<endl;
    
  return 1;*/
}
}