#include "task_descriptor.hpp"

namespace PrositCore {
void GenericTaskDescriptor::insert_deadline(DeadlineUnit deadline) {
  if (deadline_step == 0)
    EXC_PRINT_2("Deadline inserted before defining the step for task ", name);

  pair<unsigned int, double> entry(deadline, 0.0);
  if (!(probabilistic_deadlines.insert(entry).second))
    EXC_PRINT_2("cannot create deadline for task ", name);
  return;
}

void GenericTaskDescriptor::compute_probability() {
  if (!probability_solver)
    EXC_PRINT_2("Probability solver unset for task ", name);
  if (probabilistic_deadlines.empty())
    EXC_PRINT_2("No deadline specified for task ", name);
	if (solved)
    return;

  probability_solver->solve();
}

double GenericTaskDescriptor::get_probability(DeadlineUnit deadline) {
  if (!probability_solver)
    EXC_PRINT("Task is not linked to any solver");
  
  DeadlineProbabilityMapIter it = probabilistic_deadlines.find(deadline);

  if (!probability_solver->is_solved()) {
    if (verbose_flag)
      cout << "Probability requested for unsolved task. Now solving..." << endl;
    compute_probability();
  }

  if (it == probabilistic_deadlines.end())
    EXC_PRINT_2("Deadline does not exist for task ", name);

  return it->second;
}

void GenericTaskDescriptor::set_solver(ProbabilitySolver *psd) {
  probability_solver = psd;
  solved = false;
  probability_solver->register_task(this);
}

void GenericTaskDescriptor::set_deadline_step(unsigned int ds) {
  if ((deadline_step != 0) && (!probabilistic_deadlines.empty()))
    EXC_PRINT_2("Deadline step reset in presence of probabilistic deadlines for task", name);
  deadline_step = ds;
  return;
}

void GenericTaskDescriptor::set_delta(unsigned int d){
  delta = d;
}

void GenericTaskDescriptor::set_step(unsigned int s){
  step = s;
}

void ResourceReservationTaskDescriptor::set_deadline_step(unsigned int ds) {
  if ((ds != 0) && ((ds % Ts) > 1))
    EXC_PRINT_2("Deadline step has to be a multiple of the server period for task", name);
  GenericTaskDescriptor::set_deadline_step(ds);
}

void ResourceReservationTaskDescriptor::display(GenericTaskDescriptor* td,
                                                const vector<double> &probability, 
                                                const vector<double> &quality, 
                                                const vector<long long> &time, 
                                                bool show_time,
                                                int index){
  ResourceReservationTaskDescriptor* t;
  if (!(t = dynamic_cast<ResourceReservationTaskDescriptor *>(td)))
    EXC_PRINT_2("Impossible to cast task GenericTaskDescriptor to ResourceReservationTaskDescriptor for task ", 
                td->get_name());
  
  printf("%20s%20d%20f%20f%20f", t->get_name().c_str(), 
                                 t->get_budget(),
                                 double(t->get_budget())/double(t->get_server_period()),
                                 probability[index], 
                                 quality[index]);

  if(show_time)
    printf("%*llu\n",20,time[index]);
  else
    printf("\n");

  t->Btot = double(t->get_budget()) / double(t->get_server_period());
  t->inf_norm = min<double>(quality[index], t->inf_norm);
}

}