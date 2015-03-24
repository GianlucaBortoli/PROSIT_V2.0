#ifndef QOS_FUNCTION_HPP
#define QOS_FUNCTION_HPP

#include "exc.hpp"

namespace PrositCore {
class QosFunction {
  double scale; /**< Slope fo the linear mapping */
  double pmin; /**< Lower bound for the probability of deadline hit (quality offset below) */
  double pmax; /**< Upper bound for the probability of deadline hit (quality saturates above) */
  double offset; /**< Minimum value for the quality */
public:
  QosFunction(double scaled, double pmind, double pmaxd, double offsetd=0.95) 
    : scale(scaled), pmin(pmind), pmax(pmaxd), offset(offsetd) {
    if((pmind > pmaxd) || (pmind < 0) || 
       (pmind > 1.0)   || (pmaxd < 0) ||
       (pmaxd > 1.0)) {
      EXC_PRINT("Wrong probability limits");
    }
    if(scaled < 0) {
      EXC_PRINT("Wrong scaling constant");
    }
  }
  //! Function returning the quality
  /*! \param prob is the probability of respecting the deadline. In
   *  a different implementation could easily be a vector of probabilities.
   */
  double eval(double prob);
};
}
#endif