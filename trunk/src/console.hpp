//  $Id: console.hpp,v 1.2 2003/06/08 15:49:00 grumbel Exp $
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

#ifndef CONSOLE_HXX
#define CONSOLE_HXX

#include <vector>
#include <sstream>

struct ConsoleEntry {
  std::string message;
  float display_time;
};

class Console;

class ConsoleStreambuf : public std::streambuf
{
public:
  ConsoleStreambuf(Console* console);
  ~ConsoleStreambuf();

protected:
  virtual int overflow(int c);
  virtual int sync();

private:
  Console* console;
  char buf[1024];
};

/** */
class Console : public std::ostream
{
private:
  int x_pos;
  int y_pos;
  typedef std::vector<ConsoleEntry> Buffer;
  Buffer buffer;
  static Console* current_;
  std::string command_line;
  bool active;
  std::vector<std::string> history;
  int history_position;
  ConsoleEntry current_entry;
public:
  static Console& current();

  Console(int x, int y);

  void add(char* buf, int len);

  void draw();
  void update(float delta);

  void activate();
  void deactive();
  bool is_active() const;
private:
  Console (const Console&);
  Console& operator= (const Console&);
};

#endif

/* EOF */
