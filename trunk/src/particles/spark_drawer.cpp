//  $Id$
//
//  Pingus - A free Lemmings clone
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

#include <ClanLib/Display/graphic_context.h>
#include <ClanLib/Display/surface.h>
#include <ClanLib/gl.h>
#include "display/drawing_request.hpp"
#include "display/vertex_array_drawing_request.hpp"
#include "particle_system.hpp"
#include "color.hpp"
#include "spark_drawer.hpp"

void
SparkDrawer::draw(SceneContext& sc, ParticleSystem& psys) 
{
  VertexArrayDrawingRequest* buffer = new VertexArrayDrawingRequest(CL_Vector(0, 0, 100.0f),
                                                                    sc.color().get_modelview());

  buffer->set_mode(GL_LINES);
  buffer->set_blend_func(GL_SRC_ALPHA, GL_ONE);

  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      buffer->color(Color(1.0f, 1.0f, 0, 1.0f - psys.get_progress(i->t)));
      buffer->vertex(i->x, i->y);

      buffer->color(Color(0, 0, 0, 0));
      buffer->vertex(i->x - i->v_x/10.0f, i->y - i->v_y/10.0f); 
    }

  sc.color().draw(buffer);
}

/* EOF */
