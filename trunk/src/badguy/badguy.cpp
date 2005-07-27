/*  $Id: bomb.cpp 752 2005-07-25 10:00:44Z grumbel $
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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "badguy.hpp"
#include "sector.hpp"
#include "globals.hpp"
#include "tile_map.hpp"

Badguy::Badguy()
{
}

Badguy::~Badguy()
{
}

bool
Badguy::on_ground()
{
  return get_world()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE)*TILE_SIZE,
                                                        (int(pos.y)/TILE_SIZE)*TILE_SIZE)
    && Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE+1)*TILE_SIZE,
                                                      (int(pos.y)/TILE_SIZE)*TILE_SIZE)
    && Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE+2)*TILE_SIZE,
                                                      (int(pos.y)/TILE_SIZE)*TILE_SIZE)
    && Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE - 1)*TILE_SIZE,
                                                      (int(pos.y)/TILE_SIZE)*TILE_SIZE)
    && Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE - 2)*TILE_SIZE,
                                                      (int(pos.y)/TILE_SIZE)*TILE_SIZE);
}

bool
Badguy::in_wall()
{
  return get_world()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE)*TILE_SIZE,
                                                        (int(pos.y)/TILE_SIZE - 1)*TILE_SIZE)
    || Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE - 1)*TILE_SIZE,
                                                        (int(pos.y)/TILE_SIZE - 1)*TILE_SIZE)
    || Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE - 2)*TILE_SIZE,
                                                        (int(pos.y)/TILE_SIZE - 1)*TILE_SIZE)
    || Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE + 1)*TILE_SIZE,
                                                        (int(pos.y)/TILE_SIZE - 1)*TILE_SIZE)
    || Sector::current()->get_tilemap()->is_ground((int(pos.x)/TILE_SIZE + 2)*TILE_SIZE,
                                                      (int(pos.y)/TILE_SIZE - 1)*TILE_SIZE);
}
  
