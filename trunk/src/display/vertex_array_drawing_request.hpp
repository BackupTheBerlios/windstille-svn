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

#ifndef HEADER_VERTEX_ARRAY_DRAWING_REQUEST_HXX
#define HEADER_VERTEX_ARRAY_DRAWING_REQUEST_HXX

#include <vector>
#include <ClanLib/gl.h>
#include "drawing_request.hpp"

/** */
class VertexArrayDrawingRequest : public DrawingRequest
{
private:
  GLenum mode;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

  bool use_texture;
  GLuint texture;
  std::vector<unsigned char> colors;
  std::vector<float> texcoords;
  std::vector<float> vertices;

public:
  VertexArrayDrawingRequest(const CL_Vector& pos_, const CL_Matrix4x4& modelview_);

  void draw(CL_GraphicContext* gc);

  void vertex(float x, float y, float z = 0.0f);
  void texcoord(float u, float v);
  void color(const CL_Color& color);

  int num_vertices() const;

  void set_mode(GLenum mode_);
  void set_texture(GLuint texture);
  void set_blend_func(GLenum sfactor, GLenum dfactor);
};

#endif

/* EOF */
