//  $Id: lisp.h 2419 2005-05-06 19:08:24Z matzebraun $
//
//  TuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Matthias Braun <matze@braunis.de>
//  code in this file based on lispreader from Mark Probst
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#ifndef __LISPREADER_HPP__
#define __LISPREADER_HPP__

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace lisp
{

class Lisp
{
public:
  ~Lisp();
    
  enum LispType {
    TYPE_CONS,
    TYPE_SYMBOL,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_REAL,
    TYPE_BOOLEAN
  };

  LispType get_type() const
  { return type; } 

  Lisp* get_car() const
  { return v.cons.car; }
  Lisp* get_cdr() const
  { return v.cons.cdr; }

  std::string get_string() const
  {
    std::string result;
    if(!get(result)) {
      std::ostringstream msg;
      msg << "Expected string, got ";
      print(msg);
      throw std::runtime_error(msg.str());
    }
    return result;
  }

  int get_int() const
  {
    int result;
    if(!get(result)) {
      std::ostringstream msg;
      msg << "Expected integer, got ";
      print(msg);
      throw std::runtime_error(msg.str());
    }
    return result;
  }

  float get_float() const
  {
    float result;
    if(!get(result)) {
      std::ostringstream msg;
      msg << "Expected float, got ";
      print(msg);
      throw std::runtime_error(msg.str());
    }
    return result;
  }

  bool get_bool() const
  {
    bool result;
    if(!get(result)) {
      std::ostringstream msg;
      msg << "Expected bool, got ";
      print(msg);
      throw std::runtime_error(msg.str());
    }
    return result;
  }

  template<class T>
  void get_vector(std::vector<T>& vec) const
  {
    vec.clear();
 
    for(const lisp::Lisp* child = this ; child != 0; child = child->get_cdr()) {
      if(type != TYPE_CONS)
        throw std::runtime_error("get_vector called on non-list");
      
      T val;
      if(!child->get_car())                           
        continue;
      if(child->get_car()->get(val)) {
        vec.push_back(val);
      }
    }
  }

  /** conveniance functions which traverse the list until a child with a
   * specified name is found. The value part is then interpreted in a specific
   * way. The functions return true, if a child was found and could be
   * interpreted correctly, otherwise false is returned and the variable value
   * is not changed.
   * (Please note that searching the lisp structure is O(n) so these functions
   *  are no good idea for performance critical areas)
   */
  template<class T>
  bool get(const char* name, T& val) const
  {
    const Lisp* lisp = get_lisp(name);
    if(!lisp)
      return false;

    if(lisp->get_type() != TYPE_CONS)
      return false;
    lisp = lisp->get_car();
    if(!lisp)
      return false;
    return lisp->get(val);
  }

  template<class T>
  bool get_vector(const char* name, std::vector<T>& vec) const
  {
    vec.clear();
    
    const Lisp* child = get_lisp(name);
    if(!child)
      return false;
    
    for( ; child != 0; child = child->get_cdr()) {
      T val;
      if(!child->get_car())
        continue;
      if(child->get_car()->get(val)) {
        vec.push_back(val);
      }
    }
    
    return true;
  }
  
  Lisp* get_lisp(const char* name) const;
  Lisp* get_lisp(const std::string& name) const
  { return get_lisp(name.c_str()); }

  // for debugging/error messages
  void print(std::ostream& out, int indent = 0) const;

private:
  bool get(std::string& val) const
  { 
    if(type != TYPE_STRING && type != TYPE_SYMBOL)
      return false;
    val = v.string;
    return true;
  }
  
  bool get(unsigned int& val) const
  {
    if(type != TYPE_INTEGER)
      return false;
    val = v.integer;
    return true;
  }
  
  bool get(int& val) const
  {
    if(type != TYPE_INTEGER)
      return false;
    val = v.integer;
    return true;
  }
  
  bool get(float& val) const
  {
    if(type != TYPE_REAL) {
      if(type == TYPE_INTEGER) {
        val = v.integer;
        return true;
      }
      return false;
    }
    val = v.real;
    return true;
  }
  
  bool get(bool& val) const
  {
    if(type != TYPE_BOOLEAN)
      return false;
    val = v.boolean;
    return true;
  }
 
  friend class Parser;
  Lisp(LispType newtype);

  LispType type;
  union
  {
    struct
    {
      Lisp* car;
      Lisp* cdr;
    } cons;

    char* string;
    int integer;
    bool boolean;
    float real;
  } v;
};

} // end of namespace lisp

#endif

