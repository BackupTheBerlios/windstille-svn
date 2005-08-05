/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

#include <string.h>
#include <iostream>
#include "matrix.hpp"

/////////////////////////////////////////////////////////////////////////////
// Matrix construction:

Matrix::Matrix()
{
	memset(matrix, 0, sizeof(float[16]));
}

Matrix
Matrix::identity()
{
	Matrix matrix;

	matrix.matrix[0] = 1.0;
	matrix.matrix[5] = 1.0;
	matrix.matrix[10] = 1.0;
	matrix.matrix[15] = 1.0;
	
	return matrix;
}

Matrix::Matrix(const Matrix &copy)
{
	for (int i=0; i<16; i++)
		matrix[i] = copy.matrix[i];
}

Matrix::Matrix(const float *init_matrix)
{
	for (int i=0; i<16; i++)
		matrix[i] = init_matrix[i];
}

/////////////////////////////////////////////////////////////////////////////
// Matrix attributes:

float Matrix::get_origin_x() const
{
	return matrix[12];
}

float Matrix::get_origin_y() const
{
	return matrix[13];
}

float Matrix::get_origin_z() const
{
	return matrix[14];
}

/////////////////////////////////////////////////////////////////////////////
// Matrix operations:

bool Matrix::operator==(const Matrix &other) const 
{
	for (int i=0; i<16; i++)
		if (matrix[i] != other.matrix[i]) return false;
	return true;
}

bool Matrix::operator!=(const Matrix &other) const
{
	for (int i=0; i<16; i++)
		if (matrix[i] != other.matrix[i]) return true;
	return false;
}

Matrix &Matrix::operator =(const Matrix &copy)
{
	for (int i=0; i<16; i++)
		matrix[i] = copy.matrix[i];
	return *this;
}

Matrix Matrix::multiply(const Matrix &mult) const
{
	Matrix result;
	for (int x=0; x<4; x++)
	{
		for (int y=0; y<4; y++)
		{
			result.matrix[x+y*4] =
				matrix[x]*mult.matrix[y*4] +
				matrix[x+4]*mult.matrix[y*4+1] +
				matrix[x+8]*mult.matrix[y*4+2] +
				matrix[x+12]*mult.matrix[y*4+3];
		}
	}
	return result;
}

std::ostream& operator<<(std::ostream& s, const Matrix& m)
{
  s << "[" << m[ 0] << ", " << m[ 4] << ", " << m[ 8] << ", " << m[12] << "\n";
  s << " " << m[ 1] << ", " << m[ 5] << ", " << m[ 9] << ", " << m[13] << "\n";
  s << " " << m[ 2] << ", " << m[ 6] << ", " << m[10] << ", " << m[14] << "\n";
  s << " " << m[ 3] << ", " << m[ 7] << ", " << m[11] << ", " << m[15] << "]\n";

  return s;
}

/* EOF */
