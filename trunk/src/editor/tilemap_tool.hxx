//  $Id: tilemap_tool.hxx,v 1.1 2003/09/23 19:10:05 grumbel Exp $
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

#ifndef HEADER_TILEMAP_TOOL_HXX
#define HEADER_TILEMAP_TOOL_HXX

class EditorTileMap;
class CL_InputEvent;

/** */
class TileMapTool
{
protected:
  EditorTileMap* tilemap;
public:
  TileMapTool(EditorTileMap* t) : tilemap(t) {}
  virtual ~TileMapTool() {}

  virtual void draw() {}

  virtual void on_mouse_up  (const CL_InputEvent& event) {}
  virtual void on_mouse_down(const CL_InputEvent& event) {}
  virtual void on_mouse_move(const CL_InputEvent& event) {}

private:
  TileMapTool (const TileMapTool&);
  TileMapTool& operator= (const TileMapTool&);
};

#endif

/* EOF */
