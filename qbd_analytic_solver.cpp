#include "qbd_analytic_solver.hpp"

namespace PrositCore {
void AnalyticResourceReservationProbabilitySolver::pre_process(){}

void AnalyticResourceReservationProbabilitySolver::apply_algorithm(){
  PrositAux::pmf * c = new PrositAux::pmf(prob_function.get_size(), prob_function.get_offset());
  double bandwith = server_period * budget;
  unsigned WCET = c->get_max();
  double pi_0 = 1.0; //where the result will be stored

  printf("Bandwith: %f\nWCET: %d\n", bandwith, WCET);

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

  for(unsigned int i = bandwith+1; i < WCET; i++)
    pi_0 -= (i-bandwith)*(c->get(i)-c->get(i-1))/bandwith;

  if(verbose_flag)
    cout << "Analytic computation completed" << endl;
  
  if(pi_0 < 0)
    pi_0 = 0;

  return;
}

void AnalyticResourceReservationProbabilitySolver::post_process(){}
void AnalyticResourceReservationProbabilitySolver::fill_in_probability_map(){}
}