/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_DOLL_HPP
#define HEADER_WINDSTILLE_OBJECTS_DOLL_HPP

#include <boost/shared_ptr.hpp>

#include "math/vector2f.hpp"
#include "engine/game_object.hpp"
#include "util/currenton.hpp"

class Sprite3DDrawable;
class Controller;
class EdgePosition;

class Doll : public GameObject,
             public Currenton<Doll>
{
private:
  boost::shared_ptr<Sprite3DDrawable> m_drawable;
  Vector2f m_pos;
  Vector2f m_last_pos;
  boost::scoped_ptr<EdgePosition> m_edge_position;
  
public:
  Doll();
  ~Doll();

  void draw (SceneContext& ) {}
  void update (float delta);
  void update(const Controller& controller, float delta);
  
  Vector2f get_pos() const { return m_pos; }

private:
  Doll(const Doll&);
  Doll& operator=(const Doll&);
};

#endif

/* EOF */
