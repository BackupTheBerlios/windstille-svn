//  $Id$
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <ClanLib/Display/graphic_context.h>
#include "vertex_array_drawing_request.hpp"

VertexArrayDrawingRequest::VertexArrayDrawingRequest(const CL_Vector& pos_, const CL_Matrix4x4& modelview_)
  : DrawingRequest(pos_, modelview_)
{
  blend_sfactor = GL_SRC_ALPHA;
  blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
  mode = GL_QUADS;
  use_texture = false;
}

int
VertexArrayDrawingRequest::num_vertices() const
{
  return vertices.size()/3;
}

void
VertexArrayDrawingRequest::draw(CL_GraphicContext* gc)
{
  assert(!vertices.empty());
  assert(texcoords.empty() || int(texcoords.size()/2) == num_vertices());
  assert(colors.empty() || int(colors.size()/4) == num_vertices());

  gc->push_modelview();
  gc->add_modelview(modelview);

  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  if (!colors.empty())
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, &*colors.begin());
    }
  else
    {
      glDisableClientState(GL_COLOR_ARRAY);
    }

  if (!texcoords.empty())
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, &*texcoords.begin());
    }
  else
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  
  glDisableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer  (3, GL_FLOAT, 0, &*vertices.begin());
  
  glEnable(GL_BLEND);
  glBlendFunc(blend_sfactor, blend_dfactor);

  if (use_texture)
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture);
    }
  else
    {
      glDisable(GL_TEXTURE_2D);
    }

  glDrawArrays(mode, 0, num_vertices());

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  gc->pop_modelview();
}

void
VertexArrayDrawingRequest::vertex(float x, float y, float z)
{
  vertices.push_back(x + pos.x);
  vertices.push_back(y + pos.y);
  vertices.push_back(z + pos.z);
}

void
VertexArrayDrawingRequest::texcoord(float u, float v)
{
  texcoords.push_back(u);
  texcoords.push_back(v);
}

void
VertexArrayDrawingRequest::color(const CL_Color& color)
{
  colors.push_back(color.get_red());
  colors.push_back(color.get_green());
  colors.push_back(color.get_blue());
  colors.push_back(color.get_alpha());
}

void
VertexArrayDrawingRequest::set_texture(GLuint texture)
{
  this->texture = texture;
  use_texture = true;
}

void
VertexArrayDrawingRequest::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  blend_sfactor = sfactor;
  blend_dfactor = dfactor;
}

void
VertexArrayDrawingRequest::set_mode(GLenum mode_)
{
  mode = mode_;
}

/* EOF */
