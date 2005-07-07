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
  : data(data), rot(false), actions_switched(false), next_action(0), next_rot(0)
{
  current_action = &data->actions[0];
  last_frame = current_action->frame_count - 1;
  animation_time = 0;
  speed = 1.0;
  update(0);
  
  frame1 = &current_action->frames[0];
  frame2 = &current_action->frames[1];
}

Sprite3D::~Sprite3D()
{
}

void
Sprite3D::set_action(const std::string& actionname)
{
  current_action = & data->get_action(actionname);
  animation_time = 0;
  last_frame = current_action->frame_count - 1;
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
  
  int frame = static_cast<int>(animation_time) % current_action->frame_count;
  
  return frame >= marker.frame;
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
  if(this->speed > 0 && speed < 0) {
    this->last_frame = 0;
    if(animation_time >= current_action->frame_count - 1)
      animation_time = current_action->frame_count - 1.0001;
  } else if(this->speed < 0 && speed > 0) {
    this->last_frame = current_action->frame_count - 1;
    if(animation_time >= current_action->frame_count - 1)
      animation_time = current_action->frame_count - 1.0001;
  }
  if(speed < 0 && animation_time == 0) {
    // don't produce an action switch right away when starting backwards
    animation_time = current_action->frame_count - 1.0001;
  }

  this->speed = speed;
}

float
Sprite3D::get_speed() const
{
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
Sprite3D::update(float elapsed_time)
{
  float animation_time_delta = elapsed_time * current_action->speed * speed;

  if(speed >= 0) {
    int frame = static_cast<int>(animation_time);
    frame1 = &current_action->frames[frame];
    blend_time = fmodf(animation_time, 1.0);
    assert(frame >= 0);
    assert(frame < current_action->frame_count);

    // last frame
    if(frame >= last_frame) {
      if(next_action != 0) {
        if(next_speed > 0) {
          frame2 = &next_action->frames[0];
        } else {
          frame2 = &next_action->frames[next_action->frame_count - 1];
        }

        // time to switch actions?
        if(blend_time + animation_time_delta >= 1.0) {
          switch_next_action();
          animation_time = blend_time + animation_time_delta - 1.0;
        }
      } else {
        frame2 = &current_action->frames[0];
        if(blend_time + animation_time >= 1.0) {
          animation_time -= current_action->frame_count;
        }
      }
    } else {
      assert(frame + 1 < current_action->frame_count);
      frame2 = &current_action->frames[frame + 1];
    }
  } else {
    int frame 
      = (static_cast<int>(animation_time) + 1) % current_action->frame_count;
    frame1 = &current_action->frames[frame];
    blend_time = 1.0 - fmodf(animation_time, 1.0);
    assert(frame >= 0);
    assert(frame < current_action->frame_count);

    // last frame
    if(frame <= last_frame) {
      if(next_action != 0) {
        if(next_speed > 0) {
          frame2 = &next_action->frames[0];
        } else {
          frame2 = &next_action->frames[next_action->frame_count - 1];
        }

        // time to switch actions?
        if(blend_time - animation_time_delta >= 1.0) {
          switch_next_action();
          animation_time = static_cast<float>(current_action->frame_count) 
            - (blend_time - animation_time_delta - 1.0);
        }
      } else {
        frame2 = &current_action->frames[current_action->frame_count - 1];
      }
    } else {
      int fr2 = (frame + current_action->frame_count - 1) 
        % current_action->frame_count;
      frame2 = &current_action->frames[fr2];
      if(animation_time + animation_time_delta <= 0.0) {
        animation_time += current_action->frame_count;
      }                                                     
    }
  }

  animation_time += animation_time_delta;
}

void
Sprite3D::switch_next_action()
{
  current_action = next_action;
  speed = next_speed;
  rot = next_rot;
  next_action = 0;
  actions_switched = true;

  if(speed >= 0)
    last_frame = current_action->frame_count - 1;
  else
    last_frame = 0;
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
  if(rot) {
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
    const MeshVertices& vertices1 = frame1->meshs[m];
    const MeshVertices& vertices2 = frame2->meshs[m];
    
    // blend between frame1 + frame2
    float* verts = new float[mesh.vertex_count * 3];
    for(uint16_t v = 0; v < mesh.vertex_count*3; ++v) {
      verts[v] 
        = vertices1.vertices[v] * t_1 + vertices2.vertices[v] * blend_time;
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

