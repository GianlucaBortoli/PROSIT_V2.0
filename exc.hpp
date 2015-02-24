/*
* Header file for the exception mechanism
*
* Copyright (C) 2013, University of Trento
* Authors: Luigi Palopoli <palopoli@disi.unitn.it>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/
#ifndef EXC_HPP
#define EXC_HPP

#include <iostream>
#include <string>
/// @file exc.hpp
///
/// The file contains classes and macros for exception handling
/// The key element of the exception is the dmap_exc::Exc class.
/// The exception boject carries a string where the programmer can store
/// useful information on the exceptional event that occurred.
/// mtsdepth policy is to throw exception whenever an error situation
/// occurs that cannot be managed by standard means.
/// In addition the file provides a couple of Macros that can be used
/// to print a user defined message wherever an exception is thrown.
/// If NDEBUG is enables the macros can still be used to insert
/// file, line and fucntion where the exception occurred.
using namespace std;

namespace PrositAux {
///@brief Class for throwing exceptions.
///
/// This class is used all over the project to throw exception
class Exc {
  string msg;

public:
  /// @brief Constructor
  ///
  /// @param ms error message
  Exc(const char *ms) : msg(ms){};
  Exc(string s) : msg(s){};
  /// @brief Prints out exception message
  ///
  /// Prints out exception message stored in
  ///  the exception object to cerr
  ///
  virtual void what() { cerr << msg << endl; };
};

/// @brief Macro to print out error message and throw exception at the same time
///
/// If NDEBUG is defined it the macro simply throws the exception.
/// Otherwise, it prints out the msg, along with file, line and function
/// where the exception occurred.
/// @param msg error message
#ifdef NDEBUG
#define EXC_PRINT(msg)                                                         \
  throw dmap_exc::Exc(string(__PRETTY_FUNCTION__) + string(": ") + string(msg))
#else
#define EXC_PRINT(msg)                                                         \
  do {                                                                         \
    cerr << "file:" << __FILE__ << ", line:" << __LINE__                       \
         << ", function: " << __PRETTY_FUNCTION__ << ". " << msg << endl;      \
    throw PrositAux::Exc(string(__PRETTY_FUNCTION__) + string(": ") +          \
                         string(msg));                                         \
  } while (0)
#endif

/// @brief Macro to print two error messages and throw exception (with the
/// message stored) at the same time
///
/// If NDEBUG is defined it the macro simply throws the exception.
/// Otherwise, it prints out the msg, msg1 along with file, line and function
/// where the exception occurred.
/// @param msg error message
/// @param msg1 second error message
#ifdef NDEBUG
#define EXC_PRINT_2(msg, msg1)                                                 \
  throw dmap_exc::Exc(string(__PRETTY_FUNCTION__) + string(": ") +             \
                      string(msg) + string(" ") + string(msg1))
#else
#define EXC_PRINT_2(msg, msg1)                                                 \
  do {                                                                         \
    cerr << "file:" << __FILE__ << ", line:" << __LINE__                       \
         << ", function: " << __PRETTY_FUNCTION__ << ". " << msg << " "        \
         << msg1 << endl;                                                      \
    throw PrositAux::Exc(string(__PRETTY_FUNCTION__) + string(": ") +          \
                         string(msg) + string(" ") + string(msg1));            \
  } while (0)
#endif
};
#endif