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

Console console;

class ConsoleStreambuf : public std::streambuf
{
public:
  ConsoleStreambuf(Console* console_)
    : console(console_)
  {
    setp(buf, buf+sizeof(buf));
  }

  ~ConsoleStreambuf()
  {
    sync();
  }

protected:
  virtual int overflow(int c)
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

  virtual int sync()
  {
    return overflow(traits_type::eof());
  }

private:
  Console* console;
  char buf[1024];
};

//-------------------------------------------------------------------------------

class ConsoleImpl
{
public:
  Console& console;

  struct ConsoleEntry {
    std::string message;
    float display_time;
  };

  int x_pos;
  int y_pos;
  typedef std::vector<ConsoleEntry> Buffer;
  Buffer buffer;

  std::string command_line;
  bool active;
  std::vector<std::string> history;
  int history_position;

  int cursor_pos;

  int scroll_offset;
  ConsoleEntry current_entry;

  ConsoleImpl(Console& console_)
    : console(console_)
  {
    x_pos = 16;
    y_pos = 600-16;

    active = false;
    history_position = 1; 

    scroll_offset = 0;
    cursor_pos  = 0;   
  }

  void draw();
  void update(float delta);
  void eval_command_line();
  void tab_complete() {}

  /** adds a newline if the current line contains content */
  void maybe_newline();

  void add(char* buf, int len)
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
};

//-------------------------------------------------------------------------------

void
ConsoleImpl::draw()
{
  int y = y_pos;

  if (active)
    y -= Fonts::copyright.get_height() + 2;

  Fonts::copyright.set_alignment(origin_bottom_left);

  int num_lines = 600 / (Fonts::copyright.get_height() + 2);

  if (console.is_active())
    CL_Display::fill_rect(CL_Rect(0,0, 800, 600),
                          CL_Color(0, 0, 0, 60));

  for(int i = buffer.size()-1 - scroll_offset; i >= 0 && i > int(buffer.size()) - num_lines - scroll_offset; --i)
    {
      if (buffer[i].display_time < 5.0f || console.is_active())
        {
          Fonts::copyright.set_color(CL_Color(225, 225, 255));
          if (buffer[i].display_time > 4.0f && !console.is_active())
            Fonts::copyright.set_alpha(1.0f - (buffer[i].display_time - 4.0f));
          else
            Fonts::copyright.set_alpha(1.0f);

          Fonts::copyright.draw(x_pos, y, buffer[i].message);
        }
      y -= Fonts::copyright.get_height() + 2;
    }

  Fonts::copyright.set_color(CL_Color(255, 255, 255));
  if (active)
    {
      std::string str = command_line;
      if (int(game_time*1000) % 400 > 200)
        {
          if (cursor_pos < int(str.size()))
            str[cursor_pos] = '_';
          else
            str += "_";
        }

      Fonts::copyright.set_alignment(origin_bottom_left);
      Fonts::copyright.set_alpha(1.0f);
      Fonts::copyright.draw(x_pos, y_pos, ">" + str);
    }
  
  //needed because ClanLib font operator= doesn't deal with uniqueness properly, so we need to
  //reset it again
  Fonts::copyright.set_alpha(1.0f);
}

