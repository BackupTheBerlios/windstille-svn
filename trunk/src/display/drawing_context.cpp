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
#include <ClanLib/Display/sprite.h>
#include <ClanLib/Display/display.h>
#include <ClanLib/Display/display_window.h>
#include <ClanLib/Display/graphic_context.h>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include <iostream>
#include <iosfwd>
#include "fonts.hpp"
#include "drawing_context.hpp"
#include "glutil/surface.hpp"

std::ostream& operator<<(std::ostream& s, const Matrix& m)
{
  s << "[" << m[ 0] << ", " << m[ 4] << ", " << m[ 8] << ", " << m[12] << "\n";
  s << " " << m[ 1] << ", " << m[ 5] << ", " << m[ 9] << ", " << m[13] << "\n";
  s << " " << m[ 2] << ", " << m[ 6] << ", " << m[10] << ", " << m[14] << "\n";
  s << " " << m[ 3] << ", " << m[ 7] << ", " << m[11] << ", " << m[15] << "]\n";

  return s;
}

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
    : DrawingRequest(Vector(0, 0, -1000.0f)), color(color_)
  {
  }
  virtual ~FillScreenDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    gc->clear(CL_Color(
                static_cast<unsigned int> (color.r * 255.0),
                static_cast<unsigned int> (color.g * 255.0),
                static_cast<unsigned int> (color.b * 255.0),
                static_cast<unsigned int> (color.a * 255.0)));
  }
};

class SpriteDrawingRequest : public DrawingRequest
{
private:
  CL_Sprite sprite;

public:
  SpriteDrawingRequest(const CL_Sprite& sprite_, const Vector& pos_, const Matrix& modelview_)
    : DrawingRequest(pos_, modelview_),
      sprite(sprite_)
  {}
  virtual ~SpriteDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    //sprite.draw(static_cast<int>(pos.x + modelview[12]),
    //          static_cast<int>(pos.y + modelview[13]), gc);

    gc->push_modelview();
    gc->add_modelview(modelview);
    sprite.draw(static_cast<int>(pos.x),
                static_cast<int>(pos.y), gc);
    gc->pop_modelview();
  }
};

class CLSurfaceDrawingRequest : public DrawingRequest
{
private:
  CL_Surface sprite;

public:
  CLSurfaceDrawingRequest(const CL_Surface& sprite_, const Vector& pos_, const Matrix& modelview_)
    : DrawingRequest(pos_, modelview_),
      sprite(sprite_)
  {}
  virtual ~CLSurfaceDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    // FIXME: frequent push/pops might be slow
    gc->push_modelview();
    gc->add_modelview(modelview);
    sprite.draw(static_cast<int>(pos.x),
                static_cast<int>(pos.y), gc);
    gc->pop_modelview();
  }
};

class TextDrawingRequest : public DrawingRequest
{
private:
  std::string text;
public:
  TextDrawingRequest(const std::string& text_, const Vector& pos_, const Matrix& modelview_)
    : DrawingRequest(pos_, modelview_),
      text(text_)
  {}
  virtual ~TextDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    gc->push_modelview();
    gc->add_modelview(modelview);
    Fonts::dialog_h.set_alignment(origin_center);
    Fonts::dialog_h.draw(int(pos.x), int(pos.y), text);
    gc->pop_modelview();
  }
};

class SurfaceDrawingRequest : public DrawingRequest
{
private:
  Surface surface;
  float alpha;

public:
  SurfaceDrawingRequest(Surface surface, const Vector& pos,
                        const Matrix modelview, float alpha)
    : DrawingRequest(pos, modelview), surface(surface), alpha(alpha)
  {}
  virtual ~SurfaceDrawingRequest()
  {}

  void draw(CL_GraphicContext* gc) {
    static const float rectvertices[12]
      = { 0, 0, 0,
          1, 0, 0,
          1, 1, 0,
          0, 1, 0 };

    CL_OpenGLState state(gc);
    state.set_active();
    state.setup_2d();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glMultMatrixd(modelview);
    glTranslatef(pos.x, pos.y, 0);
    glScalef(surface.get_width(), surface.get_height(), 1.0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    if(alpha != 0.0)
      glColor4f(1.0, 1.0, 1.0, alpha);
    
    glBindTexture(GL_TEXTURE_2D, surface.get_texture().get_handle());
    
    glVertexPointer(3, GL_FLOAT, 0, rectvertices);
    glTexCoordPointer(2, GL_FLOAT, 0, surface.get_texcoords());
    glDrawArrays(GL_QUADS, 0, 4);

    if(alpha != 0.0)
      glColor4f(1.0, 1.0, 1.0, 0.0);
    glPopMatrix();
  }
};

DrawingContext::DrawingContext()
{
  modelview_stack.push_back(Matrix(true));
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
      gc = CL_Display::get_current_window()->get_gc();
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
DrawingContext::draw(const CL_Surface&   sprite,  float x, float y, float z)
{ // FIXME: This should get flattend down to a simple texture draw
  // command for easier sorting after texture-id/alpha
  draw(new CLSurfaceDrawingRequest(sprite, Vector(x, y, z), modelview_stack.back()));
}

void
DrawingContext::draw(const CL_Sprite&   sprite,  float x, float y, float z)
{ // FIXME: This should get flattend down to a simple texture draw
  // command for easier sorting after texture-id/alpha
  draw(new SpriteDrawingRequest(sprite, Vector(x, y, z), modelview_stack.back()));
}

void
DrawingContext::draw(Surface surface, float x, float y, float z, float a)
{
  draw(new SurfaceDrawingRequest(surface, Vector(x, y, z),
                                 modelview_stack.back(), a));
}

void
DrawingContext::draw(const std::string& text,    float x, float y, float z)
{ 
  draw(new TextDrawingRequest(text, Vector(x, y, z), modelview_stack.back()));
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

  Matrix matrix(true);
  matrix[0] = x*x*(1-c)+c;
  matrix[1] = y*x*(1-c)+z*s;
  matrix[2] = x*z*(1-c)-y*s;

  matrix[4] = x*y*(1-c)-z*s;
  matrix[5] = y*y*(1-c)+c;
  matrix[6] = y*z*(1-c)+x*s;

  matrix[8] = x*z*(1-c)+y*s;
  matrix[9] = y*z*(1-c)-x*s;
  matrix[10] = z*z*(1-c)+c;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::scale(float x, float y, float z)
{
  Matrix matrix(true);
  matrix[0] = x;
  matrix[5] = y;
  matrix[10] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::translate(float x, float y, float z)
{
  Matrix matrix(true);
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
  modelview_stack.push_back(Matrix(true));
}

Rectf
DrawingContext::get_clip_rect()
{
  // FIXME: Need to check the modelview matrix
  return Rectf(CL_Pointf(modelview_stack.back()[12],
                         modelview_stack.back()[13]),
               CL_Sizef(800, 600));
}

/* EOF */
