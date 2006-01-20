#!BPY

"""
Name: 'Windstille Exporter V2'
Blender: 233
Group: 'Export'
Tip: 'Export meshes/actions to windstille format'
"""

##  $Id: windstille_main.hxx,v 1.4 2003/11/07 13:00:39 grumbel Exp $
## 
##  Windstille - A Jump'n Shoot Game
##  Copyright (C) 2005 Matthias Braun <matze@braunis.de>,
##                     Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
## 
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

# Simple Python script that shows how to export animations from
# Blender
# Put this script in your $HOME/.blender/scripts directory.
#
# See windstille/docs/models.txt for more details

import struct, shlex, os.path, math
import Blender
from Blender import NMesh
from Blender import Window
from helper import *
from data import * 

# SAMPLEFRAMES, only export every nth frame of the animation
DEFAULT_SAMPLERATE = 5
# ZOOM, is multiplied with all vertex coordinates
ZOOM = 32.0
DEFAULT_SPEED = 1.0
SPEED_MULTIPLIER = 9.8
# DO NOT change this
FORMAT_VERSION = 2

class WindstilleSprite:
  ########################################################
  def __init__(self):
      """ Create a WindstilleSprite from the current scene"""
      ### References to Blender internals
      # List of Blender mesh objects that should get exported (everything on
      # seperate layer is ignored)
      self.mesh_objects    = []

      # Blender armature object
      self.armature_object = None

      # Blender attachment objects, ie. Empties
      self.attachment_objects = []

      # Name of the actions as string
      self.actions = []

      # { actionname : ActionConfig, ... }
      self.actionconfigs = {}
      
      ### Converted Data
      # List of mesh_data, ie. data we collected and converted ourself from Blender
      self.mesh_data   = []
            
      # Used to store animation data (vertex position and such)
      self.action_data = []

      ### Start collecting data
      # Collect Action Config
      self.collect_actionconfig()
      # Fill out Blender internal references
      self.collect_data()
      # Data for all Meshes
      self.collect_object_data()
      # Collect data for the Actions and Animations
      self.collect_animation_data()

  def collect_actionconfig(self):
      try:
          self.actionconfigs = ActionConfig.parse(get_text("actionconfig"))
      except Exception, message:
          raise Exception, "Error: Parse Error in actionconfig:" + str(message)
      
  def collect_data(self):
      """Convert Blender data structures into something that is used by
      this export script"""

      scene  = Blender.Scene.getCurrent()
      layers = scene.Layers

      # compose list of meshs to export
      for obj in Blender.Object.Get():
          data = obj.getData()
          if (type(data) is not Blender.Types.NMeshType) or not data.faces:
              pass # ignore anything that isn't a mesh
          elif (obj.Layers & layers) == 0:
              print "Skipping \"%s\" because it is on different layer" % obj.getName()
          else:
              texture = data.faces[0].image
              self.mesh_objects.append(obj)

      # search for armature object
      self.armature_object = Blender.Object.Get("Armature")
      if self.armature_object and self.armature_object.getType() != "Armature":
          print "Warning: object named 'Armature' is not an armature!"
          self.armature_object = None

      # compose list of objects for attachment points
      for obj in Blender.Object.Get():
          if obj.getType() == 'Empty' and obj.getName().startswith("A:"):
              self.attachment_objects.append(obj)

      # compose list of actions to export
      if not self.armature_object:
          self.actions.append("Default")
      else:
          for action in Blender.Armature.NLA.GetActions().iteritems():
              self.actions.append(action[1])
        
  ### END: def collect_data()

  def collect_animation_data(self):
      for index, action in enumerate(self.actions):
          Window.DrawProgressBar(float(index)/len(self.mesh_data)*0.5,
                                 "Collection Action %s" % (action.name))
          
          action.setActive(self.armature_object)

          # find/autodetect config
          if self.actionconfigs.has_key(action.getName()):
              actioncfg = self.actionconfigs[action.getName()]
          else:
              print "Error: No config for action '%s' defined." % action.getName()
              actioncfg = ActionConfig(1, 1, DEFAULT_SPEED, DEFAULT_SAMPLERATE, [])

          frame_data = []
          for frame in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
              Blender.Set("curframe", int(frame))
              frame_data.append(self.collect_frame_data())

          self.action_data.append(ActionData(action.getName(), actioncfg, frame_data))

  def collect_object_data(self):
    for obj in self.mesh_objects:
      self.mesh_data += self.collect_mesh_data(obj)

    # insert code to merge meshes here

    # Create the transtable
    for mesh in self.mesh_data:
      mesh.finalize()

  def collect_mesh_data(self, obj):
      """
      Returns mesh_data as dict with format:
      key: texture_filename
      val: MeshData
      """
      mesh_data = {}

      for face in obj.getData().faces:
          if face.image:
              texture_filename = face.image.filename
          else:
              texture_filename = "//404.png" # FIXME: what is // good for?

          if not mesh_data.has_key(texture_filename):
              mesh_data[texture_filename] = MeshData(texture_filename)

          faces = []
          for v in [0, 1, 2]:
              faces.append(VertexData(obj, face.v[v].index,
                                      [face.uv[v][0], 1.0-face.uv[v][1]],
                                      [face.normal[1], -face.normal[2], -face.normal[0]]))
          mesh_data[texture_filename].faces.append(FaceData(faces,
                                                            [face.normal[1], -face.normal[2], -face.normal[0]]))

          # Write out another triangle in case we have a quad: index, u, v
          faces = []
          if len(face.v) == 4:
              for v in [0, 2, 3]:
                  faces.append(VertexData(obj, face.v[v].index,
                                          [face.uv[v][0], 1.0-face.uv[v][1]],
                                          [face.normal[1], -face.normal[2], -face.normal[0]]))
              mesh_data[texture_filename].faces.append(FaceData(faces,
                                                                [face.normal[1], -face.normal[2], -face.normal[0]]))

      return mesh_data.values()

  def collect_frame_data(self):
      """
      Collect all data for the given object in a single frame, frame
      has to be selected outside of this function.
      Format:
      [[[vertex_pos_x, vertex_pos_y, vertex_pos_z], ...],
      [[attachment_pos_x, attachment_pos_y, attachment_pos_z,
      attachment_quat1, attachment_quat2, attachment_quat3, attachment_quat4], ...]]
       """
   
      meshs = []
      attachment_points = []

      obj = None

      # no triangles here (those are global), just vertexes (those are local)
      for mesh_data in self.mesh_data:
          vertex_positions = []
          for vertex in mesh_data.transtable:
              if obj != vertex[0]:
                  obj  = vertex[0]
                  data = Blender.NMesh.GetRawFromObject(obj.getName())

              index = vertex[1]

              m = obj.getMatrix()

              # location: action/frame/mesh/vertices
              v = data.verts[index]
              vertex_positions.append([+(m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]) * ZOOM,
                                       -(m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]) * ZOOM,
                                       -(m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]) * ZOOM])
          meshs.append(vertex_positions)

      # attachment points
      for obj in self.attachment_objects:
          m    = obj.matrixWorld
          loc  = (m[3][0] * ZOOM, m[3][1] * ZOOM, m[3][2] * ZOOM)
          quat = matrix2quaternion(m)      
          attachment_points.append(AttachmentPoint([loc[1], -loc[2], -loc[0]],
                                                   [quat[0], quat[2], quat[3], quat[1]]))

      return FrameData(meshs, attachment_points)

  def write(self, out):
      """This is a new version of write_file(), instead of using blender
      internals, it uses the collected data"""

      ### Write magic, version and counts for mesh, attachment_points and actions
      out.write(struct.pack("=4sHHHH", "W3DS",
                            FORMAT_VERSION, \
                            len(self.mesh_data),
                            len(self.attachment_objects),
                            len(self.action_data)))

      ### Mesh Header:
      for mesh in self.mesh_data:
          out.write(struct.pack("=64sHH",
                                mesh.texture_filename,
                                len(mesh.faces),
                                len(mesh.vertices)))
          ### Mesh Data:
          ## Vertex indices of triangles
          for face in mesh.faces:
              out.write(struct.pack("=HHH",
                                    mesh.translate(face.verts[0]),
                                    mesh.translate(face.verts[1]),
                                    mesh.translate(face.verts[2])))

          ## Normal
          for face in mesh.faces:
              out.write(struct.pack("=fff", face.normal[0], face.normal[1], face.normal[2]))

          ## UV Coordinates
          for vert in mesh.vertices:
            out.write(struct.pack("=ff", vert.uv[0], vert.uv[1]))

      ### Attachment points
      for obj in self.attachment_objects:
          out.write(struct.pack("=64s", obj.getName()[2:]))

      ## Action Header
      for index, action in enumerate(self.action_data):
          Window.DrawProgressBar(0.5 + float(index)/len(self.mesh_data) * 0.5,
                                 "Writing Action %s" % (action.name))

          out.write(struct.pack("=64sfHH",
                                action.name, 
                                action.config.speed * SPEED_MULTIPLIER,
                                len(action.config.markers),
                                len(action.frame_data)))
          
          ## Marker
          for (marker_name, marker_frame) in action.config.markers:
              out.write(struct.pack("=64sH",
                                    marker_name, 
                                    (marker_frame - action.config.first_frame) / action.config.samplerate))

          ## Action Data
          for frame in action.frame_data:
              ### Vertex positions
              for mesh in frame.vertex_locs:
                  for vert in mesh:
                      out.write(struct.pack("=fff", vert[0], vert[1], vert[2]))
              ### Attachment Points
              for at in frame.attachment_points:
                  out.write(struct.pack("=fffffff",
                                        at.loc[0],  at.loc[1],  at.loc[2],
                                        at.quat[0], at.quat[1], at.quat[2], at.quat[3]))
    ### DONE ###

  def print_stats(self):
      """Print some stats, vertex count, face count and such"""
      print "+===================================================================="
      print "| WindstilleSprite"
      print "+===================================================================="
      print "| Mesh Count:     ", len(self.mesh_data)
      print "| Mesh Objects:   ", self.mesh_objects
      print "| Actions:        ", self.actions
      print "| AttachmentObjs: ", self.attachment_objects
      print "| Meshs: "
      for mesh in self.mesh_data:
          print "|   Mesh:       ", mesh
          print "|     Texture:  ", mesh.texture_filename
          print "|     Faces:    ", len(mesh.faces)
          print "|     Vertices: ", len(mesh.vertices)
          print "|"
      print "| Actions: "
      for action in self.action_data:
          print "|   Name:        ", action.name
          print "|   Config:      ", action.config
          print "|   Frames:      ", len(action.frame_data)
          print "|   Vertices:    ", [len(x) for x in action.frame_data[0].vertex_locs]
          print "|   Attachments: ", len(action.frame_data[0].attachment_points)
          print "|"
      print "+===================================================================="
    
def export(filename):
    data = WindstilleSprite()

    file = open(filename, "wb")
    data.write(file)
    file.close()

    data.print_stats()

########################################################
def fs_callback(filename):
    print "=== Exporting: %s ===" % (filename)
    export(filename)
    Window.DrawProgressBar(1.0, "Finished writing %s" % (filename)) 
    print "=== Windstille Exporter done, wrote %s ===" % (filename)

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
    defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

# EOF #