void
ConsoleImpl::update(float delta)
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
                  if (cursor_pos == int(command_line.size()))
                    {
                      command_line += (char)(*i).keyboard.code;
                      cursor_pos += 1;
                    }
                  else
                    {
                      command_line.insert(cursor_pos, std::string(1, (char)(*i).keyboard.code));
                      cursor_pos += 1;
                    }
                }
              else if ((*i).keyboard.key_type == KeyboardEvent::SPECIAL)
                {
                  switch (i->keyboard.code)
                    {
                    case CL_KEY_BACKSPACE:
                      if (!command_line.empty() && cursor_pos > 0)
                        {
                          command_line.erase(cursor_pos - 1, 1);
                          cursor_pos -= 1;
                        }
                      break;

                    case CL_KEY_DELETE:
                      if (!command_line.empty())
                        {
                          command_line.erase(cursor_pos, 1);
                        }
                      break;

                    case CL_KEY_DOWN:
                      if (!history.empty())
                        {
                          history_position += 1;
                          if (history_position > int(history.size())-1)
                            history_position = int(history.size())-1;
                          command_line = history[history_position];
                          cursor_pos = command_line.size();
                        }
                      break;

                    case CL_KEY_HOME:
                      console.scroll(10);
                      break;

                    case CL_KEY_END:
                      console.scroll(-10);
                      break;

                    case CL_KEY_TAB:
                      tab_complete();
                      break;

                    case CL_KEY_UP:
                      if (!history.empty())
                        {
                          history_position -= 1;
                          if (history_position < 0)
                            history_position = 0;

                          command_line = history[history_position];
                          cursor_pos = command_line.size();
                        }
                      break;

                    case CL_KEY_LEFT:
                      cursor_pos -= 1;
                      if (cursor_pos < 0)
                        cursor_pos = 0;
                      break;

                    case CL_KEY_RIGHT:
                      cursor_pos += 1;
                      if (cursor_pos > int(command_line.size()))
                        cursor_pos = command_line.size();
                      break;

                    case CL_KEY_ENTER:
                      eval_command_line();
                      break;

                    case CL_KEY_F1:
                      console.deactive();
                      break;
                    }
                }
            }
        }
    }
}

void
ConsoleImpl::maybe_newline()
{
  console << std::flush;
  if (!current_entry.message.empty())
    {
      console << std::endl;
    }
}

void
Console::scroll(int lines)
{
  impl->scroll_offset += lines;
  
  if (impl->scroll_offset < 0)
    impl->scroll_offset = 0;
  else if (impl->scroll_offset >= int(impl->buffer.size()))
    impl->scroll_offset = impl->buffer.size()-1;
}

void
ConsoleImpl::eval_command_line()
{
  if (history.empty() || history.back() != command_line)
    {
      history.push_back(command_line);
      history_position = history.size();
    }
                      
  console << ">" << command_line << std::endl;

  if (command_line == "quit" || command_line == "exit")
    {
      console.deactive();
    }
  else if (command_line == "help")
    {
      console << "This is a script console, can enter stuff in here that will then be evaluated.\n"
              << "Type 'quit' to exit the console." << std::endl;
    }
  else if (command_line == "reset")
    {
      GameSession::current()->set_sector("levels/newformat2.wst");
    }
  else if (command_line == "show")
    {
      HSQUIRRELVM v = script_manager->get_vm();

      int size = sq_getsize(v, -1);
      console << size << " elements on the root table" << std::endl;

      sq_pushroottable(v);

      //push your table/array here
      sq_pushnull(v);  //null iterator
      while(SQ_SUCCEEDED(sq_next(v,-2)))
        {
          //here -1 is the value and -2 is the key
          const SQChar *s;
          if (SQ_SUCCEEDED(sq_getstring(v,-2, &s)))
            {
              console << s << " -> ";
                                  
              sq_pushroottable(v);
              sq_pushstring(v,"print",-1);
              sq_get(v,-2); //get the function from the root table
                                  
              sq_pushroottable(v); //'this' (function environment object)
              sq_push(v,-4);
              sq_call(v,2,SQFalse);
                                  
              sq_pop(v,2); //pops the roottable and the function

              console << std::endl;;
            }
          else
            {
              console << "Unknown key type for element" << std::endl;
            }
                              
          sq_pop(v,2); //pops key and val before the nex iteration
        }
                          
      sq_pop(v, 1);
    }
  else
    {
      GameSession::current()->execute(command_line);
      maybe_newline();
    }
  command_line = "";
  cursor_pos = 0;
}
//-------------------------------------------------------------------------------

Console::Console()
  : std::ostream(new ConsoleStreambuf(this)),
    impl(new ConsoleImpl(*this))
{
}

void
Console::activate()
{
  // Get rid of all input events so that we don't double press
  InputManager::clear();
  impl->active = true;
}

void
Console::deactive()
{
  impl->active = false;
}

bool
Console::is_active() const
{
  return impl->active;
}

void
Console::draw()
{
  impl->draw();
}

void
Console::update(float delta)
{
  impl->update(delta);
}

void
Console::add(char* buf, int len)
{
  impl->add(buf, len);
}

/* EOF */
