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

#include <iostream>
#include "input_manager_player.hxx"

InputManagerPlayer::InputManagerPlayer(const std::string& filename)
{
  (void) filename;
#if 0
  // FIXME
  std::cout << "InputManagerPlayer::InputManagerPlayer(" << filename << ")" << std::endl;
  entry_counter = 0;
  lisp_object_t* port = lisp_read_from_file(filename.c_str());
  lisp_object_t* entry;
  while(scm_eof_object_p(entry = scm_read(port)) == lisp_object_t*_BOOL_F)
    {
      InputEventLst lst;
      int entry_num = lisp_scm2int(lisp_cadr(entry));
      entry = lisp_cddr(entry);
      
      while(lisp_cons_p(entry))
        {
          lst.push_back(scm2event(lisp_car(entry)));
          entry = lisp_cdr(entry);
        }
      entries.push(Entry(entry_num, lst));
    }
  scm_close_port(port);
#endif 
}

InputEvent
InputManagerPlayer::scm2event(const lisp::Lisp* )
{
  InputEvent event;
#if 0
  lisp_object_t* sym  = lisp_car(entry);
  lisp_object_t* data = lisp_cdr(entry);

  if (strcmp("axis", lisp_symbol(sym)) == 0)
    {
      event.type = AXIS_EVENT;
      event.axis.name = lisp_integer(lisp_list_nth(data, 0));
      event.axis.pos  = lisp_real   (lisp_list_nth(data, 1));
    } 
  else if (strcmp("button", lisp_symbol(sym)) == 0)
    {
      event.type = BUTTON_EVENT;
      event.button.name = lisp_integer(lisp_list_nth(data, 0));
      event.button.down = lisp_real   (lisp_list_nth(data, 1));
    } 
  else 
    {
      std::cout << "scm2event: Unknown sym: " << std::endl; //Guile::scm2string(sym) << std::endl;
    }
#endif
  return event;
}
  
void
InputManagerPlayer::update(float delta)
{
  (void) delta;
  if (entries.front().entry_num == entry_counter)
    {
      events = entries.front().events;
      
      for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
        {
          if (i->type == AXIS_EVENT)
            controller.set_axis_state(i->axis.name, i->axis.pos);
          else if  (i->type == BUTTON_EVENT)
            controller.set_button_state(i->button.name, i->button.down);
        }
      entries.pop();
    }

  entry_counter += 1;
}

/* EOF */
