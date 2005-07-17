#include <config.h>

#include "sprite3d/data.hpp"

#include <physfs.h>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include "util.hpp"
#include "globals.hpp"
#include "glutil/texture_manager.hpp"
#include "glutil/texture.hpp"

namespace sprite3d
{

static const int FORMAT_VERSION = 2;

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

Data::Data(const std::string& filename)
  : mesh_count(0), meshs(0), bone_count(0), bones(0),
  action_count(0), actions(0)
{
  PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
  if(!file) {
    std::ostringstream msg;
    msg << "Couldn't open '" << filename << "': "
      << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }

  try {
    std::string magic = read_string(file, 4);
    if(magic != "W3DS")
      throw std::runtime_error("Not a windstille 3d sprite file");
    uint16_t format_version = read_uint16_t(file);
    if(format_version > FORMAT_VERSION)
      throw std::runtime_error("sprite file format too new");
    if(format_version < FORMAT_VERSION)
      throw std::runtime_error("sprite file format too old");

    mesh_count = read_uint16_t(file);
    if(mesh_count == 0)
      throw std::runtime_error("Sprite contains no meshs");
    bone_count = read_uint16_t(file);
    action_count = read_uint16_t(file);
    if(action_count == 0)
      throw std::runtime_error("Sprite contains no actions");

    // read meshs
    meshs = new Mesh[mesh_count];
    for(uint16_t i = 0; i < mesh_count; ++i) {
      Mesh& mesh = meshs[i];

      std::string texturename = read_string(file, 64);
      texturename = dirname(filename) + basename(texturename);
      mesh.triangle_count = read_uint16_t(file);
      mesh.vertex_count = read_uint16_t(file);

      printf("Reading Mesh Tex %s Tri %u Vs %u.\n", texturename.c_str(),
              mesh.triangle_count, mesh.vertex_count);

      const Texture* texture = texture_manager->get(texturename);
      mesh.texture = texture->handle;

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

    // read bones
    bones = new Bone[bone_count];
    for(uint16_t b = 0; b < bone_count; ++b) {
      Bone& bone = bones[b];
      bone.name = read_string(file, 64);
    }

    // read actions
    actions = new Action[action_count];
    for(uint16_t i = 0; i < action_count; ++i) {
      Action& action = actions[i];

      action.name = read_string(file, 64);
      action.speed = read_float(file);
      action.marker_count = read_uint16_t(file);
      action.frame_count = read_uint16_t(file);

      printf("ReadingAction %s Frames %u.\n", action.name.c_str(), action.frame_count);

      // read markers
      action.markers = new Marker[action.marker_count];
      for(uint16_t m = 0; m < action.marker_count; ++m) {
        Marker& marker = action.markers[m];
        marker.name = read_string(file, 64);
        marker.frame = read_uint16_t(file);
        printf("Marker '%s' at %u.\n", marker.name.c_str(), marker.frame);
      }

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

        frame.bones = new BonePosition[bone_count];
        for(uint16_t b = 0; b < bone_count; ++b) {
          BonePosition& bone = frame.bones[b];
          for(int i = 0; i < 3; ++i)
            bone.pos[i] = read_float(file);
          for(int i = 0; i < 4; ++i)
            bone.quat[i] = read_float(file);
        }
      }
    }
  } catch(std::exception& e) {
    clear();
    PHYSFS_close(file);
    std::ostringstream msg;
    msg << "Problem while reading '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }
  PHYSFS_close(file);
}

Data::~Data()
{
  clear();
}

void
Data::clear()
{
  if(meshs != 0) {
    for(uint16_t m = 0; m < mesh_count; ++m) {
      Mesh& mesh = meshs[m];
      delete[] mesh.vertex_indices;
      delete[] mesh.tex_coords;
      delete[] mesh.normals;
    }
    delete[] meshs;
    meshs = 0;
  }
  
  delete[] bones;
  bones = 0;

  if(actions != 0) {
    for(uint16_t a = 0; a < action_count; ++a) {
      Action& action = actions[a];
      delete[] action.markers;      
      if(action.frames == 0)
        continue;
      
      for(uint16_t f = 0; f < action.frame_count; ++f) {
        ActionFrame& frame = action.frames[f];
        if(frame.meshs == 0)
          continue;
        for(uint16_t m = 0; m < mesh_count; ++m) {
          MeshVertices& vertices = frame.meshs[m];
          delete[] vertices.vertices;
        }
        delete[] frame.meshs;
        delete[] frame.bones;
      }
      delete[] action.frames;
    }
    delete[] actions;
  }
  mesh_count = 0;
  action_count = 0;
}

const Action&
Data::get_action(const std::string& name) const
{
  for(uint16_t a = 0; a < action_count; ++a) {
    if(actions[a].name == name)
      return actions[a];
  }
  std::ostringstream msg;
  msg << "No action with name '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

const Marker&
Data::get_marker(const Action* action, const std::string& name) const
{
  for(uint16_t m = 0; m < action->marker_count; ++m) {
    if(action->markers[m].name == name)
      return action->markers[m];
  }
  std::ostringstream msg;
  msg << "No marker with name '" << name << "' defined in action '"
      << action->name << "'";
  throw std::runtime_error(msg.str());
}

uint16_t
Data::get_bone_id(const std::string& name) const
{
  for(uint16_t b = 0; b < bone_count; ++b) {
    if(bones[b].name == name)
      return b;
  }

  std::ostringstream msg;
  msg << "No bone with name '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

}

