//  $Id: screen.hpp,v 1.3 2003/10/10 21:06:22 grumbel Exp $
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

#ifndef HEADER_SCREEN_HXX
#define HEADER_SCREEN_HXX

class CL_InputEvent;

namespace Windstille {

/** */
class Screen
{
private:
  void draw_fps(float delta);
  void key_down(const CL_InputEvent& event);
  
  CL_Slot slot;

protected:  
  int frames;
  float time_counter;
  int fps_counter;
  int fps_save;
public:
  Screen();
  virtual ~Screen();

  virtual void draw() =0;
  virtual void update(float delta) =0;

  void display();

private:
  Screen (const Screen&);
  Screen& operator= (const Screen&);
};

} // namespace Windstille

#endif

/* EOF */
