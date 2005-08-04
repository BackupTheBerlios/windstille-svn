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

#include <assert.h>
#include <iostream>
#include <iosfwd>
#include <GL/gl.h>
#include "fonts.hpp"
#include "sprite2d/sprite.hpp"
#include "drawing_context.hpp"
#include "glutil/opengl_state.hpp"
#include "glutil/surface_drawing_parameters.hpp"
#include "glutil/surface.hpp"

struct DrawingRequestsSorter
{
  bool operator()(DrawingRequest* a, DrawingRequest* b) {
    return a->get_z_pos() < b->get_z_pos();
  }
};

class FillScreenDrawingRequest : public DrawingRequest
{
private:
  Color color;
public:
  FillScreenDrawingRequest(const Color& color_) 
    : DrawingRequest(Vector(0, 0), -1000.0f), color(color_)
  {
  }
  virtual ~FillScreenDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    OpenGLState state;
    // FIXME: move clear color to opengl_state
    state.activate();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
  }
};

class TextDrawingRequest : public DrawingRequest
{
private:
  std::string text;
public:
  TextDrawingRequest(const std::string& text_, const Vector& pos_, float z_pos_, const Matrix& modelview_)
    : DrawingRequest(pos_, z_pos_, modelview_),
      text(text_)
  {}
  virtual ~TextDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    glPushMatrix();
    glMultMatrixf(modelview.matrix);
    Fonts::ttffont->draw(int(pos.x), int(pos.y), text);
    glPopMatrix();
  }
};

class SurfaceDrawingRequest : public DrawingRequest
{
private:
  Surface surface;
  SurfaceDrawingParameters params;

public:
  SurfaceDrawingRequest(Surface surface, const SurfaceDrawingParameters& params, float z_pos_,
                        const Matrix modelview)
    : DrawingRequest(pos, z_pos_, modelview), surface(surface), params(params)
  {}
  virtual ~SurfaceDrawingRequest()
  {}

  void draw(CL_GraphicContext* gc) 
  {
    glPushMatrix();
    glMultMatrixf(modelview.matrix);

    surface.draw(params);

    glPopMatrix();
  }
};

DrawingContext::DrawingContext()
{
  modelview_stack.push_back(Matrix::identity());
}

DrawingContext::~DrawingContext()
{
  clear();
}

void
DrawingContext::render(CL_GraphicContext* gc)
{
  if (gc == 0)
    {
      //gc = CL_Display::get_current_window()->get_gc();
    }

  std::stable_sort(drawingrequests.begin(), drawingrequests.end(), DrawingRequestsSorter());
  
  for(DrawingRequests::iterator i = drawingrequests.begin(); i != drawingrequests.end(); ++i)
    {
      (*i)->draw(gc);
    }
}

void
DrawingContext::clear()
{
  for(DrawingRequests::iterator i = drawingrequests.begin(); i != drawingrequests.end(); ++i)
    {
      delete *i;
    }
  drawingrequests.clear();
}

void
DrawingContext::draw(DrawingRequest* request)
{
  drawingrequests.push_back(request);
}

void
DrawingContext::draw(const Sprite& sprite,  const Vector& pos, float z_pos)
{
  draw(sprite.get_current_surface(),
       SurfaceDrawingParameters()
       .set_pos(pos + sprite.get_offset() * sprite.get_scale())
       .set_blend_func(sprite.get_blend_sfactor(), sprite.get_blend_dfactor())
       .set_color(sprite.get_color())
       .set_scale(sprite.get_scale()),
       z_pos);
}

void
DrawingContext::draw(const Surface surface, const SurfaceDrawingParameters& params, float z_pos)
{
  draw(new SurfaceDrawingRequest(surface, params, z_pos,
                                 modelview_stack.back()));
}

void
DrawingContext::draw(Surface surface, float x, float y, float z, float a)
{
  draw(new SurfaceDrawingRequest(surface,
                                 SurfaceDrawingParameters().set_pos(Vector(x, y)),
                                 z, modelview_stack.back()));
}

void
DrawingContext::draw(const std::string& text, float x, float y, float z)
{ 
  draw(new TextDrawingRequest(text, Vector(x, y), z, modelview_stack.back()));
}

void
DrawingContext::fill_screen(const Color& color)
{
  draw(new FillScreenDrawingRequest(color));
}

void
DrawingContext::rotate(float angle, float x, float y, float z)
{
  double len2 = x*x+y*y+z*z;
  if (len2 != 1.0)
    {
      double len = sqrt(len2);
      x /= len;
      y /= len;
      z /= len;
    }

  double c = cos(angle*3.14159265/180);
  double s = sin(angle*3.14159265/180);

  Matrix matrix = Matrix::identity();
  matrix[0]  = x*x*(1-c)+c;
  matrix[1]  = y*x*(1-c)+z*s;
  matrix[2]  = x*z*(1-c)-y*s;

  matrix[4]  = x*y*(1-c)-z*s;
  matrix[5]  = y*y*(1-c)+c;
  matrix[6]  = y*z*(1-c)+x*s;

  matrix[8]  = x*z*(1-c)+y*s;
  matrix[9]  = y*z*(1-c)-x*s;
  matrix[10] = z*z*(1-c)+c;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::scale(float x, float y, float z)
{
  Matrix matrix = Matrix::identity();
  matrix[0]  = x;
  matrix[5]  = y;
  matrix[10] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::translate(float x, float y, float z)
{
  Matrix matrix = Matrix::identity();
  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::push_modelview()
{
  modelview_stack.push_back(modelview_stack.back());
}

void
DrawingContext::pop_modelview()
{
  modelview_stack.pop_back();
  assert(!modelview_stack.empty());
}

void
DrawingContext::set_modelview(const Matrix& matrix)
{
  modelview_stack.back() = matrix;
}

void
DrawingContext::reset_modelview()
{
  modelview_stack.clear();
  modelview_stack.push_back(Matrix::identity());
}

Rectf
DrawingContext::get_clip_rect()
{
  // FIXME: Need to check the modelview matrix
  return Rectf(Vector(modelview_stack.back()[12],
                      modelview_stack.back()[13]),
               Sizef(800, 600));
}

/* EOF */
