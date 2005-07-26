/*  $Id: windstille_main.hpp 752 2005-07-25 10:00:44Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>,
**                          Matthias Braun <matze@braunis.de>
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

#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <GL/gl.h>
#include <SDL.h>
#include "../sharedptr.hpp"

class TextureImpl;

class Texture
{
public:
  Texture(GLuint handle);

  /**
   * Upload an SDL_Surface onto an OpenGL texture. The surface must have power
   * of 2 dimensions
   * */
  Texture(SDL_Surface* image);
  Texture();
  ~Texture();

  GLuint get_handle() const;
private:
  SharedPtr<TextureImpl> impl;
};

#endif

/* EOF */
