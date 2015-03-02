#include "auxiliary_func.hpp"
#include <iostream>
using namespace std;

namespace PrositAux {
RowVectorXd stat(const MatrixXd &A) throw(Exc) {
  if (A.rows() != A.cols())
    throw Exc("Function Stat: The matrix A has to be square");

  VectorXd e(A.rows());
  MatrixXd B(A.rows(), A.cols() + 1);
  MatrixXd P;
  e.setOnes();

  B.block(0, 0, A.rows(), A.cols()) =
      A - MatrixXd::Identity(A.rows(), A.cols());
  B.block(0, A.cols(), e.rows(), e.cols()) = e;

  RowVectorXd y(A.rows() + 1);
  y.setZero();
  y(A.rows()) = 1;
  pseudoInverse<MatrixXd>(B, P);
  return y * P;
}

void help() {
  cout << "Usage: ./solver [options]\n"
       << "\t-t (--period): set period of server (required)\n"
       << "\t-q (--budget): set the budget (required, default: 10000)\n"
       << "\t-e (--epsilon): set the epsilon value (default: 1e-14)\n"
       << "\t-i (--max_iterations): set the maximum number of iterations for the algorithm\n"
       << "\t-T (--task_period): set the task period\n"
       << "\t-s (--step): set the sampling step\n"
       << "\t-M (--max_deadline): set the maximum value for the deadline\n"
       << "\t-v (--verbose): set verbose flag to true\n"
       << "\t-b (--brief)\n"
       << "\t-p (--pessimistic)\n"
       << "\t-m (--compress)\n"
       << "\t-S (--shift_flag)\n"
       << "\t-h (--help): show help for possible parameters\n\n"
       << "\tSolving algorithms:\n"
       << "\t-l (--latouche)\n"
       << "\t-c (--cyclic)\n"
       << "\t-o (--companion)\n"
       << "\t-a (--analytic)\n" << endl;
  return;
}
}
