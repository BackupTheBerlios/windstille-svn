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

#ifndef HEADER_SCREEN_HXX
#define HEADER_SCREEN_HXX

#include <SDL.h>
#include "signals/slot.hpp"

class Controller;

/** */
class Screen
{
private:
  void draw_fps(float delta);
  unsigned int ticks;
  void poll_events();

protected:  
  int   frames;
  float time_counter;
  int   fps_counter;
  int   fps_save;
  float overlap_delta;

public:
  Screen();
  virtual ~Screen();

  virtual void draw() =0;
  virtual void update(float delta, const Controller& controller) =0;
  virtual void handle_event(const SDL_Event& event) =0;

  void display();
};

#endif

/* EOF */
