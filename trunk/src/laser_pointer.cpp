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

#include "display/vertex_array_drawing_request.hpp"
#include "player.hpp"
#include "laser_pointer.hpp"

LaserPointer::LaserPointer()
{
  noise = Texture("images/noise2.png");
  noise.set_wrap(GL_REPEAT);
  noise.set_filter(GL_LINEAR);
  
  progress = 0.0f;
}

LaserPointer::~LaserPointer()
{
}

void
LaserPointer::draw(SceneContext& sc)
{
  Vector pos = Player::current()->get_pos();

  VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(pos - Vector(0, 64), 10000,
                                                                   sc.highlight().get_modelview());
  array->set_mode(GL_LINE_STRIP);
  array->set_texture(noise);
  array->set_blend_func(GL_SRC_ALPHA, GL_ONE);

  array->color(Color(1.0f, 0.0f, 0.0f, 1.0f));
  array->texcoord(0/400.0f, progress);
  array->vertex(0, 0);

  array->color(Color(1.0f, 0.0f, 0.0f, 1.0f));
  array->texcoord(300/400.0f, progress);
  array->vertex(300, 0);

  array->color(Color(1.0f, 0.0f, 0.0f, 0.0f));
  array->texcoord(400/400.0f, progress);
  array->vertex(400, 0);

  sc.highlight().draw(array);
}

void
LaserPointer::update(float delta)
{
  progress += 0.1 * delta;
}

/* EOF */
