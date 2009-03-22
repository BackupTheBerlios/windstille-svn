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

#ifndef HEADER_SECTOR_MODEL_HPP
#define HEADER_SECTOR_MODEL_HPP

#include <gtkmm/treestore.h>
#include <vector>
#include "math/vector2f.hpp"

class SceneContext;

class SectorModel
{
private:
  Glib::RefPtr<Gtk::TreeStore> objects_tree;
  std::vector<Vector2f> objects;

public:
  SectorModel();

  void add(const Vector2f& obj);
  void draw(SceneContext& sc);

private:
  SectorModel(const SectorModel&);
  SectorModel& operator=(const SectorModel&);
};

#endif

/* EOF */
