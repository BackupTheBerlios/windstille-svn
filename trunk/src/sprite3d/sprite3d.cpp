//  $Id$
//
//  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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
  : data(data), actions_switched(false)
{
  frame1.action = &data->actions[0];
  frame1.frame = 0;
  frame1.rot = false;
  frame1.speed = 1.0;
  frame2 = frame1;
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(const std::string& actionname, float speed)
{
  next_frame.action = & data->get_action(actionname);
  // set to last action so that next set_next_frame call will result in frame 0
  if(speed >= 0) {
    next_frame.frame = 0;
  } else {
    next_frame.frame = next_frame.action->frame_count - 1;
  }
  next_frame.speed = speed;
  next_frame.rot = frame2.rot;

  next_action.action = 0;
  actions_switched = false;
}

const std::string&
Sprite3D::get_action() const
{
  if(next_frame.action != 0)
    return next_frame.action->name;
  
  return frame2.action->name;
}

void
Sprite3D::set_next_action(const std::string& name, float speed)
{
  next_action.action = & data->get_action(name);
  if(speed >= 0) {
    next_action.frame = 0;
  } else {
    next_action.frame = next_action.action->frame_count - 1;
  }
  next_action.speed = speed;
  next_action.rot = frame2.rot;
  actions_switched = false;

  const Frame* frame = next_frame.action != 0 ? &next_frame : &frame2;
  abort_at_frame.action = frame->action;
  abort_at_frame.speed = frame->speed;
  abort_at_frame.rot = frame->rot;
  if(frame->speed >= 0) {
      abort_at_frame.frame = frame->action->frame_count - 1;
  } else {
      abort_at_frame.frame = 0;
  }
}

void
Sprite3D::set_next_rot(bool rot)
{
  next_action.rot = rot;
}

void
Sprite3D::abort_at_marker(const std::string& name)
{
  const Marker& marker = data->get_marker(frame1.action, name);
  abort_at_frame = frame1;
  abort_at_frame.frame = marker.frame;
}

bool
Sprite3D::after_marker(const std::string& name) const
{
  const Marker& marker = data->get_marker(frame1.action, name);  
  return frame1.frame >= marker.frame;
}

bool
Sprite3D::switched_actions()
{
  if(actions_switched) {
    actions_switched = false;
    return true;
  }

  return false;
}

void
Sprite3D::set_speed(float speed)
{
  if(speed < 0 && frame1.speed >= 0
      || speed >= 0 && frame1.speed < 0) {
    blend_time = 1.0 - blend_time;
    std::swap(frame1, frame2);
  }
  frame1.speed = speed;
  frame2.speed = speed;
}

float
Sprite3D::get_speed() const
{
  return frame1.speed;
}

void
Sprite3D::set_rot(bool rot)
{
  next_frame.rot = rot;
}

bool
Sprite3D::get_rot() const
{
  if(next_frame.action != 0)
    return next_frame.rot;
  
  return frame1.rot;
}

class Sprite3DDrawingRequest : public DrawingRequest
{
private:
  Sprite3D* sprite;

public:
  Sprite3DDrawingRequest(Sprite3D* sprite, const Vector& pos,
                         const Matrix& modelview)
      : DrawingRequest(pos, modelview), sprite(sprite)
  {
  }

  void draw(CL_GraphicContext* gc)
  {
    sprite->draw(gc, pos, modelview);
  }
};

void
Sprite3D::set_next_frame()
{
  if(frame2.action != frame1.action && abort_at_frame.action == 0) {
    actions_switched = true;
  }
  
  frame1 = frame2;
  if(next_frame.action != 0) {
    frame2 = next_frame;
    next_frame.action = 0;
    return;
  }
  if(frame2 == abort_at_frame && next_action.action != 0) {
    frame2 = next_action;
    abort_at_frame.action = 0;
    next_action.action = 0;
    return;
  }

  frame2.action = frame1.action;
  if(frame1.speed < 0) {
    frame2.frame = (frame1.frame + frame1.action->frame_count - 1)
      % frame2.action->frame_count;
  } else {
    frame2.frame = (frame1.frame + 1) % frame1.action->frame_count;
  }
  frame2.speed = frame1.speed;
  frame2.rot = frame1.rot;
}

void
Sprite3D::update(float elapsed_time)
{
  float time_delta = elapsed_time * frame1.action->speed * frame1.speed;
  if(frame1.speed < 0)
    time_delta = -time_delta;

  while(blend_time + time_delta >= 1.0) {
    elapsed_time -= (1.0 - blend_time) / (frame1.action->speed * frame1.speed);
    set_next_frame();

    time_delta = elapsed_time * frame1.action->speed * frame1.speed;
    blend_time = 0.0;
  }
  blend_time += time_delta;
}

void
Sprite3D::draw(SceneContext& sc, const Vector& pos)
{
  sc.color().draw(
    new Sprite3DDrawingRequest(this, pos, sc.color().get_modelview()));
}

void
Sprite3D::draw(CL_GraphicContext* gc, const Vector& pos,
               const Matrix& modelview)
{
  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  glPushMatrix();
  glMultMatrixd(modelview);
  glTranslatef(pos.x, pos.y, pos.z);
  if(frame1.rot) {
    glRotatef(180, 0, 1.0, 0);
  }                           

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);  

  assert_gl("gl init before sprite");

  const ActionFrame& aframe1 = frame1.action->frames[frame1.frame];
  const ActionFrame& aframe2 = frame2.action->frames[frame2.frame];
  
  float t_1 = 1.0 - blend_time;
  for(uint16_t m = 0; m < data->mesh_count; ++m) {
    const Mesh& mesh = data->meshs[m];
    const MeshVertices& vertices1 = aframe1.meshs[m];
    const MeshVertices& vertices2 = aframe2.meshs[m];
    
    // blend between frame1 + frame2
    float* verts = new float[mesh.vertex_count * 3];
    if(frame1.rot == frame2.rot) {
      for(uint16_t v = 0; v < mesh.vertex_count*3; ++v) {
        verts[v] 
          = vertices1.vertices[v] * t_1 + vertices2.vertices[v] * blend_time;
      }
    } else {
      // need to manually rotate 180 degree here because frames have different
      // rot values (=> x=-x, y=y, z=-z)
      for(uint16_t v = 0; v < mesh.vertex_count; ++v) {
        verts[v*3] 
          = vertices1.vertices[v*3] * t_1 
              - vertices2.vertices[v*3] * blend_time;
        verts[v*3+1]
          = vertices1.vertices[v*3+1] * t_1 
              + vertices2.vertices[v*3+1] * blend_time;
        verts[v*3+2]
          = vertices1.vertices[v*3+2] * t_1
              - vertices2.vertices[v*3+2] * blend_time;
      }
    }
   
    // draw mesh
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

