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
#include "lispreader.hxx"
#include "display/drawing_request.hxx"
#include "display/scene_context.hxx"
#include "sprite3d.hxx"
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
  Vert v1;
  Vert v2;
  Vert v3;
};

class Sprite3DImpl
{
public:
  typedef std::vector<Vertex> Vertices;
  typedef std::vector<Face>   Faces;

  Vertices vertices;
  Faces    faces;
  CL_OpenGLSurface surface;

  float angle;

  Sprite3DImpl()
    : angle(0)
  {
  }

  void parse_file(const std::string& filename)
  {
    lisp_object_t* tree = lisp_read_from_file(filename.c_str());

    if (tree && strcmp(lisp_symbol(lisp_car(tree)), "windstille-3dsprite") != 0)
      {
        std::cout << filename << ": not a Windstille 3DSprite file, type='" << lisp_symbol(lisp_car(tree)) << "'!" << std::endl;
      }
    else
      {
        LispReader reader(lisp_cdr(tree));

        std::string texture;
        if (reader.read_string("texture",  &texture))
          {
            surface = CL_OpenGLSurface(datadir + texture);
          }
        
        lisp_object_t* vertices_ptr = 0;
        if (reader.read_lisp("vertices", &vertices_ptr))
          {
            while(!lisp_nil_p(vertices_ptr))
              {
                lisp_object_t* data = lisp_car(vertices_ptr);
                if (lisp_cons_p(data) && lisp_symbol_p(lisp_car(data)))
                  {
                    std::string ident = lisp_symbol(lisp_car(data));
                    
                    if (ident == "vertex")
                      {
                        Vertex vertex;
                        
                        LispReader r(lisp_cdr(data));

                        r.read_vector("pos",    &vertex.pos);
                        r.read_vector("normal", &vertex.normal);
                        
                        vertices.push_back(vertex);
                      }
                    else
                      {
                        
                      }
                  }

                vertices_ptr = lisp_cdr(vertices_ptr);
              }
          }

        lisp_object_t* faces_ptr = 0;
        if (reader.read_lisp("faces", &faces_ptr))
          {
            while(!lisp_nil_p(faces_ptr))
              {
                lisp_object_t* data = lisp_car(faces_ptr);
                if (lisp_cons_p(data) && lisp_symbol_p(lisp_car(data)))
                  {
                    std::string ident = lisp_symbol(lisp_car(data));
                    
                    if (ident == "face")
                      {
                        Face face;
                        
                        lisp_object_t* v1 = lisp_cdr(lisp_list_nth(lisp_cdr(data), 0));
                        lisp_object_t* v2 = lisp_cdr(lisp_list_nth(lisp_cdr(data), 1));
                        lisp_object_t* v3 = lisp_cdr(lisp_list_nth(lisp_cdr(data), 2));


                        face.v1.index = lisp_integer(lisp_list_nth(v1, 0));
                        face.v1.u     =    lisp_real(lisp_list_nth(v1, 1));
                        face.v1.v     =    lisp_real(lisp_list_nth(v1, 2));

                        face.v2.index = lisp_integer(lisp_list_nth(v2, 0));
                        face.v2.u     =    lisp_real(lisp_list_nth(v2, 1));
                        face.v2.v     =    lisp_real(lisp_list_nth(v2, 2));

                        face.v3.index = lisp_integer(lisp_list_nth(v3, 0));
                        face.v3.u     =    lisp_real(lisp_list_nth(v3, 1));
                        face.v3.v     =    lisp_real(lisp_list_nth(v3, 2));


                        //std::cout << "Face: " << face.v1.index << " " << face.v1.u << " " << face.v1.v << "\n"
                        //          << "      " << face.v2.index << " " << face.v2.u << " " << face.v2.v << "\n"
                        //          << "      " << face.v3.index << " " << face.v3.u << " " << face.v3.v << "\n";
                          
                        faces.push_back(face);
                      }
                    else
                      {
                        
                      }
                  }

                faces_ptr = lisp_cdr(faces_ptr);
              }
          }        
      }      

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
  
  void draw(CL_GraphicContext* gc) 
  {
    // FIXME: This must be moved into a DrawingRequest
    CL_OpenGLState state(gc);
    state.set_active();
    state.setup_2d();
  
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z); //pos.z);
    
    glScalef(2.0f, 2.0f, 2.0f); // FIXME: Dirty hack to work around the wrong near/far clip settings 

    // FIXME: just for testing, remove for production
    glRotated(impl->angle, 0, 1.0, 1.0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    impl->surface.bind();

    glBegin(GL_TRIANGLES);
    for(Sprite3DImpl::Faces::iterator i = impl->faces.begin(); i != impl->faces.end(); ++i)
      {
        glTexCoord2f(i->v1.u, i->v1.v); 
        glNormal3f(impl->vertices[i->v1.index].normal.x, impl->vertices[i->v1.index].normal.y, impl->vertices[i->v1.index].normal.z);
        glVertex3f(impl->vertices[i->v1.index].pos.x,
                   impl->vertices[i->v1.index].pos.y,
                   impl->vertices[i->v1.index].pos.z);

        glTexCoord2f(i->v2.u, i->v2.v); 
        glNormal3f(impl->vertices[i->v2.index].normal.x, impl->vertices[i->v2.index].normal.y, impl->vertices[i->v2.index].normal.z);
        glVertex3f(impl->vertices[i->v2.index].pos.x,
                   impl->vertices[i->v2.index].pos.y,
                   impl->vertices[i->v2.index].pos.z);

        glTexCoord2f(i->v3.u, i->v3.v);
        glNormal3f(impl->vertices[i->v3.index].normal.x, impl->vertices[i->v3.index].normal.y, impl->vertices[i->v3.index].normal.z); 
        glVertex3f(impl->vertices[i->v3.index].pos.x,
                   impl->vertices[i->v3.index].pos.y,
                   impl->vertices[i->v3.index].pos.z);
      }
    glEnd();

    glPopMatrix();  
  }
};

void
Sprite3D::draw(SceneContext& sc)
{
  sc.color().draw(new Sprite3DDrawingRequest(impl, CL_Vector(250, 250, 100)));
}

/* EOF */
