//  $Id: PlayerView.cxx,v 1.1 2002/03/19 17:56:52 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <ClanLib/gl.h>
#include "Player.hxx"
#include "PlayerView.hxx"
#include "GameWorld.hxx"

PlayerView::PlayerView (Player* t)
  : player (t),
    world (player->get_world ())
{
  
}

void
PlayerView::draw ()
{
  glPushMatrix ();
  glTranslatef (-pos.x + CL_Display::get_width ()/2, -pos.y + CL_Display::get_height ()/2, 0.0);
  world->draw ();
  glPopMatrix ();
}

void
PlayerView::update (float delta)
{
  CL_Vector tpos = player->get_pos ();

  float dist = tpos.x - pos.x;
  if (dist > 150)
    pos.x = tpos.x - 150;
  else if (dist < -150)
    pos.x = tpos.x + 150;

  dist = tpos.y - pos.y;
  if (dist > 50)
    pos.y = tpos.y - 50;
  else if (dist < -150)
    pos.y = tpos.y + 150;

}

/* EOF */
