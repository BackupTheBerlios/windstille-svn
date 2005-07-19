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

SurfaceDrawer::SurfaceDrawer(const CL_Surface& sur)
  : surface(sur)
{
}

SurfaceDrawer::~SurfaceDrawer() 
{
}
  
void
SurfaceDrawer::set_surface(const CL_Surface& sur)
{
  surface = sur;
  surface.set_alignment(origin_center);
}

static GLenum blendfunc2opengl(CL_BlendFunc blend)
{
  switch (blend)
    {
    case blend_zero:
      return GL_ZERO;
    case blend_one:
      return GL_ONE;
    case blend_dest_color:
      return GL_DST_COLOR;
    case blend_src_color:
      return GL_SRC_COLOR;
    case blend_one_minus_dest_color:
      return GL_ONE_MINUS_DST_COLOR;
    case blend_one_minus_src_color:
      return GL_ONE_MINUS_SRC_COLOR;
    case blend_src_alpha:
      return GL_SRC_ALPHA;
    case blend_one_minus_src_alpha:
      return GL_ONE_MINUS_SRC_ALPHA;
    case blend_dst_alpha:
      return GL_DST_ALPHA;
    case blend_one_minus_dest_alpha:
      return GL_ONE_MINUS_DST_ALPHA;
    case blend_src_alpha_saturate:
      return GL_SRC_ALPHA_SATURATE;
    case blend_constant_color:
      return GL_CONSTANT_COLOR;
    case blend_constant_alpha:
      return GL_CONSTANT_ALPHA;
    case blend_one_minus_constant_color:
      return GL_ONE_MINUS_CONSTANT_COLOR;
    case blend_one_minus_constant_alpha:
      return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
  return GL_ONE;
}

void
SurfaceDrawer::draw(SceneContext& sc, ParticleSystem& psys) 
{          
  VertexArrayDrawingRequest* buffer = new VertexArrayDrawingRequest(CL_Vector(psys.get_x_pos(), psys.get_y_pos(), 0), // FIXME: add zpos
                                                                    sc.color().get_modelview());

  buffer->set_mode(GL_QUADS);
  buffer->set_surface(surface);
  CL_BlendFunc src, dest;
  surface.get_blend_func(src, dest);

  buffer->set_blend_func(blendfunc2opengl(src), blendfunc2opengl(dest));

  int s_width  = surface.get_width();
  int s_height = surface.get_height();
  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      if (i->t != -1.0f)
        {
          float p = 1.0f - psys.get_progress(i->t);
          CL_Color color(int(psys.color_start.get_red()   * p + psys.color_stop.get_red()   * (1.0f - p)),
                         int(psys.color_start.get_green() * p + psys.color_stop.get_green() * (1.0f - p)),
                         int(psys.color_start.get_blue()  * p + psys.color_stop.get_blue()  * (1.0f - p)),
                         int(psys.color_start.get_alpha() * p + psys.color_stop.get_alpha() * (1.0f - p)));

          // scale
          float scale  = psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start);
          float width  = s_width  * scale;
          float height = s_height * scale;
              
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

          buffer->color(color);
          buffer->texcoord(0, 0);
          buffer->vertex(i->x - x_rot, i->y - y_rot);

          buffer->color(color);
          buffer->texcoord(1, 0);
          buffer->vertex(i->x + y_rot, i->y - x_rot);

          buffer->color(color);
          buffer->texcoord(1, 1);
          buffer->vertex(i->x + x_rot, i->y + y_rot);

          buffer->color(color);
          buffer->texcoord(0, 1);
          buffer->vertex(i->x - y_rot, i->y + x_rot);
        }
    }

  sc.color().draw(buffer);
}

/* EOF */
