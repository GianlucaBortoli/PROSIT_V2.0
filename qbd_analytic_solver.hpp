#include "probability_solver.hpp"
#include "auxiliary_func.hpp"
#include "pmf.hpp"

namespace PrositCore {
class AnalyticResourceReservationProbabilitySolver 
    : public ResourceReservationProbabilitySolver {
protected:
  const PrositAux::pmf prob_function;
  unsigned int N;
  unsigned int budget;

  void apply_algorithm();
public:
  /*
   * @param p probability mass function
   * @param N 
   * @param Q budget 
   */
  AnalyticResourceReservationProbabilitySolver(const PrositAux::pmf & p,
                                               unsigned int N,
                                               unsigned int Q)
    : prob_function(p), N(N), budget(Q){};

  virtual ~AnalyticResourceReservationProbabilitySolver(){};
};
}