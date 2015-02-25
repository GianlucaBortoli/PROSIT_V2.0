#include "probability_solver.hpp"
#include "auxiliary_func.hpp"
#include "pmf.hpp"
#include "task_descriptor.hpp"
#include <Eigen/Dense>
#include <memory>

namespace PrositCore {
class AnalyticResourceReservationProbabilitySolver 
    : public ResourceReservationProbabilitySolver {
protected:
  void apply_algorithm();
public:
  /*
   * @param p probability mass function
   * @param N 
   * @param Q budget 
   */
  AnalyticResourceReservationProbabilitySolver(const PrositAux::pmf & p,
                                               unsigned int N,
                                               unsigned int Q){};

  virtual ~AnalyticResourceReservationProbabilitySolver(){};
};
}