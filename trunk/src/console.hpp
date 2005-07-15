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

class ConsoleImpl;

class Console : public std::ostream
{
public:
  Console();

  void draw();
  void update(float delta);

  void activate();
  void deactive();
  bool is_active() const;

  void scroll(int lines);

  void add(char* buf, int len);

private:
  Console (const Console&);
  Console& operator= (const Console&);

  ConsoleImpl* impl;
};

extern Console console;

#endif

/* EOF */
