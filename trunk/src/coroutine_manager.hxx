//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_COROUTINE_MANAGER_HXX
#define HEADER_COROUTINE_MANAGER_HXX

#include <ruby.h>
#include "ruby_object.hxx"
#include <vector>

class Coroutine
{
public:
  enum COStatus { CO_WAITING, CO_RUNNING, CO_FINISHED }; 

  /** Run the given Cooroutine*/
  virtual void update()  =0;
  virtual void restart() =0;
  virtual COStatus status()  =0;
};

class RubyCoroutine : public Coroutine
{
private:
  RubyObject value;
public:
  RubyCoroutine(const RubyObject& value_);
  virtual ~RubyCoroutine();
  
  void update();
  void restart();
  COStatus status();
};

/** */
class CoroutineManager
{
private:
  static CoroutineManager* current_;
public:
  static CoroutineManager* current() { return current_; }
private:
  typedef std::vector<Coroutine*> Coroutines;
  Coroutines coroutines;
  Coroutines new_coroutines;
public:
  CoroutineManager();

  void update(float delta);
  void add(Coroutine* co);
private:
  CoroutineManager (const CoroutineManager&);
  CoroutineManager& operator= (const CoroutineManager&);
};

#endif

/* EOF */
