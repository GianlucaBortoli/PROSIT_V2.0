#include "qbd_companion_solver.hpp"

/////////////////////////
#include <Eigen/Dense>
#include <algorithm>
/////////////////////////

namespace PrositCore {
static complex<double> GammaFunctionsTotal(const complex<double> &b, int dim,
                                           const RowVectorXd &Alpha) {
  int i;
  complex<double> gamma = pow<double>(b, dim);
  for (i = 1; i <= dim; i++) {
    gamma += pow<double>(b, int(dim) - i) * Alpha(i + 2);
  }
  return gamma;
};

bool CompanionResourceReservationProbabilitySolver::check_list() {
  bool ok = ResourceReservationProbabilitySolver::check_list();
  bool ok_periodic = false;

  if (linked_flag)
    ok_periodic = task_descriptor->is_periodic();

  if (!ok) {
    if (verbose_flag)
      cerr << "CompanionResourceReservationProbabilitySolver::check_list() "
              "failed" << endl;
    return false;
  }

  if (!ok_periodic) {
    if (verbose_flag)
      cerr << "CompanioniResourceReservationProbabilitySolver curretnly "
              "applies to periodic tasks" << endl;
    return false;
  }
  return true;
}

void CompanionResourceReservationProbabilitySolver::pre_process() {
  if (!task_descriptor)
    EXC_PRINT("generate_matrices requested before linking any task descriptor");

  unsigned step = granularity;
  unsigned Q = task_descriptor->get_budget();
  unsigned N = (task_descriptor->get_period()) /
               ((task_descriptor->get_server_period()));

  if (solved) {
    if (verbose_flag)
      cerr << "pre_process called on a solver in solved state for task "
           << task_descriptor->get_name() << endl;
    return;
  }

  if (pre_process_done) {
    if (verbose_flag)
      cerr << "pre_process called on a solver that has already generated the "
              "matrices for task " << task_descriptor->get_name() << endl;
    return;
  }

  PrositAux::pmf *tmp;

  if (step != 1) {
    if (verbose_flag) {
      cout << "WCET: " << (task_descriptor->get_computation_time())->get_max()
           << endl;
      cout << "BCET: " << (task_descriptor->get_computation_time())->get_min()
           << endl;
      cout << "Now resampling the distribution" << endl;
    }

    tmp = (task_descriptor->get_computation_time())->resample(step);

    if (verbose_flag) {
      cout << "Distribution resampled" << endl;
      cout << "WCET: " << tmp->get_max() << endl;
      cout << "BCET: " << tmp->get_min() << endl;
    }
  } else {
    tmp = new PrositAux::pmf(*task_descriptor->get_computation_time());
  }

  std::unique_ptr<PrositAux::pmf> p(tmp);

  PrositAux::cdf c(p->get_size(), p->get_offset());
  PrositAux::pmf2cdf(*p, c);
  unsigned BCET = c.get_min();
  unsigned WCET = c.get_max();
  unsigned size = (WCET - BCET);
  double a0;
  a0 = c.get(BCET);
  RowVectorXd v(size);
  Alfa = RowVectorXd(size + 1);
  Alfa(0) = 1;

  if (verbose_flag)
    cout << "now preparing the companion form" << endl;
  if (N * Q > WCET) {
    if (verbose_flag)
      cout << "Bandwidth grater than Worst case requirements" << endl;
    return;
  };

  for (unsigned int i = 0; i < size; i++) {
    double ai = c.get(i + BCET + 1) - c.get(i + BCET);
    v(size - i - 1) = -ai / a0;
    Alfa(i + 1) = ai / a0;
  };

  v(WCET - N * Q) = v(WCET - N * Q) + 1 / a0;

  if (verbose_flag)
    cout << "last row of the comapnion form has size " << v.size() << endl;

  VectorXd z(size - 1);
  z.setZero();
  MatrixXd I = MatrixXd::Identity(size - 1, size - 1);
  MatrixXd M(size, size);
  M.block(0, 0, size - 1, 1) = z;
  M.block(0, 1, size - 1, size - 1) = I;
  M.block(size - 1, 0, 1, size) = v;

  if (verbose_flag)
    cout << "Companion form matrix size " << M.size() << endl;
}

void CompanionResourceReservationProbabilitySolver::apply_algorithm() {
  double pi_0;
  unsigned int size = M.rows();
  EigenSolver<MatrixXd> eigensolver(M, false);

  if (verbose_flag)
    cout << "Companion form eigenvalues found " << endl;
  if (eigensolver.info() != Success)
    EXC_PRINT("cannot compute eigenvalues of m");

  complex<double> product(1, 0);

  VectorXcd unstable(size);
  for (unsigned int i = 0; i < size; i++) {
    complex<double> p_i = eigensolver.eigenvalues()[i];

    if (abs(p_i) < 1 - 0.000000001) {
      product = product * (complex<double>(1, 0) - p_i);
    } else {
      unstable(unst_num++) = p_i;
    }
  }

  if (product.imag() > 0.00000001)
    cerr << "Warning the probability did not reslt into a real number" << endl;
  pi_0 = product.real();

  if ((pi_0 < 0) || (pi_0 > 1.0000001)) {
    cerr << "ERROR: companion did not produce the correct result" << endl;
    pi_0 = 0.0;
  };

  VectorXcd b(unst_num);
  MatrixXcd A(unst_num, unst_num);
  RowVectorXcd Ah(unst_num);

  for (unsigned int h = 0; h < unst_num; h++) {
    complex<double> betah = unstable(h);
    complex<double> bh = 0;

    for (unsigned int q1 = 0; q1 <= unst_num; q1++) {
      bh = bh + GammaFunctionsTotal(betah, q1, Alfa);
    }
    b(h - 1) = -bh * pi_0;
    for (unsigned int k = 1; k <= unst_num; k++) {
      complex<double> ah = 0;
      for (unsigned int q1 = 0; q1 <= unst_num - k; q1++)
        ah += GammaFunctionsTotal(betah, q1, Alfa);
      Ah(k - 1) = ah;
    }
    A.block(h, 0, h, unst_num - 1) = Ah;
  };

  RowVectorXd pi_final(unst_num + 1);
  pi_final(0) = pi_0;
  MatrixXcd Pinv;
  VectorXcd res;
  PrositAux::pseudoInverse<MatrixXcd>(A, Pinv);
  res = Pinv * b;

  for (unsigned int i = 1; i < unst_num + 1; i++) {
    complex<double> el = res(i);
    if (el.imag() > 0.00000001)
      cerr << "Warning the probability did not reslt into a real number"
           << endl;
    pi_final(i) = el.real();
  }
  pi0 = pi_final;
  return;
}

void CompanionResourceReservationProbabilitySolver::post_process(){};

void CompanionResourceReservationProbabilitySolver::fill_in_probability_map() {
  if (!task_descriptor)
    EXC_PRINT("Cannot call fill_in_probability_map on disconnected solver");
  unsigned int N =
      task_descriptor->get_period() / task_descriptor->get_server_period();
  unsigned T = task_descriptor->get_server_period();
  unsigned Q = task_descriptor->get_budget();
  DeadlineProbabilityMap *pm = task_descriptor->get_probabilistic_deadlines();

  DeadlineProbabilityMapIter pmi;
  unsigned int max_deadline = 0;

  for (pmi = pm->begin(); pmi != pm->end(); pmi++) {
    if (((*pmi).first < N))
      cerr << "Deadline " << (*pmi).first * T
           << " will not be computed for task " << task_descriptor->get_name()
           << endl;
    if (((*pmi).first > N + unst_num))
      cerr << "Deadline " << (*pmi).first * T
           << " will not be computed for task " << task_descriptor->get_name()
           << " because the feature is under implementation" << endl;
    if (max_deadline < (*pmi).first)
      max_deadline = (*pmi).first;
  }

  int delta = 0;
  double prob = 0.0;

  for (unsigned int i = 0; i < std::min(N + unst_num, max_deadline); i++) {
    prob += pi0(i);
    if (i % Q == 0) {
      if ((pmi = pm->find(delta)) != pm->end()) {
        (*pmi).second = prob;
        if (task_descriptor->get_verbose())
          cout << "P { d < " << T *(delta) << " } = " << prob << endl;
      };
      delta++;
    }
  }
  return;
}
};