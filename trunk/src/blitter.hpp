//  $Id: blitter.hpp 765 2005-07-26 00:16:45Z matzebraun $
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

#ifndef HEADER_BLITTER_HXX
#define HEADER_BLITTER_HXX

#include <SDL.h>
#include <ClanLib/Display/pixel_buffer.h>

/** Duplicate all the edge pixel of the given rectangle to the outside
    of the rectangle, thus creating a border around the given
    rectangle, this is needed for OpenGL textures to avoid filtering
    artefacts:

    X X X X X    1 1 2 3 3
    X 1 2 3 X _\ 1 1 2 3 3
    X 4 5 6 X  / 4 4 5 6 6
    X X X X X    4 4 5 6 6
*/
void generate_border(CL_PixelBuffer buffer, int x_pos, int y_pos, int width, int height);
void generate_border(SDL_Surface* surface, int x_pos, int y_pos, int width, int height);

void blit_opaque(CL_PixelBuffer target, CL_PixelBuffer brush, int x_pos, int y_pos);
void blit(CL_PixelBuffer target, CL_PixelBuffer brush, int x_pos, int y_pos);
void clear(CL_PixelBuffer target);

#endif

/* EOF */
