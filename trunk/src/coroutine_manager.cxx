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

#include <algorithm>
#include <iostream>
#include "coroutine_manager.hxx"

CoroutineManager* CoroutineManager::current_ = 0;

CoroutineManager::CoroutineManager()
{
  current_ = this;
}

void
CoroutineManager::update(float delta)
{
  if (!new_coroutines.empty())
    {
      for(Coroutines::size_type i = 0; i != new_coroutines.size(); ++i)
        {
          std::cout << "XXX: pushing back coroutine" << std::endl;
          coroutines.push_back(new_coroutines[i]);
        }
      new_coroutines.clear();
    }

  for(Coroutines::size_type i = 0; i != coroutines.size(); ++i)
    {
      if (coroutines[i] && coroutines[i]->status() == Coroutine::CO_WAITING)
        {
          coroutines[i]->update();
        }
      else
        {
          coroutines[i] = 0;
        }
    }
  
  Coroutines::iterator ret
    = std::remove(coroutines.begin(), coroutines.end(), static_cast<Coroutine*>(0));
  if (ret != coroutines.end())
    coroutines.erase(ret);
}

void
CoroutineManager::add(Coroutine* co)
{
  std::cout << "XXX: New coroutine" << std::endl;
  new_coroutines.push_back(co);
}

RubyCoroutine::RubyCoroutine(const RubyObject& value_)
  : value(value_)
{
  
}

RubyCoroutine::~RubyCoroutine()
{
}

void
RubyCoroutine::update()
{
  rb_funcall(value.ptr(), rb_intern("update"), 0);
}

void
RubyCoroutine::restart()
{
  rb_funcall(value.ptr(), rb_intern("restart"), 0);
}

Coroutine::COStatus
RubyCoroutine::status()
{
  VALUE ret = rb_funcall(value.ptr(), rb_intern("status"), 0);

  char* sym = rb_str2cstr(rb_inspect(ret), 0);
  if (strcmp(sym, ":waiting") == 0)
    {
      return CO_WAITING;
    } 
  else if (strcmp(sym, ":running") == 0) 
    {
      return CO_RUNNING;
    }
  else if (strcmp(sym, ":finished") == 0) 
    {
      return CO_FINISHED;
    }
  else
    {
      std::cout << "Error: RubyCoroutine: Unknown status: '" 
                << sym
                << "'" << std::endl;
      return CO_FINISHED;
    }
}

/* EOF */
