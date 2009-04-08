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

#include "navgraph_select_tool.hpp"

NavgraphSelectTool::NavgraphSelectTool()
{
}

NavgraphSelectTool::~NavgraphSelectTool()
{
}

bool
NavgraphSelectTool::mouse_down(GdkEventButton* event, WindstilleWidget& wst)
{
  return true;
}

bool
NavgraphSelectTool::mouse_move(GdkEventMotion* event, WindstilleWidget& wst)
{
  return true;
}

bool
NavgraphSelectTool::mouse_up(GdkEventButton* event, WindstilleWidget& wst)
{
  return true;
}

void
NavgraphSelectTool::draw(SceneContext& sc)
{
}

/* EOF */
