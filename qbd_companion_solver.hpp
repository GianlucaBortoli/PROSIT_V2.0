#ifndef QBD_CMP_SOLVER_HPP
#define QBD_CMP_SOLVER_HPP

///@file qbd_companion_solver.hpp
///
/// Classes for companion for based solvers

#include "probability_solver.hpp"
#include "auxiliary_func.hpp"
#include "pmf.hpp"
#include "task_descriptor.hpp"
#include <Eigen/Dense>
#include <memory>

namespace PrositCore {
///@brief Root for the class hierarchy of solvers that apply companion form
///methods
class CompanionResourceReservationProbabilitySolver
    : public ResourceReservationProbabilitySolver {
protected:
  MatrixXd M;
  unsigned int granularity;
  double epsilon;
  /*!< Number of unstable eigenvalues of M.
       Unstable eigenvalues are associated with as many independent equation
       that allow us to find the fundamental elements of the probability
     vectors.
       The other elements come for free.*/
  unsigned unst_num;
  RowVectorXd Alfa;
  /*!< Vector of fundamental proabilities that can be used to construct the
     whole
         vector by a simple recursion */
  RowVectorXd pi0;
  ///@brief Generates matrices required for the solution
  void pre_process();
  ///@brief checks that everything is ok for computation
  bool check_list();
  ///@brief Generates the different probabilities
  void post_process();
  ///@brief Applies algorithm
  void apply_algorithm();
  ///@brief computes the various probababilities after the "core" problem has
  ///been solved
  void fill_in_probability_map();

public:
  ///@brief Default constructor
  ///
  ///@param gran_d desired value for granularity (to resample the functions if
  ///required)
  ///@param eps_d desired value for minum granularity for computations involving
  ///double
  CompanionResourceReservationProbabilitySolver(unsigned int gran_d = 1,
                                                double eps_d = 1e-7)
      : M(), granularity(gran_d), epsilon(eps_d), pi0(){};
  ///@brief Virtual destructor
  virtual ~CompanionResourceReservationProbabilitySolver(){};

  ///@brief Sets granularity to a desired value
  ///
  /// The distribution of computation time is resampled at the graularity
  /// parameter
  /// to reduce the size of the system maintaining a conservative compuation.
  ///@param grand desired value
  void set_granularity(unsigned int grand) {
    granularity = grand;
    reset();
  };

  ///@brief resets epsilon to a desired value
  ///
  ///@param epsilon value of epsilon
  void set_epsilon(double epsilon_d) {
    if (epsilon_d < 0)
      EXC_PRINT(
          "Epsilon parameter has to be non negative for Companion solver");
    epsilon = epsilon_d;
    reset();
  };
};
}
#endif