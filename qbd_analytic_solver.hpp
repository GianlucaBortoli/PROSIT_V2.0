#ifndef QBD_ANALYTIC_SOLVER_HPP
#define QBD_ANALYTIC_SOLVER_HPP

#include "probability_solver.hpp"
#include "pmf.hpp"

namespace PrositCore {
class AnalyticResourceReservationProbabilitySolver 
    : public ResourceReservationProbabilitySolver {
protected:
  PrositAux::pmf prob_function;
  unsigned int server_period;
  unsigned int budget;

  ///@brief Generates matrices required for the solution
  void pre_process();
  ///@brief checks that everything is ok for computation
  void post_process();
  ///@brief computes the various probababilities after the "core" problem has
  ///been solved
  void fill_in_probability_map();
  ///@brief Applies algorithm
  void apply_algorithm();
public:
  /*
   * @param p probability mass function
   * @param N server period
   * @param Q budget 
   */
  AnalyticResourceReservationProbabilitySolver(PrositAux::pmf & p,
                                               unsigned int N,
                                               unsigned int Q)
    : prob_function(p), server_period(N), budget(Q){};

  virtual ~AnalyticResourceReservationProbabilitySolver(){};
};
}
#endif