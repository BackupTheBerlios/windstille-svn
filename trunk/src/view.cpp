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

#include <SDL.h>
#include "display/display.hpp"
#include "collision/collision_engine.hpp"
#include "player.hpp"
#include "sector.hpp"
#include "view.hpp"

View* View::current_ = 0;

View::View()
  : state(Display::get_width(), Display::get_height()),
    zoom(1), transform(0, 0)
{
  current_ = this;
}

void
View::draw (SceneContext& sc)
{
  state.set_pos(camera.get_pos());
  state.set_zoom(zoom);
  state.set_pos(state.get_pos() + Vector(transform.x, transform.y));

  state.push(sc);
  Sector::current()->draw(sc);
  if (debug)
    Sector::current()->get_collision_engine()->draw(sc.highlight());
  state.pop(sc);
}

void
View::update (float delta)
{
  camera.update(delta);

  Uint8 *keystate = SDL_GetKeyState(NULL);

  if (keystate[SDLK_KP_PLUS])
    zoom *= 1.0 + delta;
  if (keystate[SDLK_KP_MINUS])
    zoom *= 1.0 - delta;

  if(keystate[SDLK_KP2])
    transform.y += delta * 200 / zoom;
  if(keystate[SDLK_KP8])
    transform.y -= delta * 200 / zoom;
  if(keystate[SDLK_KP4])
    transform.x -= delta * 200 / zoom;
  if(keystate[SDLK_KP6])
    transform.x += delta * 200 / zoom;
  if(keystate[SDLK_KP5]){
    transform = Vector(0, 0);
    zoom = 1.0;
  }
}

Rectf
View::get_clip_rect()
{
  return state.get_clip_rect();
}

Vector
View::screen_to_world(const Vector& point)
{
  return state.screen_to_world(point);
}

/* EOF */
