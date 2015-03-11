/// @file qbd_latouche_solver.cpp
/// Implementation of the Cyclic Reduction algorithm
#include "qbd_rr_solver.hpp"

namespace PrositCore {
void CRResourceReservationProbabilitySolver::apply_algorithm() {
  if (!PrositAux::check_sizes(A0, A1) || !PrositAux::check_sizes(A1, A2) ||
      !PrositAux::check_sizes(A1, R))
    EXC_PRINT("A0, A1, A2 matrixes have to be square and equal size");

  MatrixXd A0c = A0;
  MatrixXd A1c = A1;
  MatrixXd A2c = A2;
  double drift = 0;

  MatrixXd Id = MatrixXd::Identity(A2c.rows(), A2c.cols());
  VectorXd D = A2c.diagonal();
  VectorXd u(A1c.rows());
  u.setOnes();
  RowVectorXd uT(A1c.cols());
  uT.setOnes();
  uT = 1 / (A2c.cols()) * uT;

  if (D.sum() < 0)
    EXC_PRINT("Only discrete time supported at the moment");

  if (shift_f) {
    RowVectorXd theta = PrositAux::stat(A2c + A1c + A0c);
    double tmp;

    drift = theta * A2c.rowwise().sum();
    tmp = theta * A0c.rowwise().sum();
    drift = drift - tmp;
    if (drift < 0) { // MC is transient: use its dual
      A0c = A0c - u * (theta * A0c);
      A1c = A1c + u * (theta * A2c);
    } else {
      A2c = A2c - A2c.rowwise().sum() * uT;
      A1c = A1c + A2.rowwise().sum() * uT;
    }
  }

  // Start of Cyclic Reduction (Basic)
  MatrixXd A = A1c;
  MatrixXd B = A0c;
  MatrixXd C = A2c;
  MatrixXd Ahat = A;

  double check = 1.0;
  long numit = 0;

  while ((check > 1e-14) && numit < max_iter) {
    MatrixXd Atemp;
    MatrixXd BAtemp;

    Atemp = (Id - A).inverse();
    BAtemp = B * Atemp;
    Atemp = C * Atemp;
    Ahat = Ahat + BAtemp * C;
    A = A + BAtemp * C + Atemp * B;
    B = BAtemp * B;
    C = Atemp * C;
    numit += 1;

    check = min(PrositAux::InfinityNorm<MatrixXd>(B),
                PrositAux::InfinityNorm<MatrixXd>(C));

    if (task_descriptor->get_verbose())
      cerr << "After " << numit << " iterations " << check << " reached"
           << endl;
  }

  G = ((Id - Ahat).inverse()) * A2c;
  if (numit == max_iter)
    cerr << "Warning: maximum number of iterations reached " << endl;
  if (shift_f) {
    if (drift < 0) {
      A1c = A1;
      A0c = A2;
    } else {
      G = G + u * uT;
      A1c = A1;
      A2c = A0;
    }
  }

  if (task_descriptor->get_verbose()) {
    cerr << "Final Residual Error for G: "
         << PrositAux::InfinityNorm<MatrixXd>(G - A2c - (A1c + A0c * G) * G)
         << endl;
  }
  R = A0c * (Id - (A1c + A0c * G)).inverse();
  if (task_descriptor->get_verbose()) {
    cerr << "Final Residual Error for R: "
         << PrositAux::InfinityNorm<MatrixXd>(R - A0c - R * (A1c + R * A2c))
         << endl;
  }
  U = A1c + R * A2c;
  if (task_descriptor->get_verbose()) {
    cerr << "Final Residual Error for U: "
         << PrositAux::InfinityNorm<MatrixXd>(
                U - A1c - A0c * (Id - U).inverse() * A2c) << endl;
  }
  return;
}
}