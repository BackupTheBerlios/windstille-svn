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
      
    actionconfig[action_name] = \
        (first_frame, last_frame, speed, samplerate, markers)
    print "Config: %s - %s" % (action_name, str(actionconfig[action_name]))
    token = lex.get_token()
    if token == lex.eof:
      break
    lex.push_token(token)

  return actionconfig

def parse_config(text):
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

  bones = []
  while True:
    token = lex.get_token()
    if token == lex.eof:
      break
    elif token == "ExportBone":
      bones.append(expect_string())
    elif token == "DefaultSpeed":
      global DEFAULT_SPEED
      DEFAULT_SPEED = expect_float()
    elif token == "SpeedMultiplier":
      global SPEED_MULTIPLIER
      SPEED_MULTIPLIER = expect_float()
    elif token == "Zoom":
      global ZOOM
      ZOOM = expect_float()
    elif token == "DefaultSamplerate":
      global DEFAULT_SAMPLERATE
      DEFAULT_SAMPLERATE = expect_int()
    else:
      raise Exception, "Unexpected token '%s'" % token
  return bones

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

def export(filename):
  scene = Blender.Scene.getCurrent()
  layers = scene.Layers

  # parse exportconfig
  bone_names = []
  try:
    bone_names = parse_config(get_text("exportconfig"))
  except Exception, message:
    raise Exception, "Parse Error in exportconfig:" + str(message)

  # parse actionconfig
  actionconfig = {}
  try:
    actionconfig = parse_actionconfig(get_text("actionconfig"))
  except Exception, message:
    raise Exception, "Parse Error in actionconfig:" + str(message)
 
  # compose list of meshs to export
  meshes = []
  for obj in Blender.Object.Get():
    data = obj.getData()
    if (type(data) is not Blender.Types.NMeshType) or not data.faces:
      continue
    if (obj.Layers & layers) == 0:
      print "Skipping \"%s\" because it is on different layer" % obj.getName()
      continue
    meshes.append(obj)

  # search for armature object
  armatures = Blender.Armature.Get()
  if len(armatures) != 1:
    raise Exception, "Need to have exactly 1 armature in the scene"

  armatureobj = 0
  for obj in Blender.Object.Get():
    print "Type: " + obj.getType()
    if obj.getType() == "Armature":
      print "found"
      armatureobj = obj
      armature = obj.getData()
      break
  if armatureobj == 0:
    raise Exception, "Couldn't find armature object..."

  # compose list of bones to export
  bones = []
  for name in bone_names:
    found = False
    for bone in armature.getBones():
      if bone.getName() == name:
        bones.append(bone)
        found = True
        break
    if not found:
      raise Exception, "No bone '%s' defined in armature" % name

  # compose list of actions to export
  actions = []
  for action in Blender.Armature.NLA.GetActions().iteritems():
    actions.append(action[1])

  # write file header
  file = open(filename, "wb")
  file.write(struct.pack("=4sHHHH", "W3DS", FORMAT_VERSION, \
        len(meshes), len(bones), len(actions)))
  objvertmaps = {}

  # Mesh Headers + Data
  for obj in meshes:
    data = obj.getData()

    print "Exporting Mesh %s" % obj.getName()

    texture = data.faces[0].image
    if not texture:
      print "Skipping Mesh %s: no texture" % obj.getName()
      continue
      
    texture_filename = texture.filename
    texture_filename = os.path.basename(texture_filename)
    vertexmap = []
    uvs = []
    def mapvertex(index, u, v):
      for mv in xrange(0, len(vertexmap)):
        if vertexmap[mv] == index and uvs[mv] == (u, v):
          return mv
      vertexmap.append(index)
      uvs.append( (u, v) )
      return len(vertexmap)-1

    # triangles
    vertexmap = []
    uvs = []
    bodydata = ""
    facecount = 0
    texturewarning = False
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

    file.write(struct.pack("=64sHH", texture_filename, facecount, len(vertexmap)))
    print "Exporting Mesh Tex: %s Facecount: %d Vertices: %d" \
            % (texture_filename, facecount, len(vertexmap))
    file.write(bodydata)

    objvertmaps[obj.getName()] = vertexmap

  # Bone Headers
  for bone in bones:
    file.write(struct.pack("=64s", bone.getName()))

  # Action Headers + actions
  actionnum = 0
  for action in actions:
    # enable action
    action.setActive(armatureobj)
 
    # find/autodetect config
    if actionconfig.has_key(action.getName()):
      (first_frame, last_frame, action_speed, samplerate, markers) = actionconfig[action.getName()]
    else:
      print "No config for action '%s' defined." % action.getName()
      first_frame = 1 
  
      # find out when the last ipo frame is set
      last_frame = 1
      for _ipo in action.getAllChannelIpos().iteritems():
        ipo = _ipo[1]
        for curve in ipo.getCurves():
          for p in curve.getPoints():
            time = p.getPoints() [0]
            if time > last_frame:
              last_frame = int(time)
      action_speed = DEFAULT_SPEED
      samplerate = DEFAULT_SAMPLERATE
      markers = []

    # calculate number of frames and stuff for the header
    numframes = last_frame - first_frame + 1
    resultframes = 0
    for i in range(first_frame, last_frame+1, samplerate):
      resultframes += 1
    progress = 1.0/float(len(actions)) * actionnum

    print "Exporting Action %s (%d frames)" \
            % (action.getName(), resultframes)
    Window.DrawProgressBar(progress, "Exporting Action %s (%d frames)" \
            % (action.getName(), resultframes))
    actionnum += 1
    file.write(struct.pack("=64sfHH", action.getName(), \
          action_speed * SPEED_MULTIPLIER, len(markers), resultframes))

    def blenderframe_to_wspriteframe(frame):
      return (frame - first_frame) / samplerate

    # write markers
    for marker in markers:
      file.write(struct.pack("=64sH", marker[0], \
            blenderframe_to_wspriteframe(marker[1])))
  
    # output for all frames for all meshs all vertex positions
    frs = 0
    for frame in range(first_frame, last_frame+1, samplerate):
      frs += 1
      Blender.Set("curframe", int(frame))
      #for obj in Blender.Object.Get():
      #  data = obj.getData()
      #  if (type(data) is not Blender.Types.NMeshType) or not data.faces:                 continue
      #  if (obj.Layers & layers) == 0:
      #    continue
      for obj in meshes:
        data = Blender.NMesh.GetRawFromObject(obj.getName())
        vertexmap = objvertmaps[obj.getName()]
        m = obj.getMatrix()
        # action/frame/mesh/vertices
        for nv in vertexmap:
          v = data.verts[nv]
          t = [0, 0, 0]
          t[0] = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]
          t[1] = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]        
          t[2] = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]
          t[0] *= ZOOM
          t[1] *= ZOOM
          t[2] *= ZOOM
          file.write(struct.pack("=fff", t[1], -t[2], t[0]))

      # bone positions
      for bone in bones:
        bonemat = bone.getRestMatrix('worldspace')
        loc = (m[3][0], m[3][1], m[3][2])
        file.write(struct.pack("=fff", loc[0], loc[1], loc[2]))
        quat = matrix2quaternion(bonemat)
        file.write(struct.pack("=ffff", quat[0], quat[1], quat[2], quat[3]))
      
def fs_callback(filename):
  export(filename)

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
    defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

