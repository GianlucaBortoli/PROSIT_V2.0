#include "qbd_rr_solver.hpp"

///@file qbd_rr_solver.cpp
///
/// Implementation of general fuynction for qbd based solvers.
/// The core is the method for the computation of the matrices
/// In addition we have some auxiliary function for managing matrices.
namespace PrositCore {
///@brief Computes the transition matrix
///
/// Computes the transition matrix for a
/// reservation using the model in (1)
///(1) Luca Abeni, Nicola Manica, Luigi Palopoli: Efficient and robust
///probabilistic guarantees for real-time tasks. Journal of Systems and Software
///(JSS) 85(5):1147-1156 (2012)
///@param i row of the desired element
///@param j column of the desired element
///@param q server period
///@param p cdf of the computation time
///@param u pmf of the interarrival time
double QBDResourceReservationProbabilitySolver::matrix_prob_ts(
  int i, int j, int q, const PrositAux::cdf &p, const PrositAux::pmf &u) {

  #ifdef DEBUG
    cerr << "matrix_prob_ts called" << endl;
    cerr << "i = " << i << endl;
    cerr << "j = " << j << endl;
    cerr << "u.get_min() " << u.get_min() << endl;
    cerr << "u.get_max() " << u.get_max() << endl;
    cerr << "u: " << endl;
    u.print();
    cerr << "p: " << endl;
    p.print();
  #endif

  double prob = 0;
  int l = u.get_max();
  if (i < u.get_min() * q)
    if (j == 0)
      prob = p.get(j);
    else
      prob = p.get(j) - p.get(j - 1);
  else
    for (int z = u.get_min(); z <= l; z++)
      if ((j - i + z * q) <= p.get_min())
        prob += u.get(z) * (p.get(j - i + z * q));
      else
        prob += u.get(z) * (p.get(j - i + z * q) - p.get(j - i + z * q - 1));

  #ifdef DEBUG
    cerr << "Probability Returned = " << prob << endl;
  #endif
    
  return prob;
}

///@brief Computes the transition matrix in compressed form
///
/// Computes the transition matrix for a
/// reservation with computation time p and interarrival time u
/// It uses the model we have used in JSS12 but it compresses the states
/// smaller than the minimum interarrival time
///@param i row of the desired element
///@param j column of the desired element
///@param q server period
///@param p cdf of the computation time
///@param u pmf of the interarrival time
double QBDResourceReservationProbabilitySolver::matrix_prob_ts_compressed(
    int i, int j, int q, const PrositAux::cdf &p, const PrositAux::pmf &u) {
#ifdef DEBUG
  cerr << "matrix_prob_ts called" << endl;
  cerr << "i = " << i << endl;
  cerr << "j = " << j << endl;
  cerr << "u.get_min() " << u.get_min() << endl;
  cerr << "u.get_max() " << u.get_max() << endl;
  cerr << "u: " << endl;
  u.print();
  cerr << "p: " << endl;
  p.print();
#endif
  double prob = 0;
  int zmin = u.get_min();
  int zmax = u.get_max();
  if (i == 0) {
    if (j == 0) {
      // P(v_h+1 <= zmin *Q|v_h <= z_min*Q)=P(c <=zmin*Q))
      prob = p.get(zmin * q);
    } else {
      // P(v_h+1 = zmin *Q+j |v_h <= z_min*Q)=P(c =zmin*Q+j))
      prob = p.get(zmin * q + j) - p.get(zmin * q + j - 1);
    }
  } else {
    if (j == 0) {
      // P(v_h+1 <= zmin *Q|v_h = z_min*Q+i)=P(c -zQ = -i)+ P(c -zQ =
      // -i-1)+...+P(c -zQ = -i-zmin*Q)
      for (int h = zmin; h <= zmax; h++)
        prob += u.get(h) * (p.get(h * q - i) - p.get(h * q - i - zmin * q));
    } else {
      // P(v_h+1 = zmin *Q+j|v_h = z_min*Q+i)=P(c -zQ = -i)+ P(c -zQ =
      // -i-1)+...+P(c -zQ = -i-zmin
      for (int h = zmin; h <= zmax; h++)
        prob += u.get(h) * (p.get(h * q + j - i) - p.get(h * q + j - i - 1));
    }
  }
#ifdef DEBUG
  cerr << "Probability Returned = " << prob << endl;
#endif
  return prob;
}

///@brief Extract the submatrices from a QBD having a recursive structure.
///
/// The QBPD has the form
/// [B  A0  0  0  0 ...]
///  A2 A1 A0  0  0 ...]
///   0 A2 A1 A0  0 ...]
///@param mat complete matrix containing the QBDP
///@param dim size of the submatrices
///@param B  submatrix B
///@param A0 submatrix A0
///@param A1 submatrix A1
///@param A2 submatrix A2
void QBDResourceReservationProbabilitySolver::extract_sub_matrices( const MatrixXd &mat, 
                                                                    int dim, 
                                                                    MatrixXd &B, 
                                                                    MatrixXd &A0, 
                                                                    MatrixXd &A1, 
                                                                    MatrixXd &A2) {
  B  = mat.block(0, 0, dim, dim);
  A0 = mat.block(0, dim, dim, dim);
  A2 = mat.block(dim, 0, dim, dim);
  A1 = mat.block(dim, dim, dim, dim);
}

void QBDResourceReservationProbabilitySolver::pre_process() {
  int maxv;     // size of the transition matrix
  int R0;       // # of rows to be repeated before beginning transient section
  int R1;       // # of transient rows
  int H1;       // # of columns to be shifted
  int H2;       // total # of rows
  int max_rows; // # of repeated rows in the transition matrix 
  int max_cols; // # of columns in the transition matrix

  if (!task_descriptor)
    EXC_PRINT("generate_matrices requested before linking any task descriptor");

  unsigned step = granularity;
  unsigned Q = task_descriptor->get_budget();

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

    tmp = (task_descriptor->get_computation_time())->resample(step, true);
    
    if (verbose_flag) {
      cout << "Distribution resampled" << endl;
      cout << "WCET: " << tmp->get_max() << endl;
      cout << "BCET: " << tmp->get_min() << endl;
    }
  } else {
    tmp = new PrositAux::pmf(*task_descriptor->get_computation_time());
  }

  std::unique_ptr<PrositAux::pmf> c(tmp); //computation

  if (verbose_flag) {
    cout << "Maximum interarrival time: "
         << (task_descriptor->get_interarrival_time())->get_max() << endl;
    cout << "Minimum interarrival time: "
         << (task_descriptor->get_interarrival_time())->get_min() << endl;
    cout << "Now resampling the distribution at server period " << endl;
  }
  tmp = (task_descriptor->get_interarrival_time())
            ->resample(task_descriptor->get_server_period(), false);

  if (verbose_flag) {
    cout << "Resampled distribution " << endl;
    cout << "Maximum interarrival time: " << tmp->get_max() << endl;
    cout << "Minimum interarrival time: " << tmp->get_min() << endl;
  }

  std::unique_ptr<PrositAux::pmf> u(tmp); //interarrival

  PrositAux::cdf cdfc(c->get_size(), 0);
  // 1. compute cdf of U
  PrositAux::pmf2cdf(*c, cdfc);
  if (verbose_flag) {
    if (!Q % step)
      cerr << "Warning: granularity is not a submultiple of budget in "
              "computation of matrix for task " << task_descriptor->get_name()
           << endl;
  }

  Q = Q / step;

  if (compress_flag) {
    if (verbose_flag)
      cout << "Computing matrix in compressed form" << endl;
    if (int(u->get_min() * Q) > c->get_max())
      EXC_PRINT_2("Compressed mode not usable: task always finishes before its "
                  "next activation. Task:",

                  task_descriptor->get_name());
    // 2.1 compute maxvalues for rows and cols
    max_cols = c->get_max() + 1 - u->get_min() * Q;
    max_rows = u->get_min() * Q - c->get_min() + 1;
  } else {
    if (verbose_flag)
      cout << "Computing matrix in Extended form" << endl;
    // 2.2 compute maxvalues for rows and cols
    int cmax = (c->get_max() > 1 ? c->get_max() : 1);
    int cmin = (c->get_min() > 1 ? c->get_min() : 1);
    int zmax = (u->get_max() > 1 ? u->get_max() : 1);
    int zmin = (u->get_min() > 1 ? u->get_min() : 1);

    R0 = (zmin * Q) - cmin + 1;
    R1 = (zmax - zmin) * Q;
    H1 = ((zmax - zmin) * Q) + (cmax - cmin) + 1;
    H2 = R0 + R1;
    
    max_rows = u->get_min() * Q + 1;
    max_cols = c->get_max() + 1;
  }

  cout << endl;
  cout << "c->get_min() = " << c->get_min() << endl;
  cout << "c->get_max() = " << c->get_max() << endl;
  cout << "u->get_min() = " << u->get_min() << endl;
  cout << "u->get_max() = " << u->get_max() << endl;
  cout << "max_rows = " << max_rows << endl;
  cout << "max_cols = " << max_cols << endl;
  cout << "R0 = " << R0 << endl;
  cout << "R1 = " << R1 << endl;
  cout << "H1 = " << H1 << endl;
  cout << "total # of rows = " << H2 << endl;
  cout << endl;

  // 2.3 take the maximum as actual size of the submatrix
  maxv = max(max_rows, max_cols);
  if (maxv <= 0)
    maxv = 1;
  if (verbose_flag)
    cout << "Computing Matrix. Size: " << maxv << endl;

  // 3. compute the whole matrix
  MatrixXd mat(2 * maxv, 2 * maxv);
  mat.setZero(); //when created there could be junk in it
  RowVectorXd v(2 * maxv); //holds the vector with the pmf, which is repeated |max_rows| times
                           //and the last computed row to be used in shifting when applied
                           //to aperiodic tasks

  for(int i = 0; i < maxv * 2; i++)
    v(i) = matrix_prob_ts(0, i, Q, cdfc, *u); //compute vector
  
  for(int i = 0; i < max_rows; i++)
    mat.row(i) << v; //initialize repeated rows

  int selectedRow = max_rows; //row to start swifting 
  int shift = maxv*2 - 1; //shift by one on the right
  v << mat.row(selectedRow - 1); //the shifted rows are the repetition of the last computed one
  while(selectedRow < maxv*2 && shift >= 0){ //shift rows by one on the right from |selectedRow| until the end
    mat.row(selectedRow).tail(shift) << v.head(shift);
    selectedRow++;
    shift--;
  }
  
  /* OLD MATRIX CREATION 
  MatrixXd mat2(2 * maxv, 2 * maxv); //the size for the whole matrix is twice the size of B
  for (int i = 0; i < maxv * 2; i++) {
    for (int j = 0; j < maxv * 2; j++) {
      if (compress_flag)
        mat2(i, j) = matrix_prob_ts_compressed(i, j, Q, cdfc, *u);
      else
        mat2(i, j) = matrix_prob_ts(i, j, Q, cdfc, *u);
    }
  }
  */

  if (verbose_flag)
    cout << "Matrix computed. Now extracting submatrixes " << endl;

  // 4. Extract submatrix
  A0 = MatrixXd(maxv, maxv);
  B0 = MatrixXd(maxv, maxv);
  A1 = MatrixXd(maxv, maxv);
  A2 = MatrixXd(maxv, maxv);
  extract_sub_matrices(mat, maxv, B0, A0, A1, A2);

  if (verbose_flag)
    cout << "Submatrices extracted " << endl;
  // cerr<<"A1: "<<A1<<endl;
  R = MatrixXd(A0.rows(), A0.cols());
  return;
}

