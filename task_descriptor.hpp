/*
* Classes and function for optimisation
*
* Copyright (C) 2014, University of Trento
* Authors: Luigi Palopoli <palopoli@disi.unitn.it>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/

/// !@file task_descriptor.hpp
/// This file contains the root elements of the hierarchy defining
/// task descriptor. At the root of the hiearchy we have the
/// GenericTaskDescriptor, which models both periodic and aperiodic
/// tasks depending on a flag. Then we have two subclasses that
/// model a different management of the task: FixedPriorityTaskDescriptor (task
/// scheduled using fixed priority), ResourceReservationTaskDescriptor (task
/// scheduled using
/// resource reservations). The key method in the hiearchy is
/// compute_probability, which
/// computes the probability of respecting a sequence of deadlines that have to
/// be registered
/// before calling the method and are multiple of a fixed basic deadline (an
/// obvious choice
/// for resource reservations is to have deadlines that are integer multiples of
/// the server period).
/// The computation of the probability relies on an externally provided solver
/// (see the documentation
/// of PrositCore::Solver class.
#ifndef TASK_DESCRIPTOR_HPP
#define TASK_DESCRIPTOR_HPP

#include "prosit_types.hpp"
#include "exc.hpp"
#include "pmf.hpp"
#include "probability_solver.hpp"
#include <vector>
#include <string>
#include <utility>
#include <memory>
#include <map>
using namespace std;

namespace PrositCore {
/// @brief Generic class descriptor
/*! It is the root of the hierarchy of task descriptors
 * This descriptor only contains the timing information.
 * The hierarchy is specialised on the basis of: 1) scheduling algorithm
 * 2) solver family
 */
class GenericTaskDescriptor {
protected:
  bool solved; /*!< The solver has been called */
protected:
  string name;                  /*!< Name of the task */
  unique_ptr<PrositAux::pmf> C; /*!< Distribution of the computation time */
  unique_ptr<PrositAux::pmf> Z; /*!< Distribution of the interarrival time */

  bool verbose_flag; /*!< Flag to print out information while processing */
  bool periodic;     /*!< Flag for periodic tasks */
  unsigned int P;    /*!< Period (has meaning onlly if periodic flag is set) */
  unsigned int deadline_step; /*!< Granularity to define probabilistic deadlines */

  DeadlineProbabilityMap probabilistic_deadlines; /*!< Map associating deadlines
                                                     with probabilities */
  ProbabilitySolver *probability_solver; /*!< Pointer to the object containing
                                            the solution algorithm for
                                            probabilities */
public:
  ///@brief Constructor for aperiodic Tasks
  /*! @param nm unique identifier for the task
   *  @param Cd pointer to the distribution of the computation times
   *  @param Zd distribution of the interarrival time
   */
  GenericTaskDescriptor(const char *nm, unique_ptr<PrositAux::pmf> Cd,
                        unique_ptr<PrositAux::pmf> Zd) throw(PrositAux::Exc)
      : solved(false), name(nm), C(std::move(Cd)), Z(std::move(Zd)),
        verbose_flag(false), periodic(false), P(0), deadline_step(0),
        probability_solver(0){};

  ///@brief Constructor for periodic tasks
  /*! @param nm unique identifier for the task
   *  @param Cd pointer to the distribution of the computation times
   *  @param Pd task period
   */
  GenericTaskDescriptor(const char *nm, unique_ptr<PrositAux::pmf> Cd,
                        unsigned int Pd) throw(PrositAux::Exc)
      : solved(false), name(nm), C(std::move(Cd)), Z(new PrositAux::pmf()),
        verbose_flag(false), periodic(true), P(Pd), deadline_step(0),
        probability_solver(0) {
    Z->set(Pd, 1.0);
  };

  ///@brief Set verbose_flag flag to a specified value
  /*!
   * @param verbose_flagd desidered value for the verbose_flag flag
   * @return old value for the verbose_flag flag
   */
  bool set_verbose_flag(bool verbose_flagd) {
    bool current = verbose_flag;
    verbose_flag = verbose_flagd;
    return current;
  };

  ///@brief Verify if the task is periodic
  bool is_periodic() const { return periodic; };
  ///@brief Returns the verbose_flag flag
  bool get_verbose() const { return verbose_flag; };
  ///@brief Returns the name of the task
  /*!
   *@return Sting containing the name of the task.
   */
  std::string get_name() const { return name; };

  ///@brief Returns the task period
  /*!
   * @return the task period (Exception raised if not periodic)
   */
  int get_period() const throw(PrositAux::Exc) {
    if (!periodic)
      EXC_PRINT_2("Period wrongly required for aperiodic task ", name);
    return P;
  };

  ///@brief Returns a copy of the computation time distribution
  /*!
   * @return a copy of the pmf related to the computation time
   */
  PrositAux::pmf *get_computation_time() const { return C.get(); };

  /// Returns a copy of the interarrival time distribution
  /*!
   * @return a copy of the pmf related to the interarrival time (Exceprion of
   * the task is periodic)
   */
  PrositAux::pmf *get_interarrival_time() const throw(PrositAux::Exc) {
    if (periodic)
      EXC_PRINT_2("Interarrival time wrongly required for periodic task ",
                  name);
    return Z.get();
  };

  ///@brief Returns the deadline step
  /*! It allows the user to know the deadline step size to be used
    in defining the probability map. All deadlines set in the map
    are intended as multiples of this basic unit.
    @return The current deadline step.
  */
  unsigned int get_deadline_step() const { return deadline_step; };

  ///@brief Sets the deadline step
  /*! It allows the user to decide the deadline step size to be used
    in defining the probability map. All deadlines set in the map
    are intended as multiples of this basic unit. The operation
    generates an exception if the probabilistic deadlines map is not
    empty.
    Additional constraints can be required by specific classes of tasks,
    hence the need to make this function virtual
    @param ds required deadline step.
   */
  virtual void set_deadline_step(unsigned int ds);

  ///@brief Returns an a pointer to the probabilistic dedadlines map
  /*! The user is descouraged to use this method directly. It is there
   * for it to be used by the solver.
   */
  DeadlineProbabilityMap *get_probabilistic_deadlines() {
    return &probabilistic_deadlines;
  };

  ///@brief Define a deadline
  /*! @param deadline has to be a multiple of deadline_step
   * @return Exception thrown for multiple entries and for non-multiple
   * of the deadline_step
   */
  void insert_deadline(DeadlineUnit deadline);

  ///@brief Clears deadline probability map
  /*! Removes all previously set deadlines. */
  void reset_probabilistic_deadlines() { probabilistic_deadlines.clear(); };

  ///@brief Computes the probability of respecting the deadlines
  /*! These probabilities are computed for a given configuration
   * of the scheduling parameters. (Deadlines have been previously registered
   * by calling insert. An exception is thrown if the solver is not set.
   *
   */
  virtual void compute_probability();

  ///@brief Returns the probability associated with a deadline
  /*!compute_probability is implicitly called if not called before.
   * @param deadline: the deadline for which the computations
   * is required.
   * @return The requested probability. An exception is thrown if the deadline
   * has not been registered.
   */
  double get_probability(DeadlineUnit deadline);

  ////@brief@brief Sets the probability solver
  ///
  /// Sets the external object that computes the probability.
  /// The probability computation has to be made anew.
  ///@param psd pointer to the probability solver that has to be used.
  /// The ownership of the solver is not taken by the task descriptor.
  /// If the solver is not appropriate for the task (e.g., if we use a FP solver
  /// for a resource reservation task), an exception is thrown by the solver
  /// during the task registration phase.
  void set_solver(ProbabilitySolver *psd);
  ///@brief Destruct, which is virtual, being the class polymorphic
  virtual ~GenericTaskDescriptor(){};
};

/// @brief Class for fixed priority task descriptors
class FixedPriorityTaskDescriptor : public GenericTaskDescriptor {
protected:
  unsigned int priority; /*!<Scheduling priority of the task */
public:
  ///@brief Constructor for aperiodic Fixed Priority Tasks
  /*! @param nm unique identifier for the task
   *  @param Cd pointer to the distribution of the computation times
   *  @param Zd distirbution of the interarrival time
   *  @param priorityd scheduling priority (in the range 0..99)
   */
  FixedPriorityTaskDescriptor(const char *nm, unique_ptr<PrositAux::pmf> Cd,
                              unique_ptr<PrositAux::pmf> Zd,
                              unsigned int priorityd) throw(PrositAux::Exc)
      : GenericTaskDescriptor(nm, std::move(Cd), std::move(Zd)) {
    if (priorityd > 99)
      EXC_PRINT_2("Priority out of range for task ", nm);
    priority = priorityd;
  };

