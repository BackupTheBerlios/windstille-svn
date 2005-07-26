//  $Id: blitter.cpp 765 2005-07-26 00:16:45Z matzebraun $
//
//  Flexlay - A Generic 2D Game Editor
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

#include <assert.h>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <ClanLib/Display/pixel_format.h>
#include <ClanLib/Display/palette.h>
#include "blitter.hpp"

void generate_border(SDL_Surface* surface,
                     int x_pos, int y_pos, int width, int height)
{
  assert(surface->format->BitsPerPixel == 32);
  SDL_LockSurface(surface);
 
  uint8_t* data = static_cast<uint8_t*>(surface->pixels);
  int pitch = surface->pitch;

  // duplicate the top line
  memcpy(data + (y_pos-1)*pitch + 4*x_pos, 
         data + (y_pos)*pitch + 4*x_pos,
         4*width);
  // duplicate the bottom line
  memcpy(data + (y_pos+height)*pitch + 4*x_pos, 
         data + (y_pos+height-1)*pitch + 4*x_pos,  
         4*width);

  // duplicate left and right borders
  for(int y = y_pos-1; y < y_pos + height+1; ++y)
    {
      uint32_t* p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos-1)));
      *p = *(p+1);
      p = reinterpret_cast<uint32_t*> (data + (y*pitch + 4*(x_pos + width)));
      *p = *(p-1);
    }

  SDL_UnlockSurface(surface);
}

/* EOF */
