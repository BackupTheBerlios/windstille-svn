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
#include "particle_system.hpp"
#include "spark_drawer.hpp"

class SparkDrawerDrawingRequest : public DrawingRequest
{
private:
  ParticleSystem& psys;
public:
  SparkDrawerDrawingRequest(ParticleSystem& psys_,
                            const CL_Vector& pos, const CL_Matrix4x4& modelview = CL_Matrix4x4(true))
    : DrawingRequest(pos, modelview),
      psys(psys_)
  {
  }

  virtual ~SparkDrawerDrawingRequest() {}
  
  void draw(CL_GraphicContext* gc) 
  {
    gc->push_modelview();
    gc->add_modelview(modelview);

    CL_OpenGLState state(gc);
    state.set_active();
    state.setup_2d();
  
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glBegin(GL_LINES);
    for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
      {
        glColor4f(1.0, 1.0, 0, 1.0f-psys.get_progress(i->t));
        glVertex2f(i->x, i->y);
        glColor4f(0, 0, 0, 0);
        glVertex2f(i->x - i->v_x/10.0f, i->y - i->v_y/10.0f);
      }
    glEnd();  

    gc->pop_modelview();
  }
};

void
SparkDrawer::draw(SceneContext& sc, ParticleSystem& psys) 
{
  sc.color().draw(new SparkDrawerDrawingRequest(psys, CL_Vector(0, 0, .5f), sc.color().get_modelview()));
}

/* EOF */
