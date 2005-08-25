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
##  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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
  s = math.sqrt(abs(m[0][0] + m[1][1] + m[2][2] + m[3][3]))
  if s == 0.0:
    x = abs(m[2][1] - m[1][2])
    y = abs(m[0][2] - m[2][0])
    z = abs(m[1][0] - m[0][1])
    if   (x >= y) and (x >= z): return 1.0, 0.0, 0.0, 0.0
    elif (y >= x) and (y >= z): return 0.0, 1.0, 0.0, 0.0
    else:                       return 0.0, 0.0, 1.0, 0.0
  return quaternion_normalize([
    -(m[2][1] - m[1][2]) / (2.0 * s),     -(m[0][2] - m[2][0]) / (2.0 * s),
    -(m[1][0] - m[0][1]) / (2.0 * s),
    0.5 * s,
    ])

def quaternion_normalize(q):
  l = math.sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3])
  return q[0] / l, q[1] / l, q[2] / l, q[3] / l

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

class ActionConfig:
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

# return contents of a text in the blender scene
def get_text(textname):
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

class WindstilleExporter:
  def __init__(self):
    self.meshes    = []
    self.attachement_objects = []
    self.actions = []
    self.objvertmaps = {}
    self.file = None
    self.armatureobj = None

  def export(self, filename):
    # parse actionconfig
    try:
      self.actionconfigs = parse_actionconfig(get_text("actionconfig"))
    except Exception, message:
      raise Exception, "Parse Error in actionconfig:" + str(message)
    
    self.export_lowlevel()
    
    self.file = open(filename, "wb")
    self.write_file()
    self.file.close()

  def write_file(self):
    # write file header
    self.file.write(struct.pack("=4sHHHH", "W3DS", FORMAT_VERSION, \
          len(self.meshes), len(self.attachement_objects), len(self.actions)))

    # Mesh Headers + Data
    for obj in self.meshes:
      self.export_mesh_header(obj)
      
    # Attachement Point Headers
    for obj in self.attachement_objects:
      self.file.write(struct.pack("=64s", obj.getName()[2:]))

    # Action Headers + actions
    def save_frame():
      for obj in self.meshes:
        data = Blender.NMesh.GetRawFromObject(obj.getName())
        m = obj.getMatrix()
        # action/frame/mesh/vertices
        for nv in self.objvertmaps[obj.getName()]:
          v = data.verts[nv]
          t = [0, 0, 0]
          t[0] = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]
          t[1] = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]        
          t[2] = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]
          t[0] *= ZOOM
          t[1] *= ZOOM
          t[2] *= ZOOM
          self.file.write(struct.pack("=fff", t[1], -t[2], t[0]))

      # attachement points
      for obj in self.attachement_objects:
        m = obj.matrixWorld
        loc = (m[3][0] * ZOOM, m[3][1] * ZOOM, m[3][2] * ZOOM)
        self.file.write(struct.pack("=fff", loc[0], loc[1], loc[2]))
        quat = matrix2quaternion(m)
        self.file.write(struct.pack("=ffff", quat[0], quat[1], quat[2], quat[3]))

    actionnum = 0

    for action in self.actions:
      # special case, no armature+animations
      if not self.armatureobj:
        self.file.write(struct.pack("=64sfHH", "Default", \
                   DEFAULT_SPEED * SPEED_MULTIPLIER, 0, 1))
        save_frame()
        break

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
      self.file.write(struct.pack("=64sfHH", action.getName(), \
            actioncfg.speed * SPEED_MULTIPLIER, len(actioncfg.markers), resultframes))

      def blenderframe_to_wspriteframe(frame):
        return (frame - actioncfg.first_frame) / actioncfg.samplerate

      # write markers
      for marker in actioncfg.markers:
        self.file.write(struct.pack("=64sH", marker[0], \
              blenderframe_to_wspriteframe(marker[1])))

      # output for all frames for all meshs all vertex positions
      frs = 0
      for frame in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
        frs += 1
        Blender.Set("curframe", int(frame))
        save_frame()
    
  def export_mesh_header(self, obj):
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

    def mapvertex(index, u, v):
      for mv in xrange(0, len(vertexmap)):
        if vertexmap[mv] == index and uvs[mv] == (u, v):
          return mv
      vertexmap.append(index)
      uvs.append( (u, v) )
      return len(vertexmap)-1
    
    for face in data.faces:
      if face.image != texture and not texturewarning:
        print "WARNING: Mesh '%s' has more than 1 material" % obj.getName()
        texturewarning = True

      for v in [0, 1, 2]:
        bodydata += struct.pack("=H", \
              mapvertex(face.v[v].index, face.uv[v][0], face.uv[v][1]))
      if len(face.v) == 4:
        facecount += 2
        for v in [0, 2, 3]:
          bodydata += struct.pack("=H", \
                mapvertex(face.v[v].index, face.uv[v][0], face.uv[v][1]))
      else:
        facecount += 1

    # normals
    for face in data.faces:
      bodydata += struct.pack("=fff", face.normal[1], -face.normal[2], face.normal[0])
      if len(face.v) == 4:
        bodydata += struct.pack("=fff", face.normal[1], -face.normal[2], face.normal[0])

    # uv coords per vertex
    for uv in uvs:
      bodydata += struct.pack("=ff", uv[0], 1.0-uv[1])

    self.file.write(struct.pack("=64sHH", texture_filename, facecount, len(vertexmap)))
    print "Exporting Mesh Tex: %s Facecount: %d Vertices: %d" \
            % (texture_filename, facecount, len(vertexmap))
    self.file.write(bodydata)

    self.objvertmaps[obj.getName()] = vertexmap

  def export_lowlevel(self):
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
      self.meshes.append(obj)

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

    # compose list of objects for attachement points
    for obj in Blender.Object.Get():
      if obj.getType() == 'Empty' and obj.getName().startswith("A:"):
        self.attachement_objects.append(obj)

    # compose list of actions to export
    if not self.armatureobj:
      self.actions.append("Default")
    else:
      for action in Blender.Armature.NLA.GetActions().iteritems():
        self.actions.append(action[1])
      
def fs_callback(filename):
  print "=== Exporting: %s ===" % (filename)
  exporter = WindstilleExporter()
  exporter.export(filename)
  print "=== Done ==="

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
    defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

# EOF #
