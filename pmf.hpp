/*
* Header file for probability distribution classes
*
* Copyright (C) 2013, University of Trento
* Authors: Luigi Palopoli <palopoli@disi.unitn.it>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/
#ifndef PMF_H
#define PMF_H

#include "exc.hpp"
#include "auxiliary_func.hpp"
#include <math.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <tinyxml2.h>

using namespace std;
using namespace tinyxml2;

#define EPS 1e-5
#define TYPICAL_SIZE 10000

namespace PrositAux {
class distr {
protected:
  VectorXd elems;
  unsigned int size;
  unsigned int offset;
  int min;
  int max;

public:
  double epsilon;
  distr(unsigned int sz = TYPICAL_SIZE, unsigned int offs = 0,
        double my_epsilon = EPS)
      : elems(sz), size(sz), offset(offs), min(sz - offs), max(-offs),
        epsilon(my_epsilon){};
  distr(const distr &p)
      : elems(p.elems), size(p.size), offset(p.offset), min(p.min), max(p.max),
        epsilon(p.epsilon){};
  virtual ~distr(){};

  const distr &operator=(const distr &p) {
    elems = p.elems;
    size = p.size;
    offset = p.offset;
    epsilon = p.epsilon;
    min = p.min;
    max = p.max;
    return *this;
  };

  int get_max() const { return max; };
  int get_min() const { return min; };
  unsigned int get_size() const { return size; };
  unsigned int get_offset() const { return offset; };
  int load(const char *filename) throw(Exc) { return load(string(filename)); };

  virtual void print() const {
    for (int i = get_min(); i <= get_max(); i++) {
      cout << i << ": " << get(i) << endl;
    }
  };

  virtual int set(int val, double p) throw(Exc) = 0;
  virtual double get(int el) const throw(Exc) = 0;
  virtual int load(const string &filename) throw(Exc) = 0;
};

//forward declaration
class pmf;
class cdf;
class beta;

class pmf : public distr {
  double tail;
public:
  typedef enum ERR_CODES {
    PMF_OK,
    PMF_NEG,
    PMF_SMALLER_ONE,
    PMF_GREATER_ONE
  } ERR_CODES;
  pmf(unsigned int sz = TYPICAL_SIZE, unsigned int offs = 0,
      double my_epsilon = EPS);
  pmf(const pmf &p) : distr::distr(p) { tail = p.tail; };
  ~pmf(){};
  double avg() const;
  double std() const;
  double var() const;
  int set_samples(int samples);
  double sum() const;
  ERR_CODES check() const;
  double get_tail() { return tail; };

  virtual double get(int el) const throw(Exc);
  virtual int set(int val, double p) throw(Exc);
  virtual int load(const string &filename) throw(Exc);
  ///@brief Creates a clone of the pmf, which is resampled at the requested
  ///granularity
  ///
  ///@param q granularity
  pmf *resample(int q) const;
  friend void pmf2cdf(const pmf &p, cdf &c);
  friend void cdf2pmf(const cdf &c, pmf &p);
};

class cdf : public distr {
  bool just_created;

public:
  typedef enum ERR_CODES {
    CDF_OK,
    CDF_BAD_MAX,
    CDF_BAD_MIN,
    CDF_NON_INCREASING
  } ERR_CODES;
  cdf(unsigned int sz = TYPICAL_SIZE, unsigned int offs = 0,
      double epsilon = EPS);
  cdf(const cdf &p) : distr::distr(p) { just_created = p.just_created; };
  ~cdf();

  ERR_CODES check() const;
  void finalise() throw(Exc);
  double get_epsilon() const { return epsilon; };

  virtual int set(int val, double p) throw(Exc);
  virtual double get(int el) const throw(Exc);
  virtual int load(const string &filename) throw(Exc);

  friend void pmf2cdf(const pmf &p, cdf &c);
  friend void cdf2pmf(const cdf &c, pmf &p);
};

class beta : public pmf {
  double a, b; //alpha & beta parameters for the Beta distribution respectively
  int b_min, b_max, b_step, b_size;

public:
  beta(){}
  beta(double alpha, double beta, int min, int max, int step, int size)
    : a(alpha), b(beta), b_min(min), b_max(max), b_step(step), b_size(size) {
      if(b_max < b_min){
        EXC_PRINT("Cmax smaller than cmin");
      }
      if((b_max-b_min)%b_step != 0){
        EXC_PRINT("Step has to be an integer submultiple of the interval");
      }
      if((a <= 1.0) || (b <= 1.0)){
        EXC_PRINT("Alpha & beta parameters have to be grater than 1.0");
      }
  }

  //TODO: these two function are nearly the same: can I merge it into a single one? Think about it

  ///Creates a beta probability function from the parameters in the XML file for the computation time
  ///@param e the input XML file
  PrositAux::pmf * create_beta_computation(XMLElement *e) throw (PrositAux::Exc);
  ///Creates a beta probability function from the parameters in the XML file for the interarrival time
  ///@param e the input XML file
  PrositAux::pmf * create_beta_interarrival(XMLElement *e) throw (PrositAux::Exc);
  
  virtual double get(int el) const throw(Exc);
  virtual int set(int val, double p) throw(Exc);
};

void pmf2cdf(const pmf &p, cdf &c);
void cdf2pmf(const cdf &c, pmf &p);
}
#endif