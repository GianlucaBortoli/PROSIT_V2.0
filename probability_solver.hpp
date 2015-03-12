#ifndef PROBABILITY_SOLVER_HPP
#define PROBABILITY_SOLVER_HPP

#include "task_descriptor.hpp"
#include <map>
#include <list>

///@file probability_solver.hpp
///
/// This file contains the abstract definitions for
/// probability solvers. In essence, a probability solver wraps
/// the different algorithms used to compute probabilistic deadlines.
/// Specific interfaces are also provided for resource reservations and
/// fixed priority. A solver for resource reservation is one to one linked
/// with a task, whereas a solver for fixed priority considers sets of tasks.

namespace PrositCore {
// forward declaration
class GenericTaskDescriptor;
class ResourceReservationTaskDescriptor;
class FixedPriorityTaskDescriptor;

///@brief root of the hiearachy. Essentially this is class defines an interface.
class ProbabilitySolver {
protected:
  bool solved; /*!< Has the solver been already called on the current
                  configuration of tasks? */
  bool pre_process_done;
  bool post_process_done;
  bool
      verbose_flag; /*!< True if any of the task linked is has verbose_flag set
                       */
  bool linked_flag; /*!< True if the solver has been linked with at least one
                       task */
  ///@brief Prepare computation
  virtual void pre_process() = 0;
  virtual void apply_algorithm() = 0;
  ///@brief Post processing after solution
  virtual void post_process() = 0;
  ///@brief Fills in the probability map
  virtual void fill_in_probability_map() = 0;
  ///@brief Is everything ok to start pre_processing?
  virtual bool check_list() = 0;

public:
  ///@brief Computes the steady state probability of meeting a set of deadlines
  ///
  /// This function is supposed to have access to the internal information of
  /// the
  /// task descriptor it applies to. The result of the call is to fill in
  /// the proabilistic_deadline structure within the associated task
  /// descriptors.
  virtual void solve();
  ///@brief Adds the task to the set of tasks the solver applies to
  ///
  /// This method is pure virtual so that each solver can verify the
  /// appropriateness
  /// of the task. The call to this method resets previous computations.
  /// It also sets the verbose flag.
  ///@param td descriptor of the task
  virtual void register_task(GenericTaskDescriptor *td) = 0;
  ///@brief Default constructor
  ProbabilitySolver()
      : solved(false), pre_process_done(false), post_process_done(false),
        verbose_flag(false), linked_flag(false){};
  ///@brief Destructor
  ///
  /// The destructor is virtual to enable subclassing
  virtual ~ProbabilitySolver(){};
  ///@brief resets the solver to make it read for a new execution
  virtual void reset() {
    solved = false;
    pre_process_done = false;
    post_process_done = false;
  };
  ///@brief returns the state of the solved flag
  bool is_solved() const { return solved; };
};

///@brief Interface for generic solvers that apply to a resource reservation
///task.
class ResourceReservationProbabilitySolver : public ProbabilitySolver {
protected:
  ResourceReservationTaskDescriptor *task_descriptor; /*!< Descriptor of only
                                                         task managed by this
                                                         type of solver >*/
public:
  ///@brief Associates the solver with one task descriptor
  ///
  /// A resource reservation solver applies to a resource reservation task
  /// descriptor.
  ///@param td descriptor of the task.
  ///@result An exception is thrown if the task descriptor
  /// does not correspond to a resource reservation task.
  virtual void register_task(GenericTaskDescriptor *td);

  bool check_list();
  ///@brief Default constructor
  ResourceReservationProbabilitySolver()
      : ProbabilitySolver(), task_descriptor(0){};
  ///@brief destructor
  virtual ~ResourceReservationProbabilitySolver(){};
};

///@brief Interface for generic solvers that apply to a set of fixed priority
///tasks
/// TO BE IMPLEMENTED
class FixedPriorityProbabilitySolver : public ProbabilitySolver {
protected:
  std::map<unsigned int,
           std::list<FixedPriorityTaskDescriptor *>> task_set; /*!< Task
                                                                  descriptors
                                                                  are ordered
                                                                  according to
                                                                  theri
                                                                  scheduling
                                                                  priority */
public:
  ///@brief Default constructor
  FixedPriorityProbabilitySolver() : ProbabilitySolver(), task_set(){};
  ///@brief Default destructor
  virtual ~FixedPriorityProbabilitySolver(){};
  ///@brief Associates the solver with one task descriptor
  ///
  /// A resource reservation solver applies to a resource reservation task
  /// descriptor.
  ///@param td descriptor of the task. An exception is thrown id
  virtual void register_task(GenericTaskDescriptor *td);
};
}
#endif