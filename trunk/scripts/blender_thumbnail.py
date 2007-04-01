## Usage:
## blender longshot.blend -P ~/projects/windstille/trunk/scripts/blender_thumbnail.py
## ! Argument order matters !

## Bugs: The .blend file must not be in "Maximized Window Mode" (Ctrl
## UpArrow) or it will segfault

## ToDo:
## - set some better rendering parameter, care less about what is set
## - in the .blend file (background, ambient occulsion, etc.)

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

class ThumbnailRender:
    """
    bbox:  bounding box
    scene: scene to render
    cam_obj:   light object for the render
    light_obj: camera object for the render
    """
    def __init__(self, scene):
        self.scene = scene
        self.bbox  = None
        self.cam_obj   = None
        self.light_obj = None
        
    def calc_bbox(self):
        self.bbox = BBox()
        for obj in self.scene.objects:
            if obj.getType() != "Empty" and \
               obj.getType() != "Light" and \
               (1 in obj.layers) and obj.boundingBox:
                # print obj.getType()
                # print obj.boundingBox
                self.bbox.join(BBox(obj.boundingBox))

        if not self.bbox.valid:
            raise "Error: Scene is empty!"

    def place_camera(self, longitude, latitude, rotation):
        if not self.bbox:
            self.calc_bbox()

        if not self.cam_obj:
            cam = Camera.New('ortho')

            light = Lamp.New('Sun')            # create new 'Spot' lamp data
            light.energy = 1.0

            self.light_obj = self.scene.objects.new(light)
            self.cam_obj   = self.scene.objects.new(cam)
            self.scene.setCurrentCamera(self.cam_obj)

            cam.scale = self.bbox.max_diagonal()

        # Position camera and light
        matrix = RotationMatrix(rotation, 4, 'z') * \
                 RotationMatrix(90, 4, 'x') * \
                 TranslationMatrix(Vector(0, -self.bbox.max_diagonal()-2,0)) * \
                 RotationMatrix(latitude,  4, 'x') * \
                 RotationMatrix(longitude, 4, 'z') * \
                 TranslationMatrix(self.bbox.center())
        self.cam_obj.setMatrix(matrix)
        self.light_obj.setMatrix(matrix)
        
    def render(self, resolution, outfile):
        render = self.scene.getRenderingContext()

        render.renderwinSize = 100

        render.aspectX = 100
        render.aspectY = 100

        render.sizeX = resolution
        render.sizeY = resolution

        render.imageType = Render.PNG

        render.enableRGBAColor()

        render.render()

        render.setRenderPath("")
        render.saveRenderedImage(outfile)

    def cleanup(self):
        self.scene.objects.unlink(self.cam_obj)
        self.scene.objects.unlink(self.light_obj)
        self.cam_obj = None
        self.light   = None


def resize_list(lst, num):
    """Resizes a list to num elements, repeating the last one"""
    while len(lst) < num:
        lst.append(lst[-1])

def main():
    # longitude: rotations around z-axis
    # latitude:  rotations around x/y-axis
    # rotation:  rotates the final image
    
    positions   = (os.getenv("BLEND_THUMB_POS") or "0,0,0").split(';')
    resolutions = (os.getenv("BLEND_THUMB_RESOLUTION") or "512").split(';')
    outfiles    = (os.getenv("BLEND_THUMB_OUTPUT") or "/tmp/out.png").split(';')

    num_passes = max(len(positions), len(resolutions), len(outfiles))

    resize_list(positions,   num_passes)
    resize_list(resolutions, num_passes)
    resize_list(outfiles,    num_passes)

    renderer = ThumbnailRender(Blender.Scene.GetCurrent())
        
    for (position, resolution, outfile) in zip(positions, resolutions, outfiles):
        print "Rendering: %s %s => %s" % (position, resolution, outfile)
        
        (longitude, latitude, rotation) = position.split(',')
        longitude = -int(longitude)
        latitude  = -int(latitude)
        rotation  =  int(rotation)
        
        renderer.place_camera(longitude, latitude, rotation)
        renderer.render(int(resolution), outfile)
    
    renderer.cleanup()
    
    Blender.Quit()

main()
        
# EOF #
