/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <iostream>
#include "tile.hpp"
#include "tile_map.hpp"
#include "stair_contact.hpp"

StairContact::StairContact(TileMap* tilemap_, const Point& pos_)
  : tilemap(tilemap_), pos(pos_), advancement(0.0f)
{
  unsigned int col = tilemap->get_pixel(pos.x, pos.y);
  if (!(col & TILE_STAIRS))
    std::cout << "Warning: StairContact: Given tile is no stair tile" << std::endl;

  tile_type = col;
}

void
StairContact::update(float delta)
{
  // shouldn't be needed!?
}

void
StairContact::advance(float s)
{
  if (s == 0)
    return;

  s /= 32.0f;

  //std::cout << "Advance: (" << pos.x << ", " << pos.y << ") " << advancement << " " << s << std::endl;
  
  float step = (s > 0.0f) ? 1.0f : -1.0f;

  // advancement must happen tile by tile  
  while ((fabs(s) > fabs(step)) && is_active())
    {
      advancement += step;
      s -= step;

      // increment tile position, based on tile direction
      // can be up/down, left/rigth
      advance_or_not();
    }

  advancement += s;
  advance_or_not();
}

void
StairContact::advance_or_not()
{
  if (tile_type & TILE_LEFT)
    {
      if (advancement < -0.5f)
        {
          pos.x -= 1;
          pos.y += 1;
          advancement += 1.0f;
        }
      else if (advancement > 0.5f)
        {
          pos.x += 1;
          pos.y -= 1;
          advancement -= 1.0f;
        }
    }
  else if (tile_type & TILE_RIGHT)
    {
      if (advancement < -0.5f)
        {
          pos.x += 1;
          pos.y -= 1;
          advancement += 1.0f;
        }
      else if (advancement > 0.5f)
        {
          pos.x -= 1;
          pos.y += 1;
          advancement -= 1.0f;
        }
    }
}

Vector
StairContact::get_pos() const
{
  if (tile_type & TILE_RIGHT)
    return Vector(pos.x * 32 + 16 + 32 * advancement,
                  pos.y * 32 + 16 + 32 * advancement);
  else // (tile_type & TILE_LEFT)
    return Vector(pos.x * 32 + 16 + 32 * advancement,
                  pos.y * 32 + 16 - 32 * advancement);
}

bool
StairContact::is_active() const
{
  return (tilemap->get_pixel(pos.x, pos.y) & TILE_STAIRS);
}

/* EOF */
