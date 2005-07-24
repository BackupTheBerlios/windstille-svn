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
#include <config.h>

#include <ClanLib/Display/joystick.h>
#include <sstream>
#include <stdexcept>
#include "input_axis_input_device.hpp"
#include "windstille_error.hpp"
#include "button_factory.hpp"
#include "button_axis.hpp"
#include "axis_factory.hpp"
#include "lisp/lisp.hpp"

InputAxis* 
AxisFactory::create(const lisp::Lisp* lisp)
{
  if(lisp->get_type() != lisp::Lisp::TYPE_LIST)
    throw std::runtime_error("expected list in axis argument");
  if(lisp->get_list_size() == 0)
    throw std::runtime_error("List needs at least 1 argument in axis");
  const lisp::Lisp* namel = lisp->get_list_elem(0);
  if(namel->get_type() != lisp::Lisp::TYPE_SYMBOL)
    throw std::runtime_error("axis needs symbol as first argument");

  std::string name = namel->get_string();
  if(name == "joystick-axis")
    return create_joystick_axis(lisp);
  else if(name == "button-axis")
    return create_button_axis(lisp);
  
  std::ostringstream msg;
  msg << "Unknown axis type '" << name << "' found";
  throw std::runtime_error(msg.str());
}

InputAxis*
AxisFactory::create_joystick_axis(const lisp::Lisp* lisp)
{
  if(lisp->get_list_size() != 3)
    throw std::runtime_error("joystick-axis needs 2 arguments");
  int device_num = lisp->get_list_elem(1)->get_int();
  int axis_num   = lisp->get_list_elem(2)->get_int();

  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputAxisInputDevice(CL_Joystick::get_device(device_num), axis_num);
  else
    throw WindstilleError("Error: AxisFactory::create_joystick_axis: ");
}

InputAxis*
AxisFactory::create_button_axis(const lisp::Lisp* lisp)
{
  if(lisp->get_list_size() != 3)
    throw std::runtime_error("button-axis needs 2 arguments");
  InputButton* left  = ButtonFactory::create(lisp->get_list_elem(1));
  InputButton* right = ButtonFactory::create(lisp->get_list_elem(2));

  return new ButtonAxis(left, right);
}

/* EOF */
