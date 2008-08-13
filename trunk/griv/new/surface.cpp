/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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
#include <assert.h>
#include "framebuffer.hpp"
#include "math.hpp"
#include "math/vector2i.hpp"
#include "math/vector2f.hpp"
#include "math/size.hpp"
#include "math/rect.hpp"
#include "software_surface.hpp"
#include "surface.hpp"

class SurfaceImpl
{
public:
  Texture texture;
  Rectf   uv;
  Size    size;
  
  SurfaceImpl(const Texture& texture_, const Rectf& uv_, const Size& size_)
    : texture(texture_),
      uv(uv_),
      size(size_)
  {
    //std::cout << uv << std::endl;
  }

  SurfaceImpl(const SoftwareSurface& src, const Rect& srcrect)
  {
    assert(src);

    texture = Texture(srcrect.get_size(), src, srcrect);
    
    uv = Rectf(Vector2f(0, 0), srcrect.get_size());

    size = Size(srcrect.get_size());
  }
  
  ~SurfaceImpl()
  {
  }

  void draw(const Rectf& rect)
  {
    if (texture)
      {
        texture.bind();
        glEnable(GL_TEXTURE_RECTANGLE_ARB);
        glColor3f(1.0f, 1.0f, 1.0f);       

        glBegin(GL_QUADS);
        glTexCoord2f(uv.left, uv.top);
        glVertex2f(rect.left, rect.top);

        glTexCoord2f(uv.right, uv.top);
        glVertex2f(rect.right, rect.top);

        glTexCoord2f(uv.right, uv.bottom);
        glVertex2f(rect.right, rect.bottom);

        glTexCoord2f(uv.left, uv.bottom);
        glVertex2f(rect.left, rect.bottom);
        glEnd();
      }
  }

  void draw(const Vector2f& pos)
  {
    draw(Rectf(pos, size));
  }
};

Surface::Surface()
{
}

Surface::Surface(boost::shared_ptr<SurfaceImpl> impl_)
  : impl(impl_)
{
}

Surface::Surface(const SoftwareSurface& src)
  : impl(new SurfaceImpl(src, Rect(Vector2i(0, 0), src.get_size())))
{
}

Surface::Surface(const SoftwareSurface& src, const Rect& srcrect)
  : impl(new SurfaceImpl(src, srcrect))
{
}

Surface::~Surface()
{
}

void
Surface::draw(const Vector2f& pos)
{
  if (impl.get())
    impl->draw(pos);
}

void
Surface::draw(const Rectf& rect)
{
  if (impl.get())
    impl->draw(rect);
}

int
Surface::get_width() const 
{
  if (impl.get())
    return impl->size.width; 
  else
    return 0;
}

int
Surface::get_height() const
{
  if (impl.get())
    return impl->size.height; 
  else
    return 0;
}

Size
Surface::get_size() const
{
  if (impl.get())
    return impl->size;
  else
    return Size();
}

void
Surface::set_size(const Size& size)
{
  if (impl.get())
    impl->size = size;
}

/* EOF */
