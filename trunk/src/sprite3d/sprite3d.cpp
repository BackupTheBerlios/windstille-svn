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
  : data(data), rot(false), actions_switched(false),
    next_action(0), next_rot(0), next_speed(1.0)
{
  current_action = &data->actions[0];
  last_frame = current_action->frame_count - 1;
  reverse = false;
  speed = 1.0;
  update(0);  
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(const std::string& actionname)
{
  current_action = & data->get_action(actionname);
  last_frame = current_action->frame_count - 1;
  next_action = 0;
}

const std::string&
Sprite3D::get_action() const
{
  return current_action->name;
}

void
Sprite3D::set_next_action(const std::string& name)
{
  next_action = & data->get_action(name);
  next_rot = rot;
  next_speed = speed;
  actions_switched = false;
}

void
Sprite3D::set_next_rot(bool rot)
{
  next_rot = rot;
}

void
Sprite3D::set_next_speed(float speed)
{
  next_speed = speed;
}

void
Sprite3D::abort_at_marker(const std::string& name)
{
  const Marker& marker = data->get_marker(current_action, name);
  last_frame = marker.frame;
}

bool
Sprite3D::after_marker(const std::string& name) const
{
  const Marker& marker = data->get_marker(current_action, name);  
  return current_frame >= marker.frame;
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
  if(this->speed > 0 && speed < 0
      || this->speed < 0 && speed > 0) {
    if(speed >= 0) {
      current_frame = (current_frame + 1) & current_action->frame_count;
      this->last_frame = 0;
    } else {
      current_frame = (current_frame + current_action->frame_count - 1)
        % current_action->frame_count;
      this->last_frame = current_action->frame_count - 1;
    }
    blend_time = 1.0 - blend_time;
    std::swap(frame1, frame2);
  }
  if(speed >= 0) {
    reverse = false;
  } else {
    speed = -speed;
    reverse = true;
  }
  this->speed = speed;
}

float
Sprite3D::get_speed() const
{
  if(reverse)
    return -speed;
  else
    return speed;
}

void
Sprite3D::set_rot(bool rot)
{
  this->rot = rot;
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
  if(current_frame == last_frame && next_action != 0) {
    current_action = next_action;
    speed = next_speed;
    if(speed < 0) {
      speed = -speed;
      reverse = true;
    } else {
      reverse = false;
    }
    rot = next_rot;
    next_action = 0;
    actions_switched = true;

    if(reverse) {
      current_frame = current_action->frame_count - 1;
      last_frame = 0;                                      
    } else {
      current_frame = 0;
      last_frame = current_action->frame_count - 1;      
    }
  } else {
    if(reverse) {
      current_frame = (current_frame + current_action->frame_count - 1) 
        % current_action->frame_count;                                         
    } else {
      current_frame = (current_frame + 1) % current_action->frame_count;
    }
  }

  assert(current_frame >= 0);
  assert(current_frame < current_action->frame_count);
  frame2.frame = &current_action->frames[current_frame];
  frame2.speed = speed;
  frame2.rot = rot;
}

void
Sprite3D::update(float elapsed_time)
{
  float time_delta = elapsed_time * current_action->speed * speed;

  while(blend_time + time_delta >= 1.0) {
    frame1 = frame2;
    elapsed_time -= (1.0 - blend_time) / (current_action->speed * speed);
    set_next_frame();

    time_delta = elapsed_time * current_action->speed * speed;
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
  
  float t_1 = 1.0 - blend_time;
  for(uint16_t m = 0; m < data->mesh_count; ++m) {
    const Mesh& mesh = data->meshs[m];
    const MeshVertices& vertices1 = frame1.frame->meshs[m];
    const MeshVertices& vertices2 = frame2.frame->meshs[m];
    
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

