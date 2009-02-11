/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_WINDSTILLE_GEOMETRY_TEST_HPP
#define HEADER_WINDSTILLE_GEOMETRY_TEST_HPP

#include "screen.hpp"
#include "math/vector.hpp"
#include "math/line.hpp"

/** */
class GeometryTest : public Screen
{
private:
  Vector cursor;
  Vector cursor2;
  Vector collision_point;

  int point_count;

  Line line1;
  Line line2;
  bool had_prev_collision;

public:
  GeometryTest();

  void draw();
  void update(float delta, const Controller& controller);

private:
  GeometryTest (const GeometryTest&);
  GeometryTest& operator= (const GeometryTest&);
};

#endif

/* EOF */
