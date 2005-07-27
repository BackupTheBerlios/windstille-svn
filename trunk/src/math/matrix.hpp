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

#ifndef HEADER_MATH_MATRIX_HPP
#define HEADER_MATH_MATRIX_HPP

#include <ClanLib/Core/Math/matrix4x4.h>
#include <iosfwd>

//: 4x4 Matrix.
class Matrix
{
//! Construction:
public:
	//: Constructs a 4x4 matrix.
	Matrix();

	Matrix(const Matrix &copy);

	Matrix(double *matrix);

	Matrix(float *matrix);

	/** Returns identity matrix */
	static Matrix identity();

//! Attributes:
public:
	double matrix[16];

	//: Operator that returns the matrix cell at the given index.
	double &operator[](int i) { return matrix[i]; }

	//: Operator that returns the matrix cell at the given index.
	const double &operator[](int i) const { return matrix[i]; }

	//: Operator that returns the matrix cell at the given index.
	double &operator[](unsigned int i) { return matrix[i]; }

	//: Operator that returns the matrix cell at the given index.
	const double &operator[](unsigned int i) const { return matrix[i]; }

	//: Returns the x coordinate for the point (0,0,0) multiplied with this matrix.
	double get_origin_x() const;

	//: Returns the y coordinate for the point (0,0,0) multiplied with this matrix.
	double get_origin_y() const;

	//: Returns the z coordinate for the point (0,0,0) multiplied with this matrix.
	double get_origin_z() const;

//! Operations:
public:
	//: Copy assignment operator.
	Matrix &operator =(const Matrix &copy);

	//: Equality operator.
	bool operator==(const Matrix &other) const;

	//: Not-equal operator.
	bool operator!=(const Matrix &other) const;

	//: Multiply two matrices.
	Matrix multiply(const Matrix &matrix) const;

//! Implementation:
private:
};

std::ostream& operator<<(std::ostream& s, const Matrix& m);

#endif

/* EOF */
