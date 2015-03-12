///@file qbd_latouche_solver.cpp
///
/// Implementation of the Latouche solver:
/// G. Latouche and V. Ramaswami. A logarithmic reduction algorithm for
/// quasi-birth-and-death processes. J. Appl. Probab., 30:650--674, 1993.
#include "qbd_rr_solver.hpp"
#include "exc.hpp"

namespace PrositCore {
void LatoucheResourceReservationProbabilitySolver::apply_algorithm() {
  if (!PrositAux::check_sizes(A0, A1) || !PrositAux::check_sizes(A1, A2) ||
      !PrositAux::check_sizes(A1, R)) {
    EXC_PRINT("A0, A1, A2 matrixes have to be square and equal size");
  }
  MatrixXd Rnew(R.rows(), R.cols());
  unsigned long it = 0;
  bool cont = true;
  R.setZero();

  while (cont) {
    Rnew = A0 + R * A1 + R * R * A2;
    if (task_descriptor->get_verbose())
      cerr << "||R-Rnew||" << PrositAux::InfinityNorm(R - Rnew) << endl;

    if (PrositAux::InfinityNorm(R - Rnew) < epsilon) {
      cont = false;
    } else {
      R = Rnew;
    }

    if (it++ > max_iter) {
      cont = false;
      cerr << "QBD_LATOUCHE:Maximum number of iterations reached" << endl;
    };

    if ((it % 10 == 0) && (task_descriptor->get_verbose()))
      cerr << "QBD_LATOUCHE: Iteration " << it << " reached." << endl;
  }

  R = Rnew;
}
}