///@file Command line solver for computation of stady state probabilities
#include "pmf.hpp"
#include "auxiliary_func.hpp"
#include "qbd_rr_solver.hpp"
#include "qbd_companion_solver.hpp"
#include "qbd_analytic_solver.hpp"
#include "exc.hpp"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <getopt.h>
#include <memory>

#define Nc 2500000
#define Nz 1000000

static int Q = 10000;
static int T = 20000;
static int Tp = 0;
static int step = 1;
static int ps = 0;
static int verbose_flag = 0;
static int latouche_flag = 0;
static int companion_flag = 0;
static int compress_flag = 0;
static int analytic_flag = 0;
static int cr_flag = 0;
static int max_deadline = 0;
static int iter = 100;
static double eps = 1e-4;
static int shift_flag = 0;

static int opts_parse(int argc, char *argv[]) {
  int opt;
  static struct option long_options[] = {
      /* These options set a flag. */
      {"verbose", no_argument, 0, 'v'},
      {"brief", no_argument, 0, 'b'},
      {"latouche", no_argument, 0, 'l'},
      {"cyclic", no_argument, 0, 'c'},
      {"companion", no_argument, 0, 'o'},
      {"compress", no_argument, 0, 'm'},
      {"analytic", no_argument, 0, 'a'},
      {"shift_flag", no_argument, 0, 'S'},
      /* These options don't set a flag. We distinguish them by their indices */
      {"budget", required_argument, 0, 'q'},
      {"period", required_argument, 0, 't'},
      {"task_period", required_argument, 0, 'T'},
      {"epsilon", required_argument, 0, 'e'},
      {"max_iterations", required_argument, 0, 'i'},
      {"pessimistic", no_argument, 0, 'p'},
      {"step", required_argument, 0, 's'},
      {"max_deadline", required_argument, 0, 'M'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0},
  };

  while ((opt = getopt_long(argc, argv, "t:q:e:i:T:s:M:vbplcomaSh", long_options,
                            0)) != -1) {
    switch (opt) {
    case 'q':
      Q = atoi(optarg);
      break;
    case 't':
      T = atoi(optarg);
      break;
    case 'i':
      iter = atoi(optarg);
      break;
    case 'e':
      eps = atof(optarg);
      break;
    case 'T':
      Tp = atoi(optarg);
      break;
    case 'M':
      max_deadline = atoi(optarg);
      break;
    case 'p':
      ps = 1;
      break;
    case 'c':
      cr_flag = 1;
      break;
    case 'l':
      latouche_flag = 1;
      break;
    case 'o':
      companion_flag = 1;
      break;
    case 'v':
      verbose_flag = 1;
      break;
    case 'b':
      verbose_flag = 0;
      break;
    case 's':
      step = atoi(optarg);
      break;
    case 'm':
      compress_flag = 1;
      break;
    case 'a':
      analytic_flag = 1;
      break;
    case 'S':
      shift_flag = 1;
      break;
    case 'h':
      PrositAux::help();
      break;
    default: /* ?~@~Y??~@~Y */
      EXC_PRINT("opts_parse parameters incorrect");
    }
  }
  return optind;
}

int main(int argc, char *argv[]) {
  int i;
  long long t_start = 0, t_end;
  PrositAux::pmf *c = new PrositAux::pmf(Nc, 0); //computation time
  PrositAux::pmf *u = new PrositAux::pmf(Nz, 0); //interarrival time

  int opt;
  try {
    opt = opts_parse(argc, argv);
    if (ps == 1)
      EXC_PRINT("pessimistic version not implemented");
    if ((analytic_flag + cr_flag + latouche_flag + companion_flag) == 0)
      latouche_flag = 1;
    if ((analytic_flag + cr_flag + latouche_flag + companion_flag) != 1)
      EXC_PRINT("Ambiguous choice of the algorithm");
    if (max_deadline <= 0)
      EXC_PRINT("Maximum deadline not properly set");
    if ((shift_flag != 0) && (cr_flag == 0))
      cerr << "Warning! shift_flag only makes sense for CR" << endl;
    c->load(argv[opt]);

    if (companion_flag) {
      // TODO: we can easily apply to aperiodic
      if (argc - optind != 1)
        EXC_PRINT("Only one file parameter requested for companion form");
      if (Tp == 0)
        EXC_PRINT("Task period needs to be set for companion");
    } else {
      if (argc - optind == 2) {
        u->load(argv[opt + 1]);
      } else {
        if (Tp != 0)
          u->set(Tp, 1.0);
        else
          EXC_PRINT("Two file parameters requested");
      }
    }

    t_start = PrositAux::my_get_time();
    std::unique_ptr<PrositAux::pmf> cp(c);
    std::unique_ptr<PrositAux::pmf> up(u);

    PrositCore::ResourceReservationTaskDescriptor task_des(
        "task", std::move(cp), std::move(up), unsigned(Q), unsigned(T));
    task_des.set_deadline_step(T);
    task_des.set_verbose_flag(verbose_flag ? true : false);

    for (int i = 0; i <= max_deadline; i++) {
      task_des.insert_deadline(i);
    }

    if (analytic_flag) {
      PrositCore::AnalyticResourceReservationProbabilitySolver *tmp =
        new PrositCore::AnalyticResourceReservationProbabilitySolver(
          *c, unsigned(T), unsigned(Q));
      std::unique_ptr<PrositCore::ResourceReservationProbabilitySolver> ps(tmp);
      task_des.set_solver(ps.get());
      task_des.compute_probability();
    } else {
      if (companion_flag) {
        PrositCore::CompanionResourceReservationProbabilitySolver *tmp =
            new PrositCore::CompanionResourceReservationProbabilitySolver(step,
                                                                          eps);
        std::unique_ptr<PrositCore::ResourceReservationProbabilitySolver> ps(
            tmp);
        task_des.set_solver(ps.get());
        task_des.compute_probability();
      } else {
        PrositCore::QBDResourceReservationProbabilitySolver *tmp;
        if (cr_flag) {
          tmp = new PrositCore::CRResourceReservationProbabilitySolver(
              step, shift_flag ? true : false, iter);
        } else if (latouche_flag) {
          tmp = new PrositCore::LatoucheResourceReservationProbabilitySolver(
              step, eps, iter);
        } else {
          EXC_PRINT("Solver not implemented yet");
        }

        std::unique_ptr<PrositCore::QBDResourceReservationProbabilitySolver> ps(
            tmp);

        task_des.set_solver(ps.get());

        if (compress_flag)
          ps->set_compress_flag();

        task_des.compute_probability();
      }
    }

    for (i = 0; i <= max_deadline; i++)
      cout << "P { f < " << task_des.get_deadline_step() * i
           << " } = " << task_des.get_probability(i) << endl;

  } catch (PrositAux::Exc &e) {
    cerr << "Exception caught" << endl;
    e.what();
  }

  t_end = PrositAux::my_get_time();
  cout << "Total time: " << t_end - t_start << endl;

  return 0;
}