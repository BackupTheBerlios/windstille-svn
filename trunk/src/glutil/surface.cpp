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
#include <config.h>

#include "surface.hpp"
#include "surface_manager.hpp"

class SurfaceImpl
{
public:
  Texture texture;
  float texcoords[8];

  int width;
  int height;
};

Surface::Surface()
  : impl(new SurfaceImpl())
{
}

Surface::Surface(Texture texture, const Rect& rect, int width, int height)
  : impl(new SurfaceImpl())
{
  impl->texture = texture;
  impl->width   = width;
  impl->height  = height;
  
  impl->texcoords[0] = rect.left;
  impl->texcoords[1] = rect.top;
  impl->texcoords[2] = rect.right;
  impl->texcoords[3] = rect.top;
  impl->texcoords[4] = rect.right;
  impl->texcoords[5] = rect.bottom;
  impl->texcoords[6] = rect.left;
  impl->texcoords[7] = rect.bottom;
}

Surface::~Surface()
{
}

int
Surface::get_width()  const
{
  return impl->width;
}

int
Surface::get_height() const
{ 
  return impl->height; 
}

Texture
Surface::get_texture() const
{
  return impl->texture;
}

const float*
Surface::get_texcoords() const
{
  return impl->texcoords; 
}


/* EOF */
