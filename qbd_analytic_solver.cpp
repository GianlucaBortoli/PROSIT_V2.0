#include "qbd_analytic_solver.hpp"

namespace PrositCore {
void AnalyticResourceReservationProbabilitySolver::pre_process(){}

void AnalyticResourceReservationProbabilitySolver::apply_algorithm(){
  PrositAux::cdf c(prob_function.get_size(), prob_function.get_offset());
  pmf2cdf(prob_function, c);
  double a0p = c.get(server_period*budget-1);
  unsigned WCET = c.get_max();
  double pi_0 = 1.0; // where the result will be stored

  if(verbose_flag)
    cout << "Prepearing analytic form" << endl;
  if(server_period*budget > WCET){
    cout << "Bandwith grater than the worst case requirements" << endl;
    pi_0 = 1.0;
    return;
  }
  if(abs(a0p < 1e-10)){
    if(verbose_flag){
      cout << "Bandwith too small" << endl;
      pi_0 = 0.0;
      return;
    }
  }

  for(unsigned int i = server_period*budget+1; i < WCET; i++)
    pi_0 -= (i-server_period*budget)*(c.get(i)-c.get(i-1))/a0p;

  if(verbose_flag)
    cout << "Analytic computation completed" << endl;
  
  if(pi_0 < 0)
    pi_0 = 0;

  return;
}

void AnalyticResourceReservationProbabilitySolver::post_process(){}
void AnalyticResourceReservationProbabilitySolver::fill_in_probability_map(){}
}