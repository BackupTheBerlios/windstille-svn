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

#ifndef HEADER_SPRITE3D_DATA_HPP
#define HEADER_SPRITE3D_DATA_HPP

#include <stdint.h>
#include <string>
#include <GL/gl.h>
#include "ref.hpp"
#include "glutil/texture.hpp"
#include "math/vector3.hpp"
#include "math/quaternion.hpp"

namespace sprite3d
{

struct Mesh;
struct AttachementPoint;
struct MeshVertices;
struct ActionFrame;
struct Action;
struct Marker;

/**
 * This class holds the data of a .wsprite file.
 */
class Data
{
public:
  Data(const std::string& filename);
  ~Data();

  const Action& get_action(const std::string& name) const;
  const Marker& get_marker(const Action* action, const std::string& name) const;
  uint16_t get_attachement_point_id(const std::string& name) const;

  uint16_t mesh_count;
  Mesh* meshs;
  uint16_t attachement_point_count;
  AttachementPoint* attachement_points;
  uint16_t action_count;
  Action* actions;

private:
  void clear();
  
  Data (const Data&);
  Data& operator= (const Data&);
};

struct Mesh
{
  Mesh();

  Texture texture;
  uint16_t triangle_count;
  uint16_t* vertex_indices;
  float* tex_coords;
  float* normals;
  uint16_t vertex_count;
};

struct AttachementPoint
{
  std::string name;
};

struct MeshVertices
{
  MeshVertices()
    : vertices(0)
  { }
  float* vertices;
};

struct AttachementPointPosition
{
  Vector3 pos; // x, y, z
  Quaternion quat; // w, x, y, z
};

struct ActionFrame
{
  ActionFrame()
    : meshs(0), attachement_points(0)
  { }
  MeshVertices* meshs;
  AttachementPointPosition* attachement_points;
};

struct Marker
{
  std::string name;
  uint16_t frame;
};

struct Action
{
  Action()
    : markers(0), frames(0)
  { }
  std::string name;
  float speed;
  uint16_t marker_count;
  Marker* markers;
  uint16_t frame_count;
  ActionFrame* frames;
};

}

#endif
