import Blender
import string

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

##########################################################
def get_text(textname):
  """Little shortcut function to return the content of
  Blender.Text.get(textname) as a single string and do a little error
  handling in addition""" 
  textobj = Blender.Text.Get(textname)
  if not textobj:
    print "WARNING: Text '%s' not found" % textname
    return ""
  else:
    return string.join(textobj.asLines(), "\n")

# EOF #
