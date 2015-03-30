#include "xml_utils.hpp"
#include "qbd_rr_solver.hpp"
#include "qbd_companion_solver.hpp"
#include "qbd_analytic_solver.hpp"
#include "auxiliary_func.hpp"

extern bool verbose_flag; //from xml_solver.cpp
extern bool silent_flag;
extern long long t_start;
extern long long t_xml_parse_end;

namespace PrositCore {
static double eps = 1e-4;
static int max_iteration = 100;

void solve_core(vector<GenericTaskDescriptor*> &v, 
                vector<double> &probability, 
                vector<double> &quality, 
                vector<long long> &time,
                int num,
                Parser *p) {
  long long t_solution_start_i = 0, t_solution_end_i = 0;
  if(verbose_flag)
    cout << "Number of tasks parsed: " << num << endl;
  
  int i = 0;
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin(); it != v.end(); it++) {
    t_solution_start_i = PrositAux::my_get_time();

    /////////////////////////////////////////////////////
    //  Setting sovler & assigning relative probability
    /////////////////////////////////////////////////////
    if(strcmp((*it)->algorithm, "analytic") != 0) {
      if(verbose_flag)
        cout << "Analytic solver chosen" << endl;

      ResourceReservationTaskDescriptor * t;
      t = dynamic_cast<ResourceReservationTaskDescriptor *>((*it));

      PrositAux::pmf * p = t->get_comp_time_distr().get(); //the pmf of the task 
      AnalyticResourceReservationProbabilitySolver *tmp = 
        new AnalyticResourceReservationProbabilitySolver(
          *p, t->get_ts(), t->get_q());
      std::unique_ptr<ResourceReservationProbabilitySolver> ps(tmp);
      (*it)->set_solver(ps.get());
      probability[i] = (*it)->get_probability(1);
    } else if(strcmp((*it)->algorithm, "companion") != 0) {
      if(verbose_flag)
        cout << "Companion solver chosen" << endl;

      CompanionResourceReservationProbabilitySolver *tmp = 
        new CompanionResourceReservationProbabilitySolver(
          (*it)->get_deadline_step(), eps);
      std::unique_ptr<ResourceReservationProbabilitySolver> ps(tmp);
      (*it)->set_solver(ps.get());
      probability[i] = (*it)->get_probability(1); 
    } else if(strcmp((*it)->algorithm, "cyclic") != 0) {
      if(verbose_flag)
        cout << "Cyclic solver chosen" << endl;

      QBDResourceReservationProbabilitySolver *tmp = 
        new CRResourceReservationProbabilitySolver(
          (*it)->get_deadline_step(), false, max_iteration);
      std::unique_ptr<QBDResourceReservationProbabilitySolver> ps(tmp);
      (*it)->set_solver(ps.get());
      probability[i] = (*it)->get_probability(1);
    } else if(strcmp((*it)->algorithm, "latouche") != 0) {
      if(verbose_flag)
        cout << "Latouche solver chosen" << endl;

      QBDResourceReservationProbabilitySolver *tmp = 
        new LatoucheResourceReservationProbabilitySolver(
          (*it)->get_deadline_step(), eps, max_iteration);
      std::unique_ptr<QBDResourceReservationProbabilitySolver> ps(tmp);
      (*it)->set_solver(ps.get());
      probability[i] = (*it)->get_probability(1);
    } else {
      EXC_PRINT("Solver not recognized!");
    }

    t_solution_end_i = PrositAux::my_get_time();
    quality[i] = p->q->eval(probability[i]);
    time[i] = t_solution_end_i - t_solution_start_i;
    i++;
  }
}
  
void solve_execute(Parser *p) {
  long long t_solution_start = 0, t_solution_end = 0;
  t_solution_start = PrositAux::my_get_time();// start time

  vector<GenericTaskDescriptor*> v = p->get_vector();//holds every task
  int num = v.size();

  //TODO: check when task is added to the task vector. Segnemtation fault here!
  cout << "-->" << v[0]->algorithm << endl;

  vector<double> probability(num);
  vector<double> quality(num);
  vector<long long> time(num);

  solve_core(v, probability, quality, time, num, p);
  t_solution_end = PrositAux::my_get_time();// end time

  cout << "Analysis results:" << endl;
  cout << "=================================================================================================================================" << endl;
  cout << "=                                                         Results                                                               =" << endl;
  cout << "=================================================================================================================================" << endl;
  int i = 0;
  double Btot_final = 0.0, inf_norm_final = 0.0;
  vector<GenericTaskDescriptor*>::iterator it;
  for (vector<GenericTaskDescriptor*>::iterator it = v.begin(); it != v.end(); it++) {
    (*it)->display((*it), probability, quality, time, true, i);
    Btot_final += (*it)->Btot;
    inf_norm_final = min<double>(quality[i], (*it)->inf_norm);
    i++;
  }
  cout << "=================================================================================================================================" << endl;
  printf("\tTotal bandwidth: \t\t%25f\n", Btot_final);
  printf("\tInfinity norm value: \t\t%25f\n", inf_norm_final);
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

}