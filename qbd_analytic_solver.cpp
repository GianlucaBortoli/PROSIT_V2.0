#include "qbd_analytic_solver.hpp"

namespace PrositCore {
void AnalyticResourceReservationProbabilitySolver::pre_process(){}

void AnalyticResourceReservationProbabilitySolver::apply_algorithm(){
  unsigned bandwith = server_period * budget;
  unsigned WCET = prob_function->get_max();

  if(verbose_flag)
    cout << "Bandwith: " << bandwith << "\nWCET: " << WCET << endl;
    //printf("Bandwith: %d\nWCET: %d\n", bandwith, WCET);
  if(verbose_flag)
    cout << "Prepearing analytic form" << endl;
  if(bandwith > WCET){
    cout << "Bandwith grater than the worst case requirements" << endl;
    pi_0 = 1.0;
    return;
  }
  if(abs(bandwith < 1e-10)){
    if(verbose_flag){
      cout << "Bandwith too small" << endl;
      pi_0 = 0.0;
      return;
    }
  }

  for(unsigned i = bandwith+1; i < WCET; i++)
    pi_0 -= (i-bandwith)*(prob_function->get(i)-prob_function->get(i-1))/bandwith;

  if(verbose_flag)
    cout << "Analytic computation completed" << endl;
  
  if(pi_0 < 0)
    pi_0 = 0;

  return;
}

void AnalyticResourceReservationProbabilitySolver::post_process(){}

void AnalyticResourceReservationProbabilitySolver::fill_in_probability_map(){
  DeadlineProbabilityMap *pm = task_descriptor->get_probabilistic_deadlines();
  DeadlineProbabilityMapIter pmi;

  for(pmi = pm->begin(); pmi != pm->end(); pmi++){
    (*pmi).second = pi_0;
  }
}
}