#include "pmf.hpp"
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>

namespace PrositAux {
cdf::cdf(unsigned int sz, unsigned int offs, double my_epsilon)
    : distr(sz, offs, my_epsilon), just_created(true) {
  elems.setZero();
};

cdf::~cdf(){};

double cdf::get(int el) const throw(Exc) {
  // #ifdef DEBUG
  //  cerr<<"cdf::get("<<el<<") "<<elems(el+offset)<<endl;
  // #endif
  if (just_created)
    EXC_PRINT("function called on an empty cdf");

  if (el > max) {
    if (elems(max + offset) > 1.0 - epsilon)
      return 1.0;
    else
      EXC_PRINT("ill formed cdf");
  }

  if (el < min)
    return 0.0;

  return elems(el + offset);
};

int cdf::load(const string &filename) throw(Exc) {
  int j = distr::load(filename);
  if (check() != CDF_OK)
    EXC_PRINT_2("ill formed cdf", filename);

  return j;
}

cdf::ERR_CODES cdf::check() const {
  if (just_created)
    throw Exc("CDF::CHECK called on an empty cdf");
  if ((elems(max + offset) > 1 + epsilon) ||
      (elems(max + offset) < 1 - epsilon)) {
    cerr << "CDF::CHECK bad max" << endl;
    return CDF_BAD_MAX;
  }
  if ((min + offset > 0) && (elems(min + offset - 1) > epsilon)) {
    cerr << "CDF::CHECK bad min" << endl;
    return CDF_BAD_MIN;
  };

  for (int i = min + 1; i <= max; i++) {
    if (get(i - 1) > get(i))
      return CDF_NON_INCREASING;
  }

  return CDF_OK;
}

int cdf::set(int val, double p) throw(Exc) {
  if (val + offset > size)
    EXC_PRINT("insert out of range");

  if (val > max) {
    if ((!just_created) && (get(max) > p))
      EXC_PRINT("ill formed cdf: bad max");
    max = val;
  };

  if (val < min) {
    if ((!just_created) && (get(min) < p))
      EXC_PRINT("ill formed cdf: bad min");
    min = val;
  }

  if (just_created)
    just_created = false;
  elems(val + offset) = p;
  return val;
}
}