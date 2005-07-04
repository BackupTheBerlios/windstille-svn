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
#ifndef HEADER_SPRITE3D_DATA_HPP
#define HEADER_SPRITE3D_DATA_HPP

#include <stdint.h>
#include <ClanLib/GL/opengl_surface.h>

struct Mesh;
struct MeshVertices;
struct ActionFrame;
struct Action;

/**
 * This class holds the data of a .wsprite file.
 */
class Sprite3DData
{
public:
  Sprite3DData(const std::string& filename);
  ~Sprite3DData();

  uint16_t mesh_count;
  Mesh* meshs;            
  uint16_t action_count;
  Action* actions;

private:
  void clear();
  
  Sprite3DData (const Sprite3DData&);
  Sprite3DData& operator= (const Sprite3DData&);
};

struct Mesh
{
  Mesh()
    : vertex_indices(0), tex_coords(0), normals(0)
  { }

  CL_OpenGLSurface texture;
  uint16_t triangle_count;
  uint16_t* vertex_indices;
  float* tex_coords;
  float* normals;
  uint16_t vertex_count;
};

struct MeshVertices
{
  MeshVertices()
    : vertices(0)
  { }
  float* vertices;
};

struct ActionFrame
{
  ActionFrame()
    : meshs(0)
  { }
  MeshVertices* meshs;
};

struct Action
{
  Action()
    : frames(0)
  { }
  std::string name;
  uint16_t frame_count;
  ActionFrame* frames;
};

#endif
