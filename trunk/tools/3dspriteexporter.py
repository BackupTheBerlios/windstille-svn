import Blender
from Blender import NMesh

print "------------------"
def writeObject(mesh):
	outfile.write("(windstille-3dsprite\n")
	writeVertices(mesh.verts)
	writeFaces(mesh.faces)
	outfile.write(")")

def writeVertices(vertices):
	outfile.write("  (vertices\n")
 	for i in vertices:
		outfile.write("    (vertex %f %f %f)\n"
						% (i[0] * 32,
                            i[2] * 32,
                            i[1] * 32))
	outfile.write(")\n")

def writeFaces(faces):
	outfile.write("  (faces\n")
	for i in faces:
		outfile.write("(face (vert %d 0 0) (vert %d 0 0) (vert %d 0 0))\n" 
		% (i.v[0].index, i.v[1].index, i.v[2].index))
	outfile.write(")\n")

outfile = open("/tmp/blender.mod", "w")

writeObject(NMesh.GetRaw("Hero"))

outfile.write("\n;; EOF ;;\n")        
outfile.close()
print "---------done-----------"

# EOF #