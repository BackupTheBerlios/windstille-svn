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

#include <assert.h>
#include <ClanLib/Display/keys.h>
#include <ClanLib/Display/keyboard.h>
#include <ClanLib/core.h>
#include "fonts.hpp"
#include "game_session.hpp"
#include "input/input_manager.hpp"
#include "script_manager.hpp"
#include "console.hpp"

Console* Console::current_ = 0;

ConsoleStreambuf::ConsoleStreambuf(Console* console_)
  : console(console_)
{
  setp(buf, buf+sizeof(buf));
}

ConsoleStreambuf::~ConsoleStreambuf()
{
  sync();
}

int
ConsoleStreambuf::overflow(int c)
{
  if(pbase() == pptr())
    return 0;

  size_t size = pptr() - pbase();
  console->add(pbase(), size);
      
  if(c != traits_type::eof()) {
    char str[1];
    str[0] = c;
    console->add(str, 1);
  }

  setp(buf, buf + size);

  return 0;
}

int
ConsoleStreambuf::sync()
{
    return overflow(traits_type::eof());
}

//-------------------------------------------------------------------------------

Console::Console(int arg_x, int arg_y)
  : std::ostream(new ConsoleStreambuf(this))
{
  current_ = this;
  x_pos = arg_x;
  y_pos = arg_y;
  active = false;
  history_position = 1;
}

Console&
Console::current()
{
  assert(current_);
  return *current_;
}

void
Console::add(char* buf, int len)
{
  current_entry.display_time = 0;

  for (int i = 0; i < len; ++i)
    {
      if (buf[i] == '\n')
        {
          buffer.push_back(current_entry);
          current_entry = ConsoleEntry();
        }
      else
        {
          current_entry.message += buf[i];
        }
    }
}

void
Console::draw()
{
  int y = y_pos;

  if (active)
    y -= Fonts::copyright.get_height() + 2;

  Fonts::copyright.set_alignment(origin_bottom_left);
  // FIXME: only display stuff that would end up on the screen
  for(Buffer::reverse_iterator i = buffer.rbegin(); i != buffer.rend(); ++i)
    {
      if (i->display_time < 5.0f || is_active())
        {
          Fonts::copyright.set_color(CL_Color(225, 225, 255));
          if (i->display_time > 4.0f && !is_active())
            Fonts::copyright.set_alpha(1.0f - (i->display_time - 4.0f));
          else
            Fonts::copyright.set_alpha(1.0f);

          Fonts::copyright.draw(x_pos, y, i->message);
        }
      y -= Fonts::copyright.get_height() + 2;
    }

  Fonts::copyright.set_color(CL_Color(255, 255, 255));
  if (active)
    {
      Fonts::copyright.set_alignment(origin_bottom_left);
      Fonts::copyright.set_alpha(1.0f);
      Fonts::copyright.draw(x_pos, y_pos, ">" + command_line);
    }
  
  //needed because ClanLib font operator= doesn't deal with uniqueness properly, so we need to
  //reset it again
  Fonts::copyright.set_alpha(1.0f);
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

                    case CL_KEY_DOWN:
                      if (!history.empty())
                        {
                          history_position += 1;
                          if (history_position > int(history.size())-1)
                            history_position = int(history.size())-1;
                          command_line = history[history_position];
                        }
                      break;

                    case CL_KEY_UP:
                      if (!history.empty())
                        {
                          history_position -= 1;
                          if (history_position < 0)
                            history_position = 0;

                          command_line = history[history_position];
                        }
                      break;

                    case CL_KEY_ENTER:
                      if (history.empty() || history.back() != command_line)
                        {
                          history.push_back(command_line);
                          history_position = history.size();
                        }
                      (*this) << ">" << command_line << std::endl;
                      if (command_line == "quit" || command_line == "exit")
                        {
                          deactive();
                        }
                      else if (command_line == "help")
                        {
                          (*this) << "This is a script console, can enter stuff in here that will then be evaluated.\n"
                                  << "Type 'quit' to exit the console." << std::endl;
                        }
                      else if (command_line == "reset")
                        {
                          GameSession::current()->set_sector("levels/newformat2.wst");
                        }
                      else if (command_line == "show")
                        {
                          HSQUIRRELVM v = script_manager->get_vm();

                          if (0)
                            {
                              int size = sq_getsize(v, -1);
                          
                              (*this) << size << " elements on the root table" << std::endl;
                            }

                          sq_pushroottable(v);

                          //push your table/array here
                          sq_pushnull(v);  //null iterator
                          while(SQ_SUCCEEDED(sq_next(v,-2)))
                            {
                              //here -1 is the value and -2 is the key
                              const SQChar *s;
                              if (SQ_SUCCEEDED(sq_getstring(v,-2, &s)))
                                {
                                  (*this) << s << " -> ";
                                  
                                  sq_pushroottable(v);
                                  sq_pushstring(v,"print",-1);
                                  sq_get(v,-2); //get the function from the root table
                                  
                                  sq_pushroottable(v); //'this' (function environment object)
                                  sq_push(v,-4);
                                  sq_call(v,2,SQFalse);
                                  
                                  sq_pop(v,2); //pops the roottable and the function

                                  (*this) << std::endl;;
                                }
                              else
                                {
                                  (*this) << "Unknown key type for element" << std::endl;
                                }
                              
                              sq_pop(v,2); //pops key and val before the nex iteration
                            }
                          
                          sq_pop(v, 1);
                        }
                      else
                        {
                          GameSession::current()->execute(command_line);
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
