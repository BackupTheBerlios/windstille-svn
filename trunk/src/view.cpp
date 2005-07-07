//  $Id: view.cxx,v 1.1 2003/09/21 18:05:21 grumbel Exp $
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

#include "player.hpp"
#include "sector.hpp"
#include "view.hpp"

View* View::current_ = 0;

View::View()
  : state(CL_Display::get_width(), CL_Display::get_height()),
  zoom(1), transform(0, 0, 0)
{
  current_ = this;
}

void
View::draw (SceneContext& sc)
{
  state.set_pos(camera.get_pos());

#if 0
  static float zoom = 1.0f;

  if (CL_Keyboard::get_keycode(CL_KEY_A))
    zoom *= 1.01f/1.0f;
  if (CL_Keyboard::get_keycode(CL_KEY_O))
    zoom *= 1.0f/1.01f;
#endif
  
  state.set_zoom(zoom);
  state.set_pos(state.get_pos() + CL_Pointf(transform.x, transform.y));

  state.push(sc);
  Sector::current()->draw(sc);
  state.pop(sc);
}

void
View::update (float delta)
{
  camera.update(delta);

  if (CL_Keyboard::get_keycode(CL_KEY_ADD))
    zoom *= 1.0 + delta;
  if (CL_Keyboard::get_keycode(CL_KEY_SUBTRACT))
    zoom *= 1.0 - delta;

  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD2))
    transform.y += delta * 200 / zoom;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD8))
    transform.y -= delta * 200 / zoom;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD4))
    transform.x -= delta * 200 / zoom;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD6))
    transform.x += delta * 200 / zoom;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD5)) {
    transform = Vector(0, 0, 0);
    zoom = 1.0;
  }
}

CL_Rectf
View::get_clip_rect()
{
  return state.get_clip_rect();
}

CL_Pointf
View::screen2world(CL_Pointf point)
{
  return state.screen2world(CL_Point(point));
}

/* EOF */
