#!BPY

"""
Name: 'Windstille Exporter'
Blender: 233
Group: 'Export'
Tip: 'Export meshes/actions to windstille format'
"""

##  $Id$
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
# Put it in your $HOME/.blender/scripts directory

import struct
import Blender
from Blender import NMesh
from Blender import Window

# SAMPLEFRAMES, only export every nth frame of the animation
SAMPLEFRAMES = 5
# ZOOM, is multiplied with all vertex coordinates
ZOOM = 32.0

def export(filename):
  scene = Blender.Scene.getCurrent()
  layers = scene.Layers

  # file header
  meshes = []
  for obj in Blender.Object.Get():
    data = obj.getData()
    if (type(data) is not Blender.Types.NMeshType) or not data.faces:
      continue
    if (obj.Layers & layers) == 0:
      print "Skipping \"%s\" because it is on different layer" % obj.getName()
      continue
    meshes.append(obj)

  armatures = Blender.Armature.Get()
  if len(armatures) != 1:
    print "Need to have exactly 1 armature in the scene"
    return

  armatureobj = 0
  for obj in Blender.Object.Get():
    print "Type: " + obj.getType()
    if obj.getType() == "Armature":
      print "found"
      armatureobj = obj
      break
  if armatureobj == 0:
    print "COuldn't find armature object..."
    return

  actions = []
  for action in Blender.Armature.NLA.GetActions().iteritems():
    actions.append(action[1])

  file = open(filename, "wb")
  file.write(struct.pack("=4sHH", "W3DS", len(meshes), len(actions)))
  objvertmaps = {}

  # Mesh Headers + Data
  for obj in meshes:
    data = obj.getData()

    print "Exporting Mesh %s" % obj.getName()

    texture = data.faces[0].image
    if not texture:
      print "Mesh %s has no texture" % obj.getName
      continue
      
    texture_filename = texture.filename
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
    for face in data.faces:
      if face.image != texture:
        print "Meshs with different materials not supported"
    
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

    for face in data.faces:
      bodydata += struct.pack("=fff", face.normal[0], face.normal[1], face.normal[2])
      if len(face.v) == 4:
        bodydata += struct.pack("=fff", face.normal[0], face.normal[1], face.normal[2])

    for uv in uvs:
      bodydata += struct.pack("=ff", uv[0], 1.0-uv[1])

    file.write(struct.pack("=64sHH", texture_filename, facecount, len(vertexmap)))
    print "Exporting Mesh Tex: %s Facecount: %d Vertices: %d" \
            % (texture_filename, facecount, len(vertexmap))
    file.write(bodydata)

    objvertmaps[obj.getName()] = vertexmap

  actionnum = 0
  for action in actions:
    # TODO...
    #print armatures[0]
    action.setActive(armatureobj)
  
    # find out when the last ipo frame is set
    numframes = 1
    for _ipo in action.getAllChannelIpos().iteritems():
      ipo = _ipo[1]
      for curve in ipo.getCurves():
        for p in curve.getPoints():
          time = p.getPoints() [0]
          if time > numframes:
            numframes = time

    resultframes = (SAMPLEFRAMES - 1 + numframes) / SAMPLEFRAMES
    progress = 1.0/len(actions).__float__() * actionnum
    Window.DrawProgressBar(progress, "Exporting Action %s (%d frames)" \
            % (action.getName(), resultframes))
    actionnum += 1
    
    file.write(struct.pack("=64sH", action.getName(), resultframes))
   
    frs = 0
    for frame in xrange(1, numframes+1, SAMPLEFRAMES):
      frs += 1
      Blender.Set("curframe", frame.__int__())
      for obj in Blender.Object.Get():
        data = obj.getData()
        if (type(data) is not Blender.Types.NMeshType) or not data.faces:                 continue
        if (obj.Layers & layers) == 0:
          continue
        data = Blender.NMesh.GetRawFromObject(obj.getName())
        vertexmap = objvertmaps[obj.getName()]
        m = obj.getMatrix()
        for nv in vertexmap:
          v = data.verts[nv]
          t = [0, 0, 0]
          t[0] = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]
          t[1] = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]        
          t[2] = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]
          t[0] *= ZOOM
          t[1] *= ZOOM
          t[2] *= ZOOM
          file.write(struct.pack("=fff", t[0], t[1], t[2]))
    print frs

def fs_callback(filename):
  export(filename)

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
    defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

# EOF #
