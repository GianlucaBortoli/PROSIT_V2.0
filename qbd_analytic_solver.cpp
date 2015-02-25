#include "qbd_analytic_solver.hpp"

namespace PrositCore {

void AnalyticResourceReservationProbabilitySolver::apply_algorithm(){
  PrositAux::cdf c(p.get_size(), p.get_offset());
  pmf2cdf(p, c);
  double a0p = c.get(N*Q-1);
  unsigned WCET = c.get_max();
  double pi_0 = 1.0; // where the result will be stored

  if(verbose_flag)
    cout << "Prepearing analytic form" << endl;
  if(N*Q > WCET){
    cout << "Bandwith grater than the worst case requirements" << endl;
    pi_0 = 1.0;
    return;
  }
  if(abs(a0p < 1e-10)){
    if(verbose_flag){
      coit << "Bandwith too small" << endl;
      pi_0 = 0.0;
      return;
    }
  }

  for(int i = N*Q+1; i < WCET; i++)
    pi_0 -= (i-N*Q)*(c.get(i)-c.get(i-1))/a0p;

  if(verbose_flag)
    cout << "Analytic computarion completed" << endl;
  if(pi_0 < 0)
    pi_0 = 0;

  return;
}

}