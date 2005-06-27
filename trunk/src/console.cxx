//  $Id: console.cxx,v 1.3 2003/06/08 15:49:00 grumbel Exp $
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

#include <ClanLib/Display/keys.h>
#include <ClanLib/Display/keyboard.h>
#include <ClanLib/core.h>
#include "fonts.hxx"
#include "game_session.hxx"
#include "input/input_manager.hxx"
#include "console.hxx"

Console* Console::current_ = 0;

Console::Console(int arg_x, int arg_y)
{
  current_ = this;
  x_pos = arg_x;
  y_pos = arg_y;
  active = false;
}

Console* 
Console::current()
{
  assert(current_);
  return current_;
}

void
Console::add(const std::string& str)
{
  ConsoleEntry entry;
  entry.display_time = 0;
  entry.message = str;
  buffer.push_back(entry);
}

void
Console::draw()
{
  CL_Font font = Fonts::copyright;
  int y = y_pos;

  if (active)
    y -= font.get_height() + 2;

  font.set_alignment(origin_bottom_left);
  for(Buffer::reverse_iterator i = buffer.rbegin(); i != buffer.rend(); ++i)
    {
      if (i->display_time < 5.0f)
        {
          if (i->display_time > 4.0f)
            font.set_alpha(1.0f - (i->display_time - 4.0f));
          else
            font.set_alpha(1.0f);

          font.draw(x_pos, y, i->message);
        }
      y -= font.get_height() + 2;
    }

  if (active)
    {
      font.set_alignment(origin_bottom_left);
      font.set_alpha(1.0f);
      font.draw(x_pos, y_pos, ">" + command_line);
    }
}

void
Console::update(float delta)
{
  for(Buffer::iterator i = buffer.begin(); i != buffer.end(); ++i)
    {
      i->display_time += delta;
    }  

  if (active)
    {
      InputEventLst events = InputManager::get_controller().get_events();
  
      for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
        {
          if ((*i).type == KEYBOARD_EVENT)
            {
              if ((*i).keyboard.key_type == KeyboardEvent::LETTER)
                {
                  //std::cout << "Key: '" << (char)((*i).keyboard.code) << "' " << (*i).keyboard.code << std::endl;
                  command_line += (char)(*i).keyboard.code;
                }
              else if ((*i).keyboard.key_type == KeyboardEvent::SPECIAL)
                {
                  switch (i->keyboard.code)
                    {
                    case CL_KEY_BACKSPACE:
                      if (!command_line.empty())
                        command_line = command_line.substr(0, command_line.size() - 1);
                      break;

                    case CL_KEY_ENTER:
                      add(">" + command_line);
                      if (command_line == "quit" || command_line == "exit")
                        {
                          deactive();
                        }
                      else if (command_line == "help")
                        {
                          add("This is a script console, can enter stuff in here that will then be evaluated.");
                          add("Type 'quit' to exit the console.");
                        }
                      else if (command_line == "reset")
                        {
                          GameSession::current()->set_sector("levels/newformat2.wst");
                        }
                      command_line = "";
                      break;

                    case CL_KEY_F1:
                      deactive();
                      break;
                    }
                }
            }
        }
    }
}

void
Console::activate()
{
  // Get rid of all input events so that we don't double press
  InputManager::clear();
  active = true;
}

void
Console::deactive()
{
  active = false;
}

bool
Console::is_active() const
{
  return active;
}

/* EOF */
