#!BPY

"""
Name: 'Windstille Exporter'
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

# SAMPLEFRAMES, only export every nth frame of the animation
DEFAULT_SAMPLERATE = 5
# ZOOM, is multiplied with all vertex coordinates
ZOOM = 32.0
DEFAULT_SPEED = 1.0
SPEED_MULTIPLIER = 9.8
# DO NOT change this
FORMAT_VERSION = 2

def matrix2quaternion(m):
  tr = 1.0 + m[0][0] + m[1][1] + m[2][2]
  if tr > .00001:
    s = math.sqrt(tr)
    w = s / 2.0
    s = 0.5 / s
    x = (m[1][2] - m[2][1]) * s
    y = (m[2][0] - m[0][2]) * s
    z = (m[0][1] - m[1][0]) * s
  elif m[0][0] > m[1][1] and m[0][0] > m[2][2]:
    s = math.sqrt(1.0 + m[0][0] - m[1][1] - m[2][2])
    x = s / 2.0
    s = 0.5 / s
    y = (m[0][1] + m[1][0]) * s
    z = (m[2][0] + m[0][2]) * s
    w = (m[1][2] - m[2][1]) * s
  elif m[1][1] > m[2][2]:
    s = math.sqrt(1.0 + m[1][1] - m[0][0] - m[2][2])
    y = s / 2.0
    s = 0.5 / s
    x = (m[0][1] + m[1][0]) * s
    z = (m[1][2] + m[2][1]) * s
    w = (m[2][0] - m[0][2]) * s
  else:
    s = math.sqrt(1.0 + m[2][2] - m[0][0] - m[1][1])
    z = s / 2.0
    s = 0.5 / s
    x = (m[2][0] + m[0][2]) * s
    y = (m[1][2] + m[2][1]) * s
    w = (m[0][1] - m[1][0]) * s

  return quaternion_normalize([w, x, y, z])

def quaternion_normalize(q):
  l = math.sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3])
  return q[0] / l, q[1] / l, q[2] / l, q[3] / l

def quaternion_to_axisangle(q):
  cos_a = q[0]
  angle = math.acos(cos_a) * 2.0
  sin_a = math.sqrt(1.0 - cos_a * cos_a)
  if(sin_a < .0005 or sin_a > .0005): sin_a = 1
  return angle, q[1]/sin_a, q[2]/sin_a, q[3]/sin_a

# config entry (first_frame, last_frame, speed, samplerate, markers[])
#  a marker is (name, frame)
def parse_actionconfig(text):
  lex = shlex.shlex(text)
  lex.wordchars += "."
  def expect_string():
    res = lex.get_token()
    if res == lex.eof:
      raise Exception, "Expected string, got EOF"
    return res

  def expect_int():
    res = lex.get_token()
    if res == lex.eof:
      raise Exception, "Expected in, got EOF"
    return int(res)

  def expect_float():
    res = lex.get_token()
    if res == lex.eof:
      raise Exception, "Expected float, got EOF"
    return float(res)

  def expect_dash():
    res = lex.get_token()
    if res == lex.eof:
      raise Exception, "Expected '-', got EOF"
    elif res != "-":
      raise Exception, "Expected '-', got '%s'" % res

  actionconfig = {}
  while True:
    token = lex.get_token()
    if token == lex.eof:
      break
    lex.push_token(token)
    action_name = expect_string()
    first_frame = expect_int()
    expect_dash()
    last_frame = expect_int()

    token = lex.get_token()
    if token == "speed":
      speed = expect_float()
    else:
      lex.push_token(token)
      speed = DEFAULT_SPEED

    token = lex.get_token()
    if token == "samplerate":
      samplerate = expect_int()
    else:
      lex.push_token(token)
      samplerate = DEFAULT_SAMPLERATE

    token = lex.get_token()
    markers = []
    while token == "marker":
      marker_name = expect_string()
      marker_frame = expect_int()
      markers.append( (marker_name, marker_frame) )
      token = lex.get_token()
    lex.push_token(token)

    actionconfig[action_name] = ActionConfig(first_frame, last_frame, speed, samplerate, markers)
    print "Config: %-20s - %s" % (action_name, str(actionconfig[action_name]))

  return actionconfig

##########################################################
def get_text(textname):
  """Little shortcut function to return the content of
  Blender.Text.get(textname) as a single string and do a little error
  handling in addition""" 
  try:
    textobj = Blender.Text.Get(textname)
  except:                                                         
    print "WARNING: Text '%s' not found" % textname
    return ""

  lines = textobj.asLines()
  text = ""
  for line in lines:
    text += line + "\n"
  return text

class MeshData:
  def __init__(self, texture_filename, faces = []):
    # Filename of the used texture
    self.texture_filename = texture_filename

    # [[MeshVertexData, MeshVertexData, MeshVertexData], ...]
    self.faces            = faces

    # Table to translate (object, index) to new_index
    # Format: [[object, index, new_index], ...]
    self.transtable       = []

  def transtable_has_vertex(self, new_object, new_index):
    for (object, index, new_index) in self.transtable:
      if object == new_object and index == new_object:
        return True
      else:
        return False

  def finalize(self):
    """Reorders vertex indexes"""

    for face in self.faces:
      for vert in face:
        if not self.transtable_has_vertex(vert.object, vert.index):
          self.transtable += [[vert.object, vert.index, len(self.transtable)]]

    # print "FINALIZE"
    # print self.transtable
    # print "FINALIZE END"

  def vertices(self):
    return self.transtable

  def translate(self, arg_object, arg_index):
    """Translate a vertex given as object, index pair to its
    corresponding index, as used in the wsprite file"""
   
    for (object, index, new_index) in self.transtable:
      if arg_object == object and arg_index == index:
        return new_index
    raise "MeshData: Couldn't translate vertex: %s, %s" % (arg_object, arg_index)

class MeshVertexData:
  def __init__(self, object, index, uv, normal):
    self.object = object
    self.index  = index
    self.uv     = uv
    # FIXME: Shouldn't the normal be a per per vertex data?
    self.normal = normal

class AttachmentPointData:
  """Data for an attachment point, its location and its rotation"""
  def __init__(self, loc, quat):
    self.loc  = loc
    self.quat = quat

class FrameData:
  """ Data used for a single frame in an action """
  def __init__(self, vertex_locs, attachment_points):
    # Format: [[[x,y,z], ...], [[x,y,z],  ...], ...] (one list for each Mesh)
    self.vertex_locs        = vertex_locs
    
    # [AttachmentPointData, ...]
    self.attachment_points = attachment_points


class ActionConfig:
  """ActionConfig handles the properties of a single
  action, ie. when it starts, when it stops, its speed, how many
  spamles should be taken, etc."""

  def __init__(self, first_frame, last_frame, speed, samplerate, markers):
    self.first_frame = first_frame
    self.last_frame  = last_frame
    self.speed       = speed
    self.samplerate  = samplerate
    self.markers     = markers
    self.numframes   = last_frame - first_frame + 1

  def __str__(self):
    return "Frames: %3i - %3i, speed: %3.2f, Samplerate: %3d" % (self.first_frame, self.last_frame,
                                                           self.speed, self.samplerate)

class ActionData:
  def __init__(self, name, config):
    # name as string
    self.name       = name

    # ActionConfig
    self.config     = config

    # FrameData (filled out later in the WindstilleExporter)
    self.frame_data = None

##########################################################
class WindstilleExporter:
  """ WindstilleExporter bundles all kind of functions... """

  ########################################################
  def __init__(self):
    # List of objects that should get exported (everything on seperate layer is ignored)
    self.mesh_objects    = []

    # List of mesh_data
    self.mesh_data   = []
    
    self.attachment_objects = []

    # Name of the actions as string
    self.actions = []
    
    self.objvertmaps = {}
    self.armatureobj = None

    # { actionname : ActionConfig, ... }
    self.actionconfigs = {}

  ########################################################
  def export(self, filename):
    """ parse actionconfig """
    try:
      self.actionconfigs = parse_actionconfig(get_text("actionconfig"))
    except Exception, message:
      raise Exception, "Parse Error in actionconfig:" + str(message)

    self.export_lowlevel()

    file = open(filename, "wb")
    self.write_file(file)
    self.collect_scene_data()
    file.close()

  ### begin: save_frame()
  def save_frame(self, out):
    """Write out vertex position and position of attachment points in a frame"""
    for obj in self.mesh_objects:
      data = Blender.NMesh.GetRawFromObject(obj.getName())
      m = obj.getMatrix()
      # location: action/frame/mesh/vertices
      for nv in self.objvertmaps[obj.getName()]:
        v = data.verts[nv]
        out.write(struct.pack("=fff",
                              +(m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]) * ZOOM,
                              -(m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]) * ZOOM,
                              -(m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]) * ZOOM))

    # attachment points
    for obj in self.attachment_objects:
      m = obj.matrixWorld
      loc = (m[3][0] * ZOOM, m[3][1] * ZOOM, m[3][2] * ZOOM)
      out.write(struct.pack("=fff", loc[1], -loc[2], -loc[0]))
      quat = matrix2quaternion(m)
      out.write(struct.pack("=ffff", quat[0], quat[2], quat[3], quat[1]))
    ### end: save_frame()

  ########################################################    
  def write_file(self, out):
    """ write file header """
    out.write(struct.pack("=4sHHHH", "W3DS", FORMAT_VERSION, \
          len(self.mesh_objects), len(self.attachment_objects), len(self.actions)))

    # Mesh Headers + Data
    for obj in self.mesh_objects:
      self.export_mesh_header(out, obj)

    # Attachment Point Headers
    for obj in self.attachment_objects:
      out.write(struct.pack("=64s", obj.getName()[2:]))

    # Action Headers + actions
    actionnum = 0

    # special case, no armature+animations
    if not self.armatureobj:
      out.write(struct.pack("=64sfHH", "Default", \
                            DEFAULT_SPEED * SPEED_MULTIPLIER, 0, 1))
      self.save_frame(out)
    else:
      for action in self.actions:
        # enable action
        action.setActive(self.armatureobj)

        # find/autodetect config
        if self.actionconfigs.has_key(action.getName()):
          actioncfg = self.actionconfigs[action.getName()]
        else:
          print "Error: No config for action '%s' defined." % action.getName()
          actioncfg = ActionConfig(1, 1, DEFAULT_SPEED, DEFAULT_SAMPLERATE, [])

        # calculate number of frames and stuff for the header
        resultframes = 0
        for i in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
          resultframes += 1
        progress = 1.0/float(len(self.actions)) * actionnum

        print "Exporting Action %s (%d frames)" \
                % (action.getName(), resultframes)
        Window.DrawProgressBar(progress, "Exporting Action %s (%d frames)" \
                % (action.getName(), resultframes))
        actionnum += 1
        out.write(struct.pack("=64sfHH", action.getName(), \
              actioncfg.speed * SPEED_MULTIPLIER, len(actioncfg.markers), resultframes))

        def blenderframe_to_wspriteframe(frame):
          return (frame - actioncfg.first_frame) / actioncfg.samplerate

        # write markers
        for marker in actioncfg.markers:
          out.write(struct.pack("=64sH", marker[0], \
                blenderframe_to_wspriteframe(marker[1])))

        # output for all frames for all meshs all vertex positions
        frs = 0
        for frame in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
          frs += 1
          Blender.Set("curframe", int(frame))
          self.save_frame(out)
  ### end: write_file()

  def export_mesh_header(self, out, obj):
    print "Exporting Mesh %s" % obj.getName()
    data = obj.getData()

    print "Faces: %s" % (len(data.faces))

    texture = data.faces[0].image
    texture_filename = texture.filename
    texture_filename = os.path.basename(texture_filename)

    # triangles
    vertexmap = []
    uvs = []
    bodydata = ""
    facecount = 0
    texturewarning = False

    ##############################
    def mapvertex(index, u, v):
      """Inline helper function"""
      for mv in xrange(0, len(vertexmap)):
        if vertexmap[mv] == index and uvs[mv] == (u, v):
          return mv
      vertexmap.append(index)
      uvs.append( (u, v) )
      return len(vertexmap)-1
    ##############################

    for face in data.faces:
      if face.image != texture and not texturewarning:
        print "WARNING: Mesh '%s' has more than 1 material" % obj.getName()
        texturewarning = True

      # Write out triangle: ((index, u, v), (index, u, v), (index, u, v))
      for v in [0, 1, 2]:
        bodydata += struct.pack("=H", \
              mapvertex(face.v[v].index, face.uv[v][0], face.uv[v][1]))
      facecount += 1

      # Write out another triangle in case we have a quad: index, u, v
      if len(face.v) == 4:
        facecount += 1
        for v in [0, 2, 3]:
          bodydata += struct.pack("=H", \
                mapvertex(face.v[v].index, face.uv[v][0], face.uv[v][1]))

    # FIXME: saving normals doesn't make sense per-mesh, should be per
    # frame, unless I am overlooking something
    
    # normals
    for face in data.faces:
      bodydata += struct.pack("=fff", face.normal[1], -face.normal[2], -face.normal[0])
      if len(face.v) == 4:
        bodydata += struct.pack("=fff", face.normal[1], -face.normal[2], -face.normal[0])

    # uv coords per vertex
    for uv in uvs:
      bodydata += struct.pack("=ff", uv[0], 1.0-uv[1])

    out.write(struct.pack("=64sHH", texture_filename, facecount, len(vertexmap)))
    print "Exporting Mesh Tex: %s Facecount: %d Vertices: %d" \
            % (texture_filename, facecount, len(vertexmap))
    out.write(bodydata)

    self.objvertmaps[obj.getName()] = vertexmap
  ### end: export_mesh()

  def export_lowlevel(self):
    """Convert Blender data structures into something that is used by
    this export script"""

    scene = Blender.Scene.getCurrent()
    layers = scene.Layers

    # compose list of meshs to export
    for obj in Blender.Object.Get():
      data = obj.getData()
      if (type(data) is not Blender.Types.NMeshType) or not data.faces:
        continue
      if (obj.Layers & layers) == 0:
        print "Skipping \"%s\" because it is on different layer" % obj.getName()
        continue
      texture = data.faces[0].image
      if not texture:
        print "Skipping Mesh %s: no texture" % obj.getName()
        continue
      self.mesh_objects.append(obj)

    # search for armature object
    armatures = Blender.Armature.Get()
    if len(armatures) > 1:
      raise Exception, "Need to have at most 1 armature in the scene"

    for obj in Blender.Object.Get():
      print "Type: " + obj.getType()
      if obj.getType() == "Armature":
        print "found"
        self.armatureobj = obj
        armature = obj.getData()
        break

    # compose list of objects for attachment points
    for obj in Blender.Object.Get():
      if obj.getType() == 'Empty' and obj.getName().startswith("A:"):
        self.attachment_objects.append(obj)

    # compose list of actions to export
    if not self.armatureobj:
      self.actions.append("Default")
    else:
      for action in Blender.Armature.NLA.GetActions().iteritems():
        self.actions.append(action[1])
  ## end: exporter_lowlevel()

  def collect_scene_data(self):
    self.collect_object_data()

    for action in self.actions:
      action.setActive(self.armatureobj)
    
      # find/autodetect config
      if self.actionconfigs.has_key(action.getName()):
        actioncfg = self.actionconfigs[action.getName()]
      else:
        print "Error: No config for action '%s' defined." % action.getName()
        actioncfg = ActionConfig(1, 1, DEFAULT_SPEED, DEFAULT_SAMPLERATE, [])

    for frame in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
      Blender.Set("curframe", int(frame))
      print self.collect_frame_data()

  def collect_object_data(self):
    for obj in self.mesh_objects:
      self.mesh_data += self.collect_mesh_data(obj)

    # insert code to merge meshes here

    # Create the transtable
    for mesh in self.mesh_data:
      mesh.finalize()

    print "############# Begin: MeshDATA ##############"
    for val in self.collect_mesh_data(obj):
      print val
    print "############# End: MeshDATA ##############"

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
        texture_filename = "404.png"
        
      if not mesh_data.has_key(texture_filename):
        mesh_data[texture_filename] = MeshData(texture_filename)

      faces = []
      for v in [0, 1, 2]:
        faces += [MeshVertexData(obj, face.v[v].index,
                                [face.uv[v][0], 1.0-face.uv[v][1]],
                                [face.normal[1], -face.normal[2], -face.normal[0]])]
      mesh_data[texture_filename].faces += [faces]

      # Write out another triangle in case we have a quad: index, u, v
      if len(face.v) == 4:
        for v in [0, 2, 3]:
          faces += [MeshVertexData(obj, face.v[v].index,
                                  [face.uv[v][0], 1.0-face.uv[v][1]],
                                  [face.normal[1], -face.normal[2], -face.normal[0]])]
        mesh_data[texture_filename].faces += [faces]
      
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
      for vertex in mesh_data.vertices():
        if obj != vertex[0]:
          obj  = vertex[0]
          data = Blender.NMesh.GetRawFromObject(obj.getName())
          
        index = vertex[1]
        
        m = obj.getMatrix()

        # location: action/frame/mesh/vertices
        v = data.verts[index]
        vertex_positions += [[+(m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]) * ZOOM,
                              -(m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]) * ZOOM,
                              -(m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]) * ZOOM]]
      meshs += [vertex_positions]

    # attachment points
    for obj in self.attachment_objects:
      m    = obj.matrixWorld
      loc  = (m[3][0] * ZOOM, m[3][1] * ZOOM, m[3][2] * ZOOM)
      quat = matrix2quaternion(m)      
      attachment_points += [AttachmentPoint([loc[1], -loc[2], -loc[0]],
                                            [quat[0], quat[2], quat[3], quat[1]])]

    return FrameData(meshs, attachment_points)

### end: WindstilleExporter

########################################################
def fs_callback(filename):
  print "=== Exporting: %s ===" % (filename)
  exporter = WindstilleExporter()
  exporter.export(filename)
  print "=== Windstille Exporter done, wrote %s ===" % (filename)

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
  defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

# EOF #
