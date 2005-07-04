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

#include <vector>
#include <stdint.h>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include <physfs.h>
#include "display/drawing_request.hpp"
#include "display/scene_context.hpp"
#include "sprite3d.hpp"
#include "lisp_util.hpp"
#include "globals.hpp"
#include "util.hpp"
#include "timer.hpp"

struct Mesh
{
  ~Mesh() {
    delete[] vertex_indices;
    delete[] tex_coords;
  }

  CL_OpenGLSurface texture;
  uint16_t triangle_count;
  uint16_t* vertex_indices;
  float* tex_coords;
  float* normals;
  uint16_t vertex_count;
};

struct MeshVertices
{
  ~MeshVertices() {
    delete[] vertices;
  }
  float* vertices;
};

struct ActionFrame
{
  ~ActionFrame() {
    delete[] meshs;
  }
  MeshVertices* meshs;
};

struct Action
{
  ~Action() {
    delete[] frames;
  }
  
  std::string name;
  uint16_t frame_count;
  ActionFrame* frames;
};

static inline float read_float(PHYSFS_file* file)
{
    uint32_t int_result;
    if(PHYSFS_readULE32(file, &int_result) == 0) {
        std::ostringstream msg;
        msg << "Problem reading float value: " << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }

    // is this platform independent?
    return * ( reinterpret_cast<float*> (&int_result) );
}

static inline uint16_t read_uint16_t(PHYSFS_file* file)
{
    uint16_t result;
    if(PHYSFS_readULE16(file, &result) == 0) {
        std::ostringstream msg;
        msg << "Problem reading uint16 value: " << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    return result;
}

static inline std::string read_string(PHYSFS_file* file, size_t size)
{
    char buffer[size+1];
    if(PHYSFS_read(file, buffer, size, 1) != 1) {
        std::ostringstream msg;
        msg << "Problem reading string value: " << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    buffer[size] = 0;

    return buffer;
}

Sprite3D::Sprite3D(const std::string& filename)
{
  PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
  if(!file) {
    std::ostringstream msg;
    msg << "Couldn't open '" << filename << "': "
      << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }

  try {
    char magic[4];
    if(PHYSFS_read(file, magic, sizeof(magic), 1) != 1)
      throw std::runtime_error("Couldn't read file magic");
    if(strncmp(magic, "W3DS", 4) != 0)
      throw std::runtime_error("Not a windstille 3d sprite file");


    mesh_count = read_uint16_t(file);
    action_count = read_uint16_t(file);

    // read meshs
    meshs = new Mesh[mesh_count];
    for(uint16_t i = 0; i < mesh_count; ++i) {
      Mesh& mesh = meshs[i];

      std::string texturename = read_string(file, 64);
      texturename = dirname(filename) + basename(texturename);
      
      mesh.texture = CL_OpenGLSurface(datadir + texturename);
      mesh.triangle_count = read_uint16_t(file);
      mesh.vertex_count = read_uint16_t(file);

      printf("Reading Mesh Tex %s Tri %u Vs %u.\n", texturename.c_str(),
              mesh.triangle_count, mesh.vertex_count);

      // read triangles
      mesh.vertex_indices = new uint16_t[mesh.triangle_count * 3];
      for(uint16_t v = 0; v < mesh.triangle_count * 3; ++v) {
        mesh.vertex_indices[v] = read_uint16_t(file);
      }
      
      mesh.normals = new float[mesh.triangle_count * 3];
      for(uint16_t n = 0; n < mesh.triangle_count * 3; ++n) {
        mesh.normals[n] = read_float(file);
      }

      mesh.tex_coords = new float[mesh.vertex_count * 2];
      for(uint16_t v = 0; v < mesh.vertex_count * 2; ++v) {
        mesh.tex_coords[v] = read_float(file);
      }
    }

    // read actions
    actions = new Action[action_count];
    for(uint16_t i = 0; i < action_count; ++i) {
      Action& action = actions[i];

      action.name = read_string(file, 64);
      action.frame_count = read_uint16_t(file);

      printf("ReadingAction %s Frames %u.\n", action.name.c_str(), action.frame_count);

      // read frames
      action.frames = new ActionFrame[action.frame_count];
      for(uint16_t f = 0; f < action.frame_count; ++f) {
        ActionFrame& frame = action.frames[f];
        frame.meshs = new MeshVertices[mesh_count];

        for(uint16_t m = 0; m < mesh_count; ++m) {
          MeshVertices& mesh = frame.meshs[m];

          mesh.vertices = new float[meshs[m].vertex_count * 3];
          for(uint16_t v = 0; v < meshs[m].vertex_count * 3; ++v) {
            mesh.vertices[v] = read_float(file);
          }
        }
      }
    }

  } catch(std::exception& e) {
    PHYSFS_close(file);
    std::ostringstream msg;
    msg << "Problem while reading '" << filename << "': "
      << e.what();
    throw std::runtime_error(msg.str());
  }
  PHYSFS_close(file);
}

Sprite3D::~Sprite3D()
{
  delete[] meshs;
  delete[] actions;
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
      const CL_Vector& pos, const CL_Matrix4x4& modelview)
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
Sprite3D::draw(SceneContext& sc)
{
  float animspeed = 3.0;
  int frame = (int) fmodf(game_time * animspeed, actions[0].frame_count);
  int nextframe = (int) fmodf(game_time * animspeed + 1.0,
                              actions[0].frame_count);
  float time = fmodf(game_time*animspeed, 1.0);
  sc.color().draw(new Sprite3DDrawingRequest(this,
        &actions[0].frames[frame], &actions[0].frames[nextframe], time,
        CL_Vector(12*32, 26*32, 100), sc.color().get_modelview()));
}

void
Sprite3D::render_frame(CL_GraphicContext* gc, const ActionFrame* frame,
    const CL_Vector& pos, const CL_Matrix4x4& modelview)
{
  //printf("RenderFrame.\n");

  assert_gl("before render_frame");
  
  static float angle = 0;
  angle += 1;
  
  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  glPushMatrix();
  glMultMatrixd(modelview);
  glTranslatef(pos.x, pos.y, pos.z);
  // just a test
  glRotatef(angle, 0, 1.0, 0);
  
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);  

  assert_gl("gl init before sprite");
  
  for(uint16_t m = 0; m < mesh_count; ++m) {
    const Mesh& mesh = meshs[m];
    const MeshVertices& vertices = frame->meshs[m];
 
    CL_OpenGLSurface& texture = const_cast<CL_OpenGLSurface&> (mesh.texture);
    texture.bind();

    glVertexPointer(3, GL_FLOAT, 0, vertices.vertices);
    glNormalPointer(GL_FLOAT, 0, mesh.normals);
    glTexCoordPointer(2, GL_FLOAT, 0, mesh.tex_coords);

    glDrawElements(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT,
        mesh.vertex_indices);
  }

  assert_gl("rendering 3d sprite");      

  glPopMatrix();
}

void
Sprite3D::blend_frames(CL_GraphicContext* gc, const ActionFrame* frame1,
    const ActionFrame* frame2, float time,
    const CL_Vector& pos, const CL_Matrix4x4& modelview)
{
  assert_gl("before render_frame");
  
  static float angle = 0;
  angle += 1;
  
  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  glPushMatrix();
  glMultMatrixd(modelview);
  glTranslatef(pos.x, pos.y, pos.z);
  // just a test
  glRotatef(angle, 0, 1.0, 0);
  
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);  

  assert_gl("gl init before sprite");
  
  float t_1 = 1.0 - time;
  for(uint16_t m = 0; m < mesh_count; ++m) {
    const Mesh& mesh = meshs[m];
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

