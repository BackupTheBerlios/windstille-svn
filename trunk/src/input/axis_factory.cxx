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
#include "../windstille_error.hxx"
#include "button_factory.hxx"
#include "button_axis.hxx"
#include "axis_factory.hxx"

InputAxis* 
AxisFactory::create(lisp_object_t* lst)
{
  while(lisp_cons_p(lst))
    {
      lisp_object_t* sym  = lisp_car(lst);
      lisp_object_t* data = lisp_cdr(lst);
      
      if (strcmp(lisp_symbol(sym), "joystick-axis") == 0)
        {
          return create_joystick_axis(data);
        }
      else if (strcmp(lisp_symbol(sym), "button-axis") == 0)
        {
          return create_button_axis(data);
        }
      else
        {
          throw WindstilleError("AxisFactory::create: parse error");
        }

      lst = lisp_cdr(lst);
    }
  return 0;
}

InputAxis*
AxisFactory::create_joystick_axis(lisp_object_t* lst)
{
  int device_num = lisp_integer(lisp_list_nth(lst, 0));
  int axis_num   = lisp_integer(lisp_list_nth(lst, 1));

  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputAxisInputDevice(CL_Joystick::get_device(device_num), axis_num);
  else
    {
      throw WindstilleError("Error: AxisFactory::create_joystick_axis: ");
      //                            + lisp_string(lst));
      return 0;
    }
}

InputAxis*
AxisFactory::create_button_axis(lisp_object_t* lst)
{
  InputButton* left  = ButtonFactory::create(lisp_list_nth(lst, 0));
  InputButton* right = ButtonFactory::create(lisp_list_nth(lst, 1));

  return new ButtonAxis(left, right);
}

/* EOF */
