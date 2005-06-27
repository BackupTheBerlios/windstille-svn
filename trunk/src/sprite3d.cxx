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
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/list_iterator.hpp"
#include "display/drawing_request.hxx"
#include "display/scene_context.hxx"
#include "sprite3d.hxx"
#include "lisp_util.hpp"
#include "globals.hxx"

struct Vertex
{
  CL_Vector pos;
  CL_Vector normal;
};

struct Vert
{
  /** Vertex index */
  unsigned int index;
  float u;
  float v;
};

struct Face
{
  // Vertices
  Vert v[3];
};

static inline void read_vector(const lisp::Lisp* lisp, CL_Vector& vec)
{
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vec.x = lisp->get_car()->get_float();
  if(lisp->get_cdr() == 0)
    throw std::runtime_error("Must specified 3 floats for CL_vector");
  lisp = lisp->get_cdr();
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vec.y = lisp->get_car()->get_float();
  if(lisp->get_cdr() == 0)
    throw std::runtime_error("Must specified 3 floats for CL_vector");
  lisp = lisp->get_cdr();
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vec.z = lisp->get_car()->get_float();
}

static void read_vert(const lisp::Lisp* lisp, Vert& vert)
{
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vert.index = lisp->get_car()->get_int();
  if(lisp->get_cdr() == 0)
    throw std::runtime_error("Must specified 3 floats for CL_vector");
  lisp = lisp->get_cdr();
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vert.u = lisp->get_car()->get_float();
  if(lisp->get_cdr() == 0)
    throw std::runtime_error("Must specified 3 floats for CL_vector");
  lisp = lisp->get_cdr();
  if(lisp->get_type() != lisp::Lisp::TYPE_CONS || lisp->get_car() == 0)
    throw std::runtime_error("Invalid data when reading CL_Vector");
  vert.v = lisp->get_car()->get_float();
}

static void read_face(const lisp::Lisp* lisp, Face& face)
{
  lisp::ListIterator iter(lisp);
  int i = 0;
  while(iter.next()) {
    if(iter.item() == "vert") {
      if(i >= 3)
        throw std::runtime_error("Too many vertices for face (should be 3)");
      read_vert(iter.lisp(), face.v[i]);
      i++;
    } else {
      std::cerr << "Skipping unknown tag '" << iter.item() << "' in face.\n";
    }
  }
  if(i < 2) {
    throw std::runtime_error("Too few vertices in face (should be 3)");
  }
}

class Sprite3DImpl
{
public:
  typedef std::vector<Vertex> Vertices;
  typedef std::vector<Face>   Faces;

  Vertices vertices;
  Faces    faces;
  CL_OpenGLSurface surface;

  GLuint buffer_id;
  typedef std::vector<float> Floats;
  Floats raw_data;
  int    normals_offset;
  int    texcoord_offset;

  float angle;

  Sprite3DImpl()
    : buffer_id(0), angle(0)
  {
  }

  ~Sprite3DImpl()
  {
    /* FIXME: is this the same?
       int buffer[1] = {buffer_id};
       DeleteBuffersARB(1, buffer);
    */
    clDeleteBuffers(1, &buffer_id);
  }

  void create_vbo()
  {
    Floats raw_texcoords;
    Floats raw_normals;
    Floats raw_vertices;

    for(Sprite3DImpl::Faces::iterator i = faces.begin(); i != faces.end(); ++i)
      {
        const Face& face = *i;
        for(int v = 0; v < 3; ++v) {
          const Vert& vert = face.v[v];

          raw_texcoords.push_back(vert.u);
          raw_texcoords.push_back(vert.v);

          const Vertex& vertex = vertices[vert.index];

          raw_normals.push_back(vertex.normal.x);
          raw_normals.push_back(vertex.normal.y);
          raw_normals.push_back(vertex.normal.z);

          raw_vertices.push_back(vertex.pos.x);
          raw_vertices.push_back(vertex.pos.y);
          raw_vertices.push_back(vertex.pos.z);
        }
      }

    std::copy(raw_vertices.begin(),  raw_vertices.end(),  std::back_inserter(raw_data));
    std::copy(raw_normals.begin(),   raw_normals.end(),   std::back_inserter(raw_data));
    std::copy(raw_texcoords.begin(), raw_texcoords.end(), std::back_inserter(raw_data));

    normals_offset  = raw_vertices.size() * sizeof(float);
    texcoord_offset = normals_offset + raw_normals.size() * sizeof(float);

    clGenBuffers(1, &buffer_id);
    clBindBuffer(CL_ARRAY_BUFFER, buffer_id);
    clBufferData(CL_ARRAY_BUFFER, raw_data.size() * sizeof(float), &*raw_data.begin(), CL_STATIC_DRAW);
  }

  void parse_file(const std::string& filename)
  {
    std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));

    const lisp::Lisp* spritelisp = root->get_lisp("windstille-3dsprite");
    if(!spritelisp) {
      std::ostringstream msg;
      msg << "'" << filename << "' is not a windstille-3dsprite file";
      throw std::runtime_error(msg.str());
    }

    lisp::ListIterator iter(spritelisp);
    while(iter.next()) {
      if(iter.item() == "texture") {
        surface = CL_OpenGLSurface(datadir + iter.value().get_string());
      } else if(iter.item() == "vertices") {
        lisp::ListIterator vertices_iter(iter.lisp());
        while(vertices_iter.next()) {
          if(vertices_iter.item() == "vertex") {
            const lisp::Lisp* vlisp = vertices_iter.lisp();
            Vertex vertex;
            const lisp::Lisp* poslisp = vlisp->get_lisp("pos");
            if(poslisp == 0)
              throw std::runtime_error("Vertex without pos found");
            read_vector(poslisp, vertex.pos);
            const lisp::Lisp* normallisp = vlisp->get_lisp("normal");
            if(normallisp == 0)
              throw std::runtime_error("Vertex without normal found");
            read_vector(normallisp, vertex.normal);
                        
            vertices.push_back(vertex);
          } else {
            std::cerr << "Skipping unknown tag '" 
                      << vertices_iter.item() << "' in vertices\n";
          }
        }
      } else if(iter.item() == "faces") {
        lisp::ListIterator faces_iter(iter.lisp());
        while(faces_iter.next()) {
          if(faces_iter.item() == "face") {
            Face face;
            read_face(faces_iter.lisp(), face);
            faces.push_back(face);                        
          } else {
            std::cerr << "Skipping unknown tag '"
                      << faces_iter.item() << "' in faces\n";
          }
        }
      } else {
        std::cerr << "Skipping unknown tag '"
                  << iter.item() << "' in sprite3d\n";
      }
    }

    create_vbo();
  }
};

Sprite3D::Sprite3D(const std::string& filename)
  : impl(new Sprite3DImpl)
{
  impl->parse_file(filename);
}

Sprite3D::~Sprite3D()
{
  delete impl;
}

void
Sprite3D::update(float delta)
{
  impl->angle += 30.0f* delta;
}

class Sprite3DDrawingRequest : public DrawingRequest
{
private:
  Sprite3DImpl* impl;

public:
  Sprite3DDrawingRequest(Sprite3DImpl* impl_,
                         const CL_Vector& pos, const CL_Matrix4x4& modelview = CL_Matrix4x4(true))
    : DrawingRequest(pos, modelview), 
      impl(impl_)
  {
  }

  virtual ~Sprite3DDrawingRequest() {}

  void draw_with_vbo(CL_GraphicContext* gc)
  {
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

    if (0)
      {
        clBindBuffer(CL_ARRAY_BUFFER, impl->buffer_id);

        clVertexPointer  (3, CL_FLOAT, 0, BUFFER_OFFSET(0));
        clNormalPointer     (CL_FLOAT, 0, BUFFER_OFFSET(impl->normals_offset));
        clTexCoordPointer(2, CL_FLOAT, 0, BUFFER_OFFSET(impl->texcoord_offset));
    
        // Enable arrays
        clEnableClientState(CL_TEXTURE_COORD_ARRAY);
        clEnableClientState(CL_NORMAL_ARRAY);
        clEnableClientState(CL_VERTEX_ARRAY);

        std::cout << "DRaw" << std::endl;
        // Draw arrays
        clDrawArrays(CL_TRIANGLES, 0, impl->faces.size()*3);
        std::cout << "DRaw1" << std::endl;

        // Disable arrays
        clDisableClientState(CL_TEXTURE_COORD_ARRAY);
        clDisableClientState(CL_NORMAL_ARRAY);
        clDisableClientState(CL_VERTEX_ARRAY);
      }
    else
      {
        GLbyte* data = reinterpret_cast<GLbyte*>(&*impl->raw_data.begin());
        clVertexPointer  (3, CL_FLOAT, 0, data);
        clNormalPointer     (CL_FLOAT, 0, data + impl->normals_offset);
        clTexCoordPointer(2, CL_FLOAT, 0, data + impl->texcoord_offset);
    
        // Enable arrays
        clEnableClientState(CL_TEXTURE_COORD_ARRAY);
        clEnableClientState(CL_NORMAL_ARRAY);
        clEnableClientState(CL_VERTEX_ARRAY);
    
        // Draw arrays
        clDrawArrays(CL_TRIANGLES, 0, impl->faces.size()*3);

        // Disable arrays
        clDisableClientState(CL_TEXTURE_COORD_ARRAY);
        clDisableClientState(CL_NORMAL_ARRAY);
        clDisableClientState(CL_VERTEX_ARRAY);
      }
  }
  
  void draw(CL_GraphicContext* gc) 
  {
    // FIXME: This must be moved into a DrawingRequest
    CL_OpenGLState state(gc);
    state.set_active();
    state.setup_2d();
  
    glPushMatrix();

    glMultMatrixd(modelview);

    glTranslatef(pos.x, pos.y, pos.z); //pos.z);
    
    // FIXME: just for testing, remove for production
    glRotated(impl->angle, 0, 1.0, 0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    impl->surface.bind();

    //draw_classic(gc);
    draw_with_vbo(gc);

    glPopMatrix();   
  }

  void draw_classic(CL_GraphicContext* gc) 
  {
    glBegin(GL_TRIANGLES);
    for(Sprite3DImpl::Faces::iterator i = impl->faces.begin(); i != impl->faces.end(); ++i)
      {
        const Face& face = *i;
        for(int v = 0; v < 3; ++v) {
          const Vert& vert = face.v[v];
          glTexCoord2f(vert.u, vert.v); 
          const Vertex& vertex = impl->vertices[vert.index];
          glNormal3f(vertex.normal.x, vertex.normal.y, vertex.normal.z);
          glVertex3f(vertex.pos.x, vertex.pos.y, vertex.pos.z);
        }
      }
    glEnd();
  }
};

void
Sprite3D::draw(SceneContext& sc)
{
  sc.color().draw(new Sprite3DDrawingRequest(impl, CL_Vector(12*32, 26*32, 100), sc.color().get_modelview()));
}

/* EOF */
