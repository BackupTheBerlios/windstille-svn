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
#include "sprite3d.hpp"

#include <vector>
#include <stdint.h>
#include <sstream>
#include <stdexcept>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include <physfs.h>
#include "display/drawing_request.hpp"
#include "display/scene_context.hpp"
#include "sprite3d_data.hpp"
#include "lisp_util.hpp"
#include "globals.hpp"
#include "util.hpp"
#include "timer.hpp"

Sprite3D::Sprite3D(const Sprite3DData* data)
  : data(data), vflip(false)
{
  current_action = &data->actions[0];
  time_delta = 0;
  speed = 3.0;
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(const std::string& actionname)
{
  // TODO: make this a little smarter so that the action change is delayed until
  // the next frame and then the current action blends with the next action
  Action* action = 0;
  for(uint16_t a = 0; a < data->action_count; ++a) {
    if(data->actions[a].name == actionname)
      action = &data->actions[a];
  }
  if(action == 0) {
    std::ostringstream msg;
    msg << "No action with name '" << action << "' found";
    throw std::runtime_error(msg.str());
  }

  current_action = action;
  time_delta = game_time;
}

const std::string&
Sprite3D::get_action() const
{
  return current_action->name;
}

void
Sprite3D::set_speed(float speed)
{
  this->speed = speed;
}

float
Sprite3D::get_speed() const
{
  return speed;
}

void
Sprite3D::set_vflip(bool vflip)
{
  this->vflip = vflip;
}

class Sprite3DDrawingRequest : public DrawingRequest
{
private:
  Sprite3D* sprite;
  const ActionFrame* frame1;
  const ActionFrame* frame2;
  float time;

public:
  Sprite3DDrawingRequest(Sprite3D* sprite, const ActionFrame* frame1,
                         const ActionFrame* frame2, float time,
                         const Vector& pos, const Matrix& modelview)
      : DrawingRequest(pos, modelview), sprite(sprite), frame1(frame1),
        frame2(frame2), time(time)
  {
  }

  void draw(CL_GraphicContext* gc)
  {
    sprite->blend_frames(gc, frame1, frame2, time, pos, modelview);
  }
};

void
Sprite3D::draw(SceneContext& sc, const Vector& pos)
{
  float gtime = (game_time - time_delta) * speed;
  int frame = static_cast<int>(gtime) % current_action->frame_count;
  int nextframe = (frame+1) % current_action->frame_count;  
  float time = fmodf(gtime, 1.0);

  const ActionFrame* frame1 = &current_action->frames[frame];
  const ActionFrame* frame2 = &current_action->frames[nextframe];
  sc.color().draw(new Sprite3DDrawingRequest(this, frame1, frame2, time, pos,
                                             sc.color().get_modelview()));
}

void
Sprite3D::blend_frames(CL_GraphicContext* gc, const ActionFrame* frame1,
                       const ActionFrame* frame2, float time,
                       const Vector& pos, const Matrix& modelview)
{
  assert_gl("before render_frame");
 
  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  glPushMatrix();
  glMultMatrixd(modelview);
  glTranslatef(pos.x, pos.y, pos.z);
  if(vflip)
    glScalef(-1.0, 1.0, 1.0);  

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);  

  assert_gl("gl init before sprite");
  
  float t_1 = 1.0 - time;
  for(uint16_t m = 0; m < data->mesh_count; ++m) {
    const Mesh& mesh = data->meshs[m];
    const MeshVertices& vertices1 = frame1->meshs[m];
    const MeshVertices& vertices2 = frame2->meshs[m];
    
    float* verts = new float[mesh.vertex_count * 3];

    for(uint16_t v = 0; v < mesh.vertex_count*3; ++v) {
      verts[v] = vertices1.vertices[v] * t_1 + vertices2.vertices[v] * time;
    }
    
    CL_OpenGLSurface& texture = const_cast<CL_OpenGLSurface&> (mesh.texture);
    texture.bind();

    glVertexPointer(3, GL_FLOAT, 0, verts);
    glNormalPointer(GL_FLOAT, 0, mesh.normals);
    glTexCoordPointer(2, GL_FLOAT, 0, mesh.tex_coords);

    glDrawElements(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT,
        mesh.vertex_indices);
    delete[] verts;
  }

  assert_gl("rendering 3d sprite");      

  glPopMatrix();
}

