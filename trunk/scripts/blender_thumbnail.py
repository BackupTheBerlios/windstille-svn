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

    def center(self):
        return Vector((self.x1 + self.x2)/2, 
                      (self.y1 + self.y2)/2, 
                      (self.z1 + self.z2)/2)

    def max(self):
        """Returns the longest side of the bbox"""
        return max(self.x2 - self.x1,
                   self.y2 - self.y1,
                   self.z2 - self.z1)

    def max_diagonal(self):
        return math.sqrt(self.width_x() ** 2 +
                         self.width_y() ** 2 +
                         self.width_z() ** 2)

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

def render_thumbnail(longitude, latitude, rotation, resolution, outfile):
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
        # Position of camera and support objects

        ### Add camera
        cam = Camera.New('ortho')
        # cam.scale += cam.scale * 0.1
        cam_obj = scn.objects.new(cam)
        scn.setCurrentCamera(cam_obj)

        ### Add lamp
        light = Lamp.New('Sun')            # create new 'Spot' lamp data
        light.energy = 1.0
        # light.setMode('Square', 'Shadow')   # set these two lamp mode flags
        light_obj = scn.objects.new(light)

        cam.scale = total.max_diagonal()
        matrix = RotationMatrix(rotation, 4, 'z') * \
                 RotationMatrix(90, 4, 'x') * \
                 TranslationMatrix(Vector(0, -total.max_diagonal()-2,0)) * \
                 RotationMatrix(latitude,  4, 'x') * \
                 RotationMatrix(longitude, 4, 'z') * \
                 TranslationMatrix(total.center())
        cam_obj.setMatrix(matrix)
        light_obj.setMatrix(matrix)

        render = scn.getRenderingContext()

        render.renderwinSize = 100

        render.aspectX = 100
        render.aspectY = 100

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

        scn.objects.unlink(cam_obj)
        scn.objects.unlink(light_obj)

        # print "total: %s" % total
        Blender.Quit()

# longitude: rotations around z-axis
# latitude:  rotations around x/y-axis
# rotation:  rotates the final image

pos = os.getenv("BLEND_THUMB_POS") or "0,0,0"
(longitude, latitude, rotation) = pos.split(',')
longitude = -int(longitude) + -90
latitude  = -int(latitude)
rotation  =  int(rotation)

render_thumbnail(longitude, latitude, rotation,
                 int(os.getenv("BLEND_THUMB_RESOLUTION") or 512),
                 os.getenv("BLEND_THUMB_OUTPUT") or "/tmp/out.png")

# EOF #
