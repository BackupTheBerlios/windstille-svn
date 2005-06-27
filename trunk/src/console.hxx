//  $Id: console.hxx,v 1.2 2003/06/08 15:49:00 grumbel Exp $
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

/** */
class Console
{
private:
  int x_pos;
  int y_pos;
  typedef std::vector<ConsoleEntry> Buffer;
  Buffer buffer;
  static Console* current_;
  std::string command_line;
public:
  static Console* current();

  Console(int x, int y);
  
  void add(const std::string&);
  template<class A, class B>
  void add(const A& a, const B& b) {
    std::ostringstream s;
    s << a << b;
    add(s.str());
  }

  template<class A, class B, class C>
  void add(const A& a, const B& b, const C& c) {
    std::ostringstream s;
    s << a << b << c;
    add(s.str());
  }

  void draw();
  void update(float delta);
private:
  Console (const Console&);
  Console& operator= (const Console&);
};

#endif

/* EOF */
