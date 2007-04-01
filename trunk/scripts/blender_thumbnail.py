## Usage:
## blender longshot.blend -P ~/projects/windstille/trunk/scripts/blender_thumbnail.py
## ! Argument order matters !

## Bugs: The .blend file must not be in "Maximized Window Mode" (Ctrl
## UpArrow) or it will segfault

## ToDo:
## - do multiple renderings in one go without restarting blender
## - optimize the bounding rect so that the four-dir view contains
##   less empty space
## - set some better rendering parameter
## - add a 3/4 view

import Blender
from Blender.Mathutils import *
from Blender import Camera
from Blender.Scene import Render
from Blender import Lamp
import time
import math
import os

# print "\033c--- Start --- %s" % time.time()

class BBox:
    def __init__(self, bbox = None):
        self.x1 = self.x2 = None
        self.y1 = self.y2 = None
        self.z1 = self.z2 = None    
        
        if bbox:                
            for v in bbox: 
                self.x1 = min(self.x1, v.x) or v.x
                self.x2 = max(self.x2, v.x) or v.x

                self.y1 = min(self.y1, v.y) or v.y
                self.y2 = max(self.y2, v.y) or v.y

                self.z1 = min(self.z1, v.z) or v.z
                self.z2 = max(self.z2, v.z) or v.z
        else:
            self.x1 = None
            self.x2 = None
            self.y1 = None
            self.y2 = None
            self.z1 = None
            self.z2 = None

    def width_x(self):
        return self.x2 - self.x1

    def width_y(self):
        return self.y2 - self.y1

    def width_z(self):
        return self.z2 - self.z1

    def max(self):
        """Returns the longest side of the bbox"""
        return max(self.x2 - self.x1,
                   self.y2 - self.y1,
                   self.z2 - self.z1)

    def normalize(self):
        self.x1 = min(self.x1, self.x2)
        self.x2 = max(self.x2, self.x1)

        self.y1 = min(self.y1, self.y2)
        self.y2 = max(self.y2, self.y1)

        self.z1 = min(self.z1, self.z2)
        self.z2 = max(self.z2, self.z1)

    def valid(self):
        return self.x1 and self.x2 and \
               self.y1 and self.y2 and \
               self.z1 and self.z2
    
    def join(self, bbox):
        self.x1 = min(self.x1, bbox.x1) or bbox.x1
        self.x2 = max(self.x2, bbox.x2) or bbox.x2

        self.y1 = min(self.y1, bbox.y1) or bbox.y1
        self.y2 = max(self.y2, bbox.y2) or bbox.y2

        self.z1 = min(self.z1, bbox.z1) or bbox.z1
        self.z2 = max(self.z2, bbox.z2) or bbox.z2

    def __str__(self):
        return "[bbox: x:%.2f, %.2f, y:%.2f, %.2f, z:%.2f, %.2f]" % \
               (self.x1, self.x2,
                self.y1, self.y2,
                self.z1, self.z2)

def render_thumbnail(axis, resolution, outfile):
    total = BBox()

    scn = Blender.Scene.GetCurrent()

    # print scn.objects
    for obj in scn.objects:
        if obj.getType() != "Empty" and \
           obj.getType() != "Light" and \
           (1 in obj.layers) and obj.boundingBox:
            # print obj.getType()
            # print obj.boundingBox
            total.join(BBox(obj.boundingBox))
    
    if not total.valid:
        # Scene is empty
        pass
    else:
        # print total
        # Position of camera and support objects

        if axis == "x": # ok
            (x, y, z) = (total.x1 - 5,
                         (total.y2 + total.y1)/2,
                         (total.z2 + total.z1)/2)
            euler = Euler(-math.pi/2, math.pi, math.pi/2)
        elif axis == "-x": # ok
            (x, y, z) = (total.x2 + 5,
                         (total.y2 + total.y1)/2,
                         (total.z2 + total.z1)/2)
            euler = Euler(math.pi/2, 0, math.pi/2)
        elif axis == "y":
            (x, y, z) = ((total.x2 + total.x1)/2,
                         total.y1 - 5,
                         (total.z2 + total.z1)/2)
            euler = Euler(math.pi/2, 0, 0)     
        elif axis == "-y":
            (x, y, z) = ((total.x2 + total.x1)/2,
                         total.y2 + 5,
                         (total.z2 + total.z1)/2)
            euler = Euler(-math.pi/2, math.pi, 0)    
        elif axis == "z": # ok
            (x, y, z) = ((total.x2 + total.x1)/2,
                         (total.y2 + total.y1)/2,
                         total.z1 - 5)
            euler = Euler(0, math.pi, 0)
        elif axis == "-z": # ok
            (x, y, z) = ((total.x2 + total.x1)/2,
                         (total.y2 + total.y1)/2,
                         total.z2 + 5)
            euler = Euler(0, 0, 0)
        else:
            raise "Unknown axis: '%s'" % (axis,)
        
        ### Add camera
        cam = Camera.New('ortho')
        cam.scale = total.max() # (total.x2 - total.x1, total.y2 - total.y1)
        # cam.scale += cam.scale * 0.1
        cam_obj = scn.objects.new(cam)
        
        scn.setCurrentCamera(cam_obj)

        # matrix = TranslationMatrix(Vector(x,y,z))
        # RotationMatrix(angle, 3, r, vec)
        # cam_obj.setMatrix(matrix)
        
        cam_obj.setLocation(x, y, z)
        cam_obj.setEuler(euler)

        ### Add lamp
        light = Lamp.New('Lamp')            # create new 'Spot' lamp data
        light.energy = 1.5
        # light.setMode('Square', 'Shadow')   # set these two lamp mode flags
        light_obj = scn.objects.new(light)
        light_obj.setLocation(x, y, z)

        render = scn.getRenderingContext()

        render.renderwinSize = 100
        
        render.aspectX = 100
        render.aspectY = 100

#         if axis == "x" or axis == "-x": # front
#             lmax = total.max()  #max(total.width_y(), total.width_z())
#             render.sizeX = int(resolution * total.width_y() / lmax)
#             render.sizeY = int(resolution * total.width_z() / lmax)
#         elif axis == "y" or axis == "-y": # side
#             lmax = total.max() # max(total.width_x(), total.width_z())
#             render.sizeX = int(resolution * total.width_x() / lmax)
#             render.sizeY = int(resolution * total.width_z() / lmax)
#         elif axis == "z" or axis == "-z": # top
#             lmax = total.max() # max(total.width_y(), total.width_x())
#             render.sizeX = int(resolution * total.width_x() / lmax)
#             render.sizeY = int(resolution * total.width_y() / lmax)
#         else:
#             raise "Unknown axis: '%s'" % (axis,)

        render.sizeX = resolution
        render.sizeY = resolution

        render.imageType = Render.PNG
        # render.crop = True
        # render.mode['crop'] = True
        render.enableRGBAColor()

        render.render()

        render.setRenderPath("")
        render.saveRenderedImage(outfile)
        print "blender_thumbnail: Wrote output to '%s'" % outfile

        #scn.objects.unlink(cam_obj)
        scn.objects.unlink(light_obj)

        # print "total: %s" % total
        Blender.Quit()
        
render_thumbnail(os.getenv("BLEND_THUMB_AXIS") or "x",
                 int(os.getenv("BLEND_THUMB_RESOLUTION") or 512),
                 os.getenv("BLEND_THUMB_OUTPUT") or "/tmp/out.png")

# EOF #
