/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "display/drawing_request.hpp"
#include "display/vertex_array_drawing_request.hpp"
#include "particle_system.hpp"
#include "lisp/properties.hpp"
#include "lisp_getters.hpp"
#include "color.hpp"
#include "spark_drawer.hpp"

SparkDrawer::SparkDrawer(FileReader& props)
{
  width = 1.0f;
  color = Color(1.0f, 1.0f, 1.0f);
  props.get("color", color);
  props.get("width", width);
}

void
SparkDrawer::draw(DrawingContext& dc, ParticleSystem& psys) 
{
  VertexArrayDrawingRequest* buffer = new VertexArrayDrawingRequest(Vector(0, 0), psys.get_z_pos(),
                                                                    dc.get_modelview());
  if (width == 1.0f)
    {
      buffer->set_mode(GL_LINES);
      buffer->set_blend_func(GL_SRC_ALPHA, GL_ONE);
      for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
        {
          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(i->x + i->v_x/10.0f, i->y + i->v_y/10.0f); 

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(i->x, i->y);
        }
    }
  else
    {
      buffer->set_mode(GL_QUADS);
      buffer->set_blend_func(GL_SRC_ALPHA, GL_ONE);
      for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
        {
          float len = sqrt(i->v_x * i->v_x + i->v_y * i->v_y);
          float o_x = i->v_x/len * width;
          float o_y = i->v_y/len * width;

          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(i->x - o_y, i->y - o_x);

          buffer->color(Color(color.r, color.g, color.b, color.a - (color.a * psys.get_progress(i->t))));
          buffer->vertex(i->x + o_y, i->y - o_x);

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(i->x - i->v_x/10.0f - o_y, i->y - i->v_y/10.0f - o_x); 

          buffer->color(Color(0, 0, 0, 0));
          buffer->vertex(i->x + i->v_x/10.0f + o_y, i->y - i->v_y/10.0f - o_x); 
        }
    }

  dc.draw(buffer);
}

/* EOF */
