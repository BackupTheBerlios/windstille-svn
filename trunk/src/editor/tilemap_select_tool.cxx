//  $Id: tilemap_select_tool.cxx,v 1.1 2003/09/23 22:10:40 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <ClanLib/Display/display.h>
#include <ClanLib/Display/input_event.h>
#include "globals.hxx"
#include "editor_tilemap.hxx"
#include "editor_map.hxx"
#include "tilemap_select_tool.hxx"

TileMapSelectTool::TileMapSelectTool(EditorMap* p, EditorTileMap* t)
  : TileMapTool(p), tilemap(t)
{
  active = false;
}

TileMapSelectTool::~TileMapSelectTool()
{
}

void
TileMapSelectTool::draw()
{
  if (rect.get_width() > 0 && rect.get_height() > 0)
    {
      CL_Display::fill_rect (CL_Rect(rect.left  * TILE_SIZE, rect.top    * TILE_SIZE,
                                     rect.right * TILE_SIZE, rect.bottom * TILE_SIZE),
                             CL_Color(255, 255, 255, 100));
    }
}

void
TileMapSelectTool::on_mouse_up  (const CL_InputEvent& event)
{
  if (event.id == CL_MOUSE_LEFT)
    {
      active = false;
      update_selection(parent->screen2tile(event.mouse_pos).x + 1,
                       parent->screen2tile(event.mouse_pos).y + 1);
    }
}

void
TileMapSelectTool::on_mouse_down(const CL_InputEvent& event)
{
  if (event.id == CL_MOUSE_LEFT)
    {
      active = true;
      click_pos = parent->screen2tile(event.mouse_pos);
    }
  else if (event.id == CL_MOUSE_RIGHT)
    {
      rect = CL_Rect();
    }
}

void
TileMapSelectTool::on_mouse_move(const CL_InputEvent& event)
{ 
  if (active)
    {
      update_selection(parent->screen2tile(event.mouse_pos).x + 1,
                       parent->screen2tile(event.mouse_pos).y + 1);
    }
}

void
TileMapSelectTool::update_selection(int x, int y)
{
  rect = CL_Rect(std::min(click_pos.x, x),
                 std::min(click_pos.y, y),
                 std::max(click_pos.x, x),
                 std::max(click_pos.y, y));
}

/* EOF */
