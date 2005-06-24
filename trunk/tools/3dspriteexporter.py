import Blender
from Blender import NMesh

print "------------------"
def writeObject(mesh):
	outfile.write("(windstille-3dsprite\n")
	outfile.write("  (texture \"3dsprites/3dsprite.png\")\n")
	writeVertices(mesh.verts)
	writeFaces(mesh.faces)
	outfile.write(")")

def writeVertices(vertices):
	outfile.write("  (vertices\n")
 	for i in vertices:
		outfile.write("    (vertex (pos %f %f %f) (normal %f %f %f))\n"
						% (i.co[0] * 32,
               i.co[1] * 32,
               i.co[2] * 32,
						   i.no[0], i.no[2], i.no[1]
						   							))
	outfile.write(")\n")
	
def writeFaces(faces):
	outfile.write("  (faces\n")
	for i in faces:
		outfile.write("(face (vert %d %f %f) (vert %d %f %f) (vert %d %f %f))\n" 
		% (i.v[0].index, i.uv[0][0], 1.0 - i.uv[0][1],
		   i.v[1].index, i.uv[1][0], 1.0 - i.uv[1][1],
		   i.v[2].index, i.uv[2][0], 1.0 - i.uv[2][1]))
	outfile.write(")\n")

outfile = open("/tmp/blender.mod", "w")

writeObject(NMesh.GetRaw("Hero"))

outfile.write("\n;; EOF ;;\n")        
outfile.close()
print "---------done-----------"

# EOF #