import Blender
from Blender.Mathutils import Vector
from Blender import Camera
from Blender.Scene import Render
import time

print "\033c--- Start --- %s" % time.time()

scn = Blender.Scene.GetCurrent()

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

    def normalize(self):
        self.x1 = min(self.x1, self.x2)
        self.x2 = max(self.x2, self.x1)

        self.y1 = min(self.y1, self.y2)
        self.y2 = max(self.y2, self.y1)

        self.z1 = min(self.z1, self.z2)
        self.z2 = max(self.z2, self.z1)

    def join(self, bbox):
        self.x1 = min(self.x1, bbox.x1) or bbox.x1
        self.x2 = max(self.x2, bbox.x2) or bbox.x2

        self.y1 = min(self.y1, bbox.y1) or bbox.y1
        self.y2 = max(self.y2, bbox.y2) or bbox.y2

        self.z1 = min(self.z1, bbox.z1) or bbox.z1
        self.z2 = max(self.z2, bbox.z2) or bbox.z2

    def __str__(self):
        return "[bbox: x:%.2f, %.2f, y:%.2f, %.2f, z:%.2f, %.2f]" % (self.x1, self.x2,
                               self.y1, self.y2,
                               self.z1, self.z2)

# Calculate the bounding box of a scene
def bounding_rect():
    total = BBox()

    for obj in scn.objects:       
        if obj.boundingBox:
            print obj.getType()
            print obj.boundingBox
            print total.join(BBox(obj.boundingBox))

    cam = Camera.New('ortho')
    
    cam.scale = max(total.x2 - total.x1, total.y2 - total.y1) + 1.0
    
    cam_obj = scn.objects.new(cam)
    
    scn.setCurrentCamera(cam_obj)
    
    cam_obj.setLocation((total.x2 + total.x1)/2,
                        (total.y2 + total.y1)/2,
                        total.z2 + 5)

    render = scn.getRenderingContext()
    
    render.aspectX = 100
    render.aspectY = 100
    
    render.sizeX = 512
    render.sizeY = 512
    
    print "total: %s" % total

bounding_rect()

# EOF #


