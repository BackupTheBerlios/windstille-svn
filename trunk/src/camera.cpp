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

#include "player.hpp"
#include "sector.hpp"
#include "display/display.hpp"
#include "camera.hpp"

Camera* Camera::current_ = 0;

Camera::Camera()
  : pos(0, 0),
    active(true)
{
  current_ = this;
}

void
Camera::update(float )
{
  if (!active)
    return;
    
  int hscroll_threshold = 100;
  int vscroll_threshold = 150;

  Vector tpos = Player::current()->get_pos();

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

  int start_x = Display::get_width()/2;
  int end_x   = Sector::current()->get_width() - Display::get_width()/2;

  int start_y = Display::get_height()/2;
  int end_y   = Sector::current()->get_height() - Display::get_height()/2;

  if (pos.x < start_x)
    pos.x = start_x;

  if (pos.y < start_y)
    pos.y = start_y;

  if (pos.x > end_x)
    pos.x = end_x;

  if (pos.y > end_y)
    pos.y = end_y;
}

void
Camera::set_pos(float x, float y)
{
  pos.x = x + Display::get_width()/2;
  pos.y = y + Display::get_height()/2;
}

/* EOF */
