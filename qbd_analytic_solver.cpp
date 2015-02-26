#include "qbd_analytic_solver.hpp"

namespace PrositCore {

void AnalyticResourceReservationProbabilitySolver::apply_algorithm(){
  PrositAux::cdf c(prob_function.get_size(), prob_function.get_offset());
  pmf2cdf(prob_function, c);
  double a0p = c.get(N*budget-1);
  unsigned WCET = c.get_max();
  double pi_0 = 1.0; // where the result will be stored

  if(verbose_flag)
    cout << "Prepearing analytic form" << endl;
  if(N*budget > WCET){
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

  for(unsigned int i = N*budget+1; i < WCET; i++)
    pi_0 -= (i-N*budget)*(c.get(i)-c.get(i-1))/a0p;

  if(verbose_flag)
    cout << "Analytic computarion completed" << endl;
  if(pi_0 < 0)
    pi_0 = 0;

  return;
}

}