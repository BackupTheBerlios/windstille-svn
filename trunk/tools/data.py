import struct, shlex, os.path, math

# SAMPLEFRAMES, only export every nth frame of the animation
DEFAULT_SAMPLERATE = 5
# ZOOM, is multiplied with all vertex coordinates
ZOOM = 32.0
DEFAULT_SPEED = 1.0
SPEED_MULTIPLIER = 9.8

class MeshData:
  def __init__(self, texture_filename):
    # Filename of the used texture
    self.texture_filename = texture_filename

    # [FaceData, ...]
    self.faces            = []

    self.vertices         = []

    # Table to translate (object, index) to new_index
    # Format: [[object, index, new_index], ...]
    self.transtable       = []

  def transtable_has_vertex(self, new_object, new_index):
      for (object, index, new_index) in self.transtable:
          if object == new_object and index == new_object:
              return True
          else:
              return False

  def translate(self, vert):
      """Translate a vertex given as object, index pair to its
      corresponding index, as used in the wsprite file"""
      
      for (object, index, new_index) in self.transtable:
          if object == vert.object and index == vert.index:
              return new_index
      
      print "MeshData: Couldn't translate vertex: %s, %s" % (vert.object, vert.index)
      return 0

  def finalize(self):
      """Reorders vertex indexes and merge vertexes which have the same
      UV coordinates"""

      for face in self.faces:
          for vert in face.verts:
              if not self.transtable_has_vertex(vert.object, vert.index):
                  new_index = len(self.transtable)
                  self.transtable.append([vert.object, vert.index, new_index])
                  self.vertices.append(vert)

class FaceData:
    def __init__(self, verts, normal):
        self.verts   = verts
        self.normal  = normal

class VertexData:
    def __init__(self, object, index, uv, normal):
        self.object = object
        self.index  = index
        self.uv     = uv
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

    # config entry (first_frame, last_frame, speed, samplerate, markers[])
    #  a marker is (name, frame)
    def parse(text):     
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

        lex = shlex.shlex(text)
        lex.wordchars += "."

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

        return actionconfig
    parse = staticmethod(parse)

class ActionData:
  def __init__(self, name, config, frame_data):
    # name as string
    self.name       = name

    # ActionConfig
    self.config     = config

    # FrameData (filled out later in the WindstilleExporter)
    self.frame_data = frame_data

# EOF #
