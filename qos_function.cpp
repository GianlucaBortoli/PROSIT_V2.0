#include "qos_function.hpp"

namespace PrositCore {
  double QosFunction::eval(double prob) {
    if(prob <= pmin)
      return offset;
    else
      return scale * (pmax-pmin) + offset;
  }
}