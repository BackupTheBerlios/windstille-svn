//  $Id: input_manager.cxx,v 1.4 2003/08/20 00:15:10 grumbel Exp $
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

#include <iostream>
#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <ClanLib/Display/joystick.h>

#include "lisp/parser.hpp"
#include "lisp/lisp.hpp"
#include "lisp/properties.hpp"
#include "windstille_error.hpp"
#include "input_manager_custom.hpp"
#include "input_manager_player.hpp"
#include "input_manager_impl.hpp"
#include "input_recorder.hpp"
#include "input_manager.hpp"

InputManagerImpl* InputManager::impl = 0;
InputRecorder* InputManager::recorder = 0;

void
InputManager::init_playback(const std::string& filename)
{
  impl = new InputManagerPlayer(filename);
}

void
InputManager::init(const std::string& filename)
{
  std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));
  lisp::Properties rootp(root.get());

  const lisp::Lisp* controller;
  if(rootp.get("windstille-controller", controller) == false) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-controller file";
    throw std::runtime_error(msg.str());
  }
  
  impl = new InputManagerCustom(controller);
}

void
InputManager::setup_recorder(const std::string& filename)
{
  if (recorder)
    delete recorder;

  recorder = new InputRecorder(filename);
}

void 
InputManager::deinit()
{
  delete impl;
}

void
InputManager::update(float delta)
{
  assert(impl);
  impl->update(delta);
  if (recorder)
    recorder->record(get_controller());
}

Controller
InputManager::get_controller()
{
  assert(impl);
  return impl->get_controller();
}

void
InputManager::clear()
{
  impl->clear();
}

/* EOF */
