#ifndef QBD_RR_SOLVER_HPP
#define QBD_RR_SOLVER_HPP
/// @file qbd_rr_solver.hpp
///
/// Family of solvers for resource reservation tasks based on algorithms for
/// numeric solution of
/// quasi-brith death processes.
/// In particular:
/// LatoucheResourceReservationSolver applies the Latouche Algorithm (1) for the
/// solution. It applies to both periodic and aperiodic tasks
/// CRResourceReservationSolver applies the cyclic reduction algorithm (2)
/// (1) G. Latouche and V. Ramaswami. A logarithmic reduction algorithm for
/// quasi-birth-and-death processes. J. Appl. Probab., 30:650--674, 1993.
/// (2) B. Meing and D. Bini Improved cyclic reduction for solving queueing
/// problems.  Numerical Algorithms, 15:57--74, 1997
#include "probability_solver.hpp"
#include "auxiliary_func.hpp"
#include "pmf.hpp"
#include "task_descriptor.hpp"
#include <Eigen/Dense>
#include <memory>

namespace PrositCore {
class QBDResourceReservationProbabilitySolver
    : public ResourceReservationProbabilitySolver {
protected:
  Eigen::MatrixXd B0, A0, A1, A2, R;
  Eigen::RowVectorXd pi0;
  unsigned int granularity; /*!< Granularity for resampling computation time */
  bool compress_flag; /*!< Should we compress all the states smaller than the
                         minimum interarrival time ? */
  ///@brief Generates matrices used by QBD methods.
  void pre_process();
  ///@brief Computes the probability from R, A0, A1, A2, B0
  bool compute_pi0();
  ///@brief Generates matrices used by QBD methods.
  void post_process();
  ///@brief computes the probability map from pi0
  void fill_in_probability_map();

public:
  ///@brief Default constructor
  ///
  ///@param granularity granularity for resampling the distribution of
  ///computation time
  QBDResourceReservationProbabilitySolver(unsigned int grand)
      : ResourceReservationProbabilitySolver(), B0(), A0(), A1(), A2(), R(),
        pi0(), granularity(grand), compress_flag(false){};
  ///@brief Sets granularity to a desired value
  ///
  /// The distribution of computation time is resampled at the graularity
  /// parameter
  /// to reduce the size of the system maintaining a conservative compuation.
  ///@param grand desired value
  void set_granularity(unsigned int grand);
  ///@brief Set compress flag
  ///
  /// The system produces a model where all states corresponding to states
  /// smaller than
  /// the minimum interarrival time are compressed into a single state.
  void set_compress_flag() {
    compress_flag = true;
    reset();
  }

  virtual ~QBDResourceReservationProbabilitySolver(){};

private:
  ///@brief Extract the submatrices from a QBD having a recursive structure.
  ///
  /// The QBPD has the form
  /// [B  A0  0  0  0 ...]
  ///  A2 A1 A0  0  0 ...]
  ///   0 A2 A1 A0  0 ...]
  ///@param mat complete matrix containing the QBDP
  ///@param dim size of the submatrices
  ///@param submatrix B
  ///@param A0 submatrix A0
  ///@param A1 submatrix A1
  ///@param A2 submatrix A2
  void extract_sub_matrices(const MatrixXd &mat, int dim, MatrixXd &B,
                            MatrixXd &A0, MatrixXd &A1, MatrixXd &A2);
  ///@brief Computes the transition matrix in compressed form
  ///
  /// Computes the transition matrix for a
  /// reservation with computation time p and interarrival time u
  /// It uses the model we have used in JSS12 but it compresses the states
  /// smaller than the minimum interarrival time
  ///@param i row of the desired element
  ///@param j column of the desired element
  ///@param server period
  ///@param p cdf of the computation time
  ///@param u pmf of the interarrival time
  double matrix_prob_ts_compressed(int i, int j, int q, const PrositAux::cdf &p,
                                   const PrositAux::pmf &u);
  ///@brief Computes the transition matrix
  ///
  /// Computes the transition matrix for a
  /// reservation using the model in (1)
  ///(1) Luca Abeni, Nicola Manica, Luigi Palopoli: Efficient and robust
  ///probabilistic guarantees for real-time tasks. Journal of Systems and
  ///Software (JSS) 85(5):1147-1156 (2012)
  ///@param i row of the desired element
  ///@param j column of the desired element
  ///@param server period
  ///@param p cdf of the computation time
  ///@param u pmf of the interarrival time
  double matrix_prob_ts(int i, int j, int q, const PrositAux::cdf &p,
                        const PrositAux::pmf &u);
};

class LatoucheResourceReservationProbabilitySolver
    : public QBDResourceReservationProbabilitySolver {
public:
  ///@brief Default constructor
  ///
  ///@param grand Desired value of the granularity for resampling the
  ///distribution of computation time
  ///@param epsilon_d desired value for the espilon parameter
  ///@param max_iter_d cut off value for the number of iterations
  LatoucheResourceReservationProbabilitySolver(unsigned int grand,
                                               double epsilon_d,
                                               unsigned int max_iter_d)
      : QBDResourceReservationProbabilitySolver(grand), epsilon(epsilon_d),
        max_iter(max_iter_d) {
    if (epsilon < 0)
      EXC_PRINT("Epsilon parameter has to be non negative for Latouche solver");
  };
  ///@brief Destructor
  virtual ~LatoucheResourceReservationProbabilitySolver(){};
  ///@brief resets the maximum number of iterations to a specified value
  ///
  ///@param max_iter_d desired maximum number of iterations
  void set_max_iter(unsigned int max_iter_d) {
    max_iter = max_iter_d;
    reset();
  };
  ///@brief resets epsilon to a desired value
  ///
  ///@param epsilon value of epsilon
  void set_epsilon(double epsilon_d) {
    if (epsilon_d < 0)
      EXC_PRINT("Epsilon parameter has to be non negative for Latouche solver");
    epsilon = epsilon_d;
    reset();
  };
  /////@brief Solver specific algorithm (called by solve)
  // virtual void apply_algorithm();

protected:
  double epsilon; /*!< Thrshold for the result of two subsequent iterations */
  unsigned int max_iter; /*!< Cut off for the maximum number of iterations */
  ///@brief executes the iterations of the latouche method
  ///
  /// This implementation implements the Latouche method
  /// for discrete time QBDP of the form
  /// [B  A0  0  0  0 ...]
  /// A2 A1 A0  0  0 ...]
  /// 0 A2 A1 A0  0 ...]
  /// The iteration produces a matrix R that has to be post-processed
  /// Matrix A0, A1, A2 and R are the ones that can be found solver after
  /// calling compute_matrices
  void apply_algorithm();
};

class CRResourceReservationProbabilitySolver
    : public QBDResourceReservationProbabilitySolver {
public:
  ///@brief Default constructor
  ///
  ///@param grand Desired value of the granularity for resampling the
  ///distribution of computation time
  ///@param epsilon_d desired value for the espilon parameter
  ///@param max_iter_d cut off value for the number of iterations
  CRResourceReservationProbabilitySolver(unsigned int grand, bool shift_d,
                                         int max_iter_d)
      : QBDResourceReservationProbabilitySolver(grand), shift_f(shift_d),
        max_iter(max_iter_d), G(), U(){};
  ///@brief Destructor
  virtual ~CRResourceReservationProbabilitySolver(){};
  ///@brief Sets the drift parameter
  void set_shift() { shift_f = true; };
  ///@brief Sets the maximum number of iterations
  void set_max_iter(int max_iter_d) { max_iter = max_iter_d; };

protected:
  ///@brief executes the iterations of the latouche method
  ///
  /// This implementation implements the CR method
  /// for discrete time QBDP of the form
  /// [B  A0  0  0  0 ...]
  /// A2 A1 A0  0  0 ...]
  /// 0 A2 A1 A0  0 ...]
  /// The iteration produces a matrix R that has to be post-processed
  /// Matrix A0, A1, A2 and R are the ones that can be found solver after
  /// calling compute_matrices
  void apply_algorithm();

private:
  bool shift_f;
  int max_iter;
  MatrixXd G;
  MatrixXd U;
};
}
#endif