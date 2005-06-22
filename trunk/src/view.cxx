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
    world (player->get_world ())
{
  current_ = this;
}

void
View::draw (SceneContext& gc)
{
  CL_Display::push_modelview();
  CL_Display::add_translate(int(-pos.x + CL_Display::get_width ()/2),
                            int(-pos.y + CL_Display::get_height ()/2));

  gc.push_modelview();
  gc.translate(-pos.x + CL_Display::get_width ()/2,
               -pos.y + CL_Display::get_height ()/2);
  world->draw(gc);
  gc.pop_modelview();

  CL_Display::pop_modelview();
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

CL_Rect
View::get_clip_rect()
{
  return CL_Rect(CL_Point(int(pos.x - CL_Display::get_width()/2),
                          int(pos.y - CL_Display::get_height()/2)),
                 CL_Size(800, 600));
}

CL_Pointf
View::screen2world(CL_Pointf point)
{
  return CL_Pointf(point.x + pos.x - CL_Display::get_width()/2,
                   point.y + pos.y - CL_Display::get_height()/2);
}

CL_Pointf
View::world2screen(CL_Pointf point)
{
  return CL_Pointf(point.x - pos.x + CL_Display::get_width()/2,
                   point.y - pos.y + CL_Display::get_height()/2);
}

/* EOF */
