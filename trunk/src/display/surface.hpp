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

#ifndef HEADER_SURFACE_HPP
#define HEADER_SURFACE_HPP

#include <string>
#include "math/rect.hpp"
#include "texture.hpp"
#include "sharedptr.hpp"

class SurfaceDrawingParameters;
class SurfaceImpl;

/**
 * Surface class. This class basically holds a reference to an opengl texture
 * along with texture coordinates that specify a rectangle on that texture.
 * Several surface may share a single texture (but have different texture
 * coordinates then)
 */
class Surface
{
public:
  Surface();
  Surface(const std::string& filename);
  
  /** 
   * Create a new Surface object from a Texture
   * @param texture The Texture to use
   * @param rect The rectangle of the texture to use in (0-1) notation
   * @param width  Width of the surface on the screen
   * @param height Height of the surface on the screen
   */
  Surface(Texture texture, const Rectf& rect, int width, int height);
  Surface(int width, int height);
  ~Surface();
  
  int get_width()  const;
  int get_height() const;

  Texture get_texture() const;

  /** Returns texture coordinates for the Surface rectangle (float[8]) */
  const float* get_texcoords() const;

  /** Returns texture coordinates for the Surface rectangle */
  Rectf get_uv() const;

  void draw(const Vector& pos) const;
  void draw(const SurfaceDrawingParameters& params) const;

  /** true if the Texture is valid and usable, false if not */
  operator bool() const;
private:
  SharedPtr<SurfaceImpl> impl;
};

#endif

/* EOF */
