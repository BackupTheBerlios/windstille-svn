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
#include <iostream>
#include <iosfwd>
#include "drawing_context.hxx"

std::ostream& operator<<(std::ostream& s, const CL_Matrix4x4& m)
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
  CL_Color color;
public:
  FillScreenDrawingRequest(const CL_Color& color_) 
    : DrawingRequest(CL_Vector(0, 0, -1000.0f)), color(color_)
  {
  }
  virtual ~FillScreenDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    gc->clear(color);
  }
};

class SpriteDrawingRequest : public DrawingRequest
{
private:
  CL_Sprite sprite;

public:
  SpriteDrawingRequest(const CL_Sprite& sprite_, const CL_Vector& pos_, const CL_Matrix4x4& modelview_)
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

class SurfaceDrawingRequest : public DrawingRequest
{
private:
  CL_Surface sprite;

public:
  SurfaceDrawingRequest(const CL_Surface& sprite_, const CL_Vector& pos_, const CL_Matrix4x4& modelview_)
    : DrawingRequest(pos_, modelview_),
      sprite(sprite_)
  {}
  virtual ~SurfaceDrawingRequest() {}

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
  TextDrawingRequest(const std::string& text_, const CL_Vector& pos_)
    : DrawingRequest(pos_),
      text(text_)
  {}
  virtual ~TextDrawingRequest() {}

  void draw(CL_GraphicContext* gc) {
    (void) gc;
    // FIXME: not implemented
  }
};

DrawingContext::DrawingContext()
{
  modelview_stack.push_back(CL_Matrix4x4(true));
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
  draw(new SurfaceDrawingRequest(sprite, CL_Vector(x, y, z), modelview_stack.back()));
}

void
DrawingContext::draw(const CL_Sprite&   sprite,  float x, float y, float z)
{ // FIXME: This should get flattend down to a simple texture draw
  // command for easier sorting after texture-id/alpha
  draw(new SpriteDrawingRequest(sprite, CL_Vector(x, y, z), modelview_stack.back()));
}

void
DrawingContext::draw(const std::string& text,    float x, float y, float z)
{ 
  draw(new TextDrawingRequest(text, CL_Vector(x, y, z)));
}

void
DrawingContext::draw_line (float , float , float , float , 
                           const CL_Color& )
{
}

void
DrawingContext::draw_fillrect (float , float , float , float , 
		      const CL_Color& )
{
}

void
DrawingContext::draw_rect (float , float , float , float , 
		  const CL_Color& )
{
}

void
DrawingContext::draw_pixel (float , float , const CL_Color& )
{
}

void
DrawingContext::draw_circle (float , float , float , const CL_Color& )
{
}

/** Draws an arc, starting from angle_start to angle_end in
      counterclockwise direction. Angles are taken in radian */
void
DrawingContext::draw_arc (float , float , float , float , float ,
                          const CL_Color& )
{
}

void
DrawingContext::fill_screen(const CL_Color& color)
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

  CL_Matrix4x4 matrix(true);
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
  CL_Matrix4x4 matrix(true);
  matrix[0] = x;
  matrix[5] = y;
  matrix[10] = z;

  modelview_stack.back() = modelview_stack.back().multiply(matrix);
}

void
DrawingContext::translate(float x, float y, float z)
{
  CL_Matrix4x4 matrix(true);
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
DrawingContext::reset_modelview()
{
  modelview_stack.clear();
  modelview_stack.push_back(CL_Matrix4x4(true));
}

CL_Rect
DrawingContext::get_clip_rect()
{
  // FIXME: Need to check the modelview matrix
  return CL_Rect(CL_Point(int(modelview_stack.back()[12]),
                          int(modelview_stack.back()[13])),
                 CL_Size(800, 600));
}

/* EOF */
