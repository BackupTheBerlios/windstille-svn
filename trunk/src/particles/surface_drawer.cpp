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

#include "display/vertex_array_drawing_request.hpp"
#include "particle_system.hpp"
#include "surface_drawer.hpp"

SurfaceDrawer::SurfaceDrawer(Surface* surface)
  : surface(surface)
{
}

SurfaceDrawer::~SurfaceDrawer() 
{
}
  
void
SurfaceDrawer::set_texture(Surface* surface)
{
  this->surface = surface;
}

void
SurfaceDrawer::set_blendfuncs(GLenum blendfunc_src, GLenum blendfunc_dest)
{
  this->blendfunc_src  = blendfunc_src;
  this->blendfunc_dest = blendfunc_dest;
}

void
SurfaceDrawer::draw(SceneContext& sc, ParticleSystem& psys) 
{          
  VertexArrayDrawingRequest* buffer 
    = new VertexArrayDrawingRequest(Vector(psys.get_x_pos(), psys.get_y_pos(), 0), // FIXME: add zpos
                                    sc.color().get_modelview());

  buffer->set_mode(GL_QUADS);
  buffer->set_texture(surface->get_texture());
  buffer->set_blend_func(blendfunc_src, blendfunc_dest);

  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      if (i->t != -1.0f)
        {
          float p = 1.0f - psys.get_progress(i->t);
          Color color(psys.color_start.r * p + psys.color_stop.r * (1.0f - p),
                      psys.color_start.g * p + psys.color_stop.g * (1.0f - p),
                      psys.color_start.b * p + psys.color_stop.b * (1.0f - p),
                      psys.color_start.a * p + psys.color_stop.a * (1.0f - p));

          // scale
          float scale  = psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start);
          
          float width  = surface->get_width()  * scale;
          float height = surface->get_height() * scale;
              
          // rotate
          float x_rot = width/2;
          float y_rot = height/2; 

          if (i->angle != 0)
            {
              float s = sin(M_PI * i->angle/180.0f);
              float c = cos(M_PI * i->angle/180.0f);
              x_rot = (width/2) * c - (height/2) * s;
              y_rot = (width/2) * s + (height/2) * c;
            }

          buffer->add_texcoords(surface->get_texcoords(), 8);

          buffer->color(color);
          buffer->vertex(i->x - x_rot, i->y - y_rot);

          buffer->color(color);
          buffer->vertex(i->x + y_rot, i->y - x_rot);

          buffer->color(color);
          buffer->vertex(i->x + x_rot, i->y + y_rot);

          buffer->color(color);
          buffer->vertex(i->x - y_rot, i->y + x_rot);
        }
    }

  sc.color().draw(buffer);
}

/* EOF */