bool QBDResourceReservationProbabilitySolver::compute_pi0() {
  if (!task_descriptor)
    EXC_PRINT("Computation of pi0 requires a task descriptor");
  if (!solved)
    EXC_PRINT_2("Computation of pi0 comes after solution. Task: ", task_descriptor->get_name());
  if (post_process_done) {
    if (verbose_flag)
      cerr << "Computation of pi0 required twice. Task "
           << task_descriptor->get_name() << endl;
    if (R.rows() != R.cols())
      EXC_PRINT("R had to be square");
  }

  if (!PrositAux::check_sizes(A0, R) || !PrositAux::check_sizes(A0, B0)) {
    EXC_PRINT("A0, A1, A2 matrixes have to be square and equal size");
  }

  if ((R.minCoeff() < 0) && verbose_flag)
    cerr << "QBD_COMPUTE_PI0: Warning: R has negative coeeficients" << endl;

  SelfAdjointEigenSolver<MatrixXd> eigensolver(R);

  if (eigensolver.info() != Success) {
    if (verbose_flag)
      cerr << "QBD_COMPUTE_PI0: cannot compute eigenvalues of R" << endl;
    return false;
  }

  if ((ArrayXd(eigensolver.eigenvalues()).abs().maxCoeff() > 1) && verbose_flag)
    cerr << "QBD_COMPUTE_PI0: Warning: R has spectral radius greater than 1" << endl;

  int n = R.rows();
  MatrixXd Id = MatrixXd::Identity(n, n);
  VectorXd u(n);
  u.setOnes();
  MatrixXd M(n, n + 1);
  M.block(0, 0, n, n) = B0 + R * A0 - Id;
  M.block(0, n, n, 1) = (Id - R).inverse() * u;

  FullPivLU<MatrixXd> lu_decomp(M);

  if (lu_decomp.rank() < n) {
    if (verbose_flag)
      cerr << "QBD_COMPUTE_PI0: No unique solution" << endl;
    return false;
  }

  RowVectorXd work(n + 1);
  work.setZero();
  work(n) = 1;
  MatrixXd W1;
  PrositAux::pseudoInverse<MatrixXd>(M, W1);
  pi0 = work * W1;

  if ((pi0.minCoeff() < 0) && verbose_flag)
    cerr << "QBD_COMPUTE_PI0: Warning: x0 has negative elements" << endl;

  return true;
}

void QBDResourceReservationProbabilitySolver::fill_in_probability_map() {
  #ifdef DEBUG
    cerr << "pi0 :" << pi0 << endl;
    cerr << "Probability: " << pi0.sum() << endl;
  #endif

  PrositAux::pmf *tmp;
  tmp = (task_descriptor->get_interarrival_time())
            ->resample(task_descriptor->get_server_period(), false);
  unsigned int T = task_descriptor->get_server_period();
  unsigned int Q = task_descriptor->get_budget();

  std::unique_ptr<PrositAux::pmf> u(tmp);

  DeadlineProbabilityMap *pm = task_descriptor->get_probabilistic_deadlines();
  DeadlineProbabilityMapIter pmi;
  unsigned int max_deadline = 0;
  for (pmi = pm->begin(); pmi != pm->end(); pmi++) {
    if (((*pmi).first < unsigned(u->get_min())) && compress_flag)
      cerr << "Deadline " << (*pmi).first * T
           << " will not be computed for task " << task_descriptor->get_name()
           << " because it used the compressed model" << endl;
    if (max_deadline < (*pmi).first)
      max_deadline = (*pmi).first;
  }

  #ifdef DEBUG
    cout << "max_deadline = " << max_deadline << endl;
    cout << "Q = " << Q << endl;
    cout << "size = " << pi0.size() << endl;
    cout << "gran = " << granularity << endl;
  #endif
    
  /// The probability of state pi_h is found as pi_0 R^h
  /// Thus he have to didentify the maximum H
  int H = ceil((max_deadline * Q) / (pi0.size()));

  Q = Q / granularity;

  Eigen::RowVectorXd pi = pi0;
  int delta = 0;
  double prob = 0.0;

  if (compress_flag)
    delta = u->get_min();

  for (int h = 0; h <= H; h++) {
    for (int i = 0; i < pi.size(); i++) {
      prob += pi(i);

      if (i % Q == 0) {
        if ((pmi = pm->find(delta)) != pm->end()) {
          (*pmi).second = prob;
          if (task_descriptor->get_verbose())
            cout << "P { d < " << T *(delta) << " } = " << prob << endl;
        }
        delta++;
      }
    }
    pi = pi * R;
  }
}

void QBDResourceReservationProbabilitySolver::post_process() {
  if (!compute_pi0())
    if (task_descriptor->get_verbose())
      cerr << "Warning: anomalies in the computation of pi0" << endl;
}
}