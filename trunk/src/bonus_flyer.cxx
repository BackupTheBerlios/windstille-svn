//  $Id: bonus_flyer.cxx,v 1.2 2003/08/12 08:24:41 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
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

#include "globals.hxx"
#include "bonus_flyer.hxx"

BonusFlyer::BonusFlyer (const CL_Vector2& arg_pos)
  : sprite ("bonusflyer", resources),
    pos (arg_pos), vel (100, 0), passed_time (0)
{
}

void
BonusFlyer::draw (SceneContext& gc)
{
  sprite.draw ((int) pos.x, (int) pos.y);
}

void
BonusFlyer::update (float delta)
{
  sprite.update (delta);
  passed_time += delta;

  vel.y = sin (passed_time*5) * 150;

  //std::cout << "Time: " << passed_time << "Vel: " << vel.y << std::endl;

  pos += vel * delta;
}

/* EOF */