  ///@brief Constructor for Fixed Priority periodic tasks
  /*! @param nm unique identifier for the task
   *  @param Cd pointer to the distribution of the computation times
   *  @param Pd task period
   *  @param priorityd scheduling priority (in the range 0..99)
   */
  FixedPriorityTaskDescriptor(const char *nm, unique_ptr<PrositAux::pmf> Cd,
                              unsigned int Pd,
                              unsigned int priorityd) throw(PrositAux::Exc)
      : GenericTaskDescriptor(nm, std::move(Cd), std::move(Pd)) {
    if (priorityd > 99)
      EXC_PRINT_2("Priority out of range for task ", nm);
    priority = priorityd;
  };

  ///@brief Returns the task priority
  unsigned int get_priority() const { return priority; };

  ///@brief Sets the task priority
  /*! @param priority new task priority
   * @return old priority
   */
  unsigned int set_priority(unsigned int priorityd) {
    unsigned int old_prio = priority;
    priority = priorityd;
    return old_prio;
  };
};

///@brief Task descriptors for tasks managed by the resource reservation
///algorithm.
class ResourceReservationTaskDescriptor : public GenericTaskDescriptor {
protected:
  unsigned int Q;  /*!< Reservation budget */
  unsigned int Ts; /*!< Reservation period */
public:
  ///@brief Constructor for aperiodic tasks
  ///
  /// An exception is thhrown if the budget is set improperly.
  ///@param nm task name
  ///@param Cd distribution of the computation time. The descriptor takes
  ///ownership of this pointer.
  ///@param Zd distribution of interarrival time. The descriptor takes ownership
  ///of this pointer.
  ///@param Qd Reservation Budget
  ///@param Tsd Reservation period.
  ResourceReservationTaskDescriptor(const char *nm,
                                    unique_ptr<PrositAux::pmf> Cd,
                                    unique_ptr<PrositAux::pmf> Zd,
                                    const unsigned int Qd,
                                    const unsigned int Tsd)
      : GenericTaskDescriptor(nm, std::move(Cd), std::move(Zd)), Q(Qd),
        Ts(Tsd) {
    if (double(Qd) / double(Tsd) > 1.0)
      EXC_PRINT_2("Server period period too small for task", name);
  };
  ///@brief Constructor for aperiodic tasks
  ///
  /// An exception is thhrown if the budget is set improperly.
  ///@param nm task name
  ///@param Cd distribution of the computation time. The descriptor takes
  ///ownership of this pointer.
  ///@param Pd Tas period
  ///@param Qd Reservation Budget
  ///@param Tsd Reservation period.
  ResourceReservationTaskDescriptor(const char *nm,
                                    unique_ptr<PrositAux::pmf> Cd,
                                    unsigned int Pd, const unsigned int Qd,
                                    const unsigned int Tsd)
      : GenericTaskDescriptor(nm, std::move(Cd), Pd), Q(Qd), Ts(Tsd) {
    if (double(Qd) / double(Tsd) > 1.0)
      EXC_PRINT_2("Server period period too small for task", name);
  };

  virtual ~ResourceReservationTaskDescriptor(){};

  ///@brief Returns the budget
  ///
  ///@return current budget
  int get_budget() const { return Q; };
  ///@brief Returns the server period
  int get_server_period() const { return Ts; };

  ///@brief Sets the server budget
  ///
  ///@param Qd desired budget. AN exception is thrown if the
  /// bandwidth exceeds 1.0
  void set_budget(unsigned int Qd) throw(PrositAux::Exc) {
    if (double(Qd) / Ts > 1.0)
      EXC_PRINT_2("budget too large for object", name);
    Q = Qd;
  };
  ///@brief Sets the deadline step
  /*! It overrides the standard definition of deadline step
   * requiring the additional constraint that it be a mutliple of the server
   * period. It can be freely reset to zero at any time.
    @param ds required deadline step.
   */
  virtual void set_deadline_step(unsigned int ds);

  ///@brief Sets the server period
  ///
  ///@param Tsd desired period. AN exception is thrown if the
  /// bandwidth exceeds 1.0 or if the server period is not a sub multiple
  /// of current deadline step.
  int set_server_period(unsigned int Tsd) {
    if (double(Q) / double(Tsd) > 1.0)
      EXC_PRINT_2("Server period too small for task ", name);
    if ((deadline_step != 0) && (!(deadline_step % Tsd)))
      EXC_PRINT_2("Deadline step has to be a multiple of server period for ",
                  name);
    Ts = Tsd;
  };
};
};
#endif