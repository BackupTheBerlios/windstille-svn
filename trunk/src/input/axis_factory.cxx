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

#include <ClanLib/Display/joystick.h>
#include "input_axis_input_device.hxx"
#include "windstille_error.hxx"
#include "button_factory.hxx"
#include "button_axis.hxx"
#include "axis_factory.hxx"
#include "lisp_util.hpp"
#include "lisp/lisp.hpp"
#include "lisp/list_iterator.hpp"

InputAxis* 
AxisFactory::create(const lisp::Lisp* lisp)
{
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "joystick-axis") {
      return create_joystick_axis(iter.lisp());
    } else if(iter.item() == "button-axis") {
      return create_button_axis(iter.lisp());
    } else {
      std::cerr << "Skipping unknown tag '" << iter.item() << "' in axis.\n";
    }
  }

  return 0;
}

InputAxis*
AxisFactory::create_joystick_axis(const lisp::Lisp* lisp)
{
  int device_num = lisp_get_list_nth(lisp, 0)->get_int();
  int axis_num   = lisp_get_list_nth(lisp, 1)->get_int();

  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputAxisInputDevice(CL_Joystick::get_device(device_num), axis_num);
  else
    throw WindstilleError("Error: AxisFactory::create_joystick_axis: ");
}

InputAxis*
AxisFactory::create_button_axis(const lisp::Lisp* lisp)
{
  InputButton* left  = ButtonFactory::create(lisp_get_list_nth(lisp, 0));
  InputButton* right = ButtonFactory::create(lisp_get_list_nth(lisp, 1));

  return new ButtonAxis(left, right);
}

/* EOF */
