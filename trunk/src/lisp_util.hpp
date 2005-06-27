#ifndef __LISP_UTIL_HPP__
#define __LISP_UTIL_HPP__

#include "lisp/lisp.hpp"

static inline const lisp::Lisp* lisp_get_list_nth(const lisp::Lisp* lisp, int nth)
{
  for(int i = 0; i < nth; ++i) {
    if(lisp->get_type() != lisp::Lisp::TYPE_CONS)
      throw std::runtime_error("Invalid data when reading lisp list");
    if(lisp->get_cdr() == 0)
      throw std::runtime_error("lisp list too short");
    lisp = lisp->get_cdr();
  }
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS)
    throw std::runtime_error("Invalid data when reading lisp list");

  return lisp->get_car();
}

#endif

