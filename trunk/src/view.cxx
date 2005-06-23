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

#include "player.hxx"
#include "sector.hxx"
#include "view.hxx"

View* View::current_ = 0;


View::View (Player* t)
  : player (t),
    world (player->get_world ()),
    state(CL_Display::get_width(), CL_Display::get_height())
{
  current_ = this;
}

void
View::draw (SceneContext& sc)
{
  state.set_pos(pos);
  state.push(sc);
  world->draw(sc);
  state.pop(sc);
}

void
View::update (float delta)
{
  int hscroll_threshold = 100;
  int vscroll_threshold = 150;

  CL_Vector tpos = player->get_pos();

  float dist = tpos.x - pos.x;
  if (dist > hscroll_threshold)
    pos.x = tpos.x - hscroll_threshold;
  else if (dist < - hscroll_threshold)
    pos.x = tpos.x + hscroll_threshold;

  dist = tpos.y - pos.y;
  if (dist > vscroll_threshold)
    pos.y = tpos.y - vscroll_threshold;
  else if (dist < -vscroll_threshold)
    pos.y = tpos.y + vscroll_threshold;

  int start_x = CL_Display::get_width()/2;
  int end_x   = world->get_width() - CL_Display::get_width()/2;

  int start_y = CL_Display::get_height()/2;
  int end_y   = world->get_height() - CL_Display::get_height()/2;

  if (pos.x < start_x)
    pos.x = start_x;

  if (pos.y < start_y)
    pos.y = start_y;

  if (pos.x > end_x)
    pos.x = end_x;

  if (pos.y > end_y)
    pos.y = end_y;
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
