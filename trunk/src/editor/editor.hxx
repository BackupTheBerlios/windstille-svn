//  $Id: editor.hxx,v 1.8 2003/10/11 08:11:59 grumbel Exp $
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

#ifndef HEADER_EDITOR_HXX
#define HEADER_EDITOR_HXX

#include <stack>

class CL_PopupMenu;
class CL_MenuData;
class TileEditor;
class EditorTileMap;
class GUIManager;

/** */
class Editor
{
private:
  GUIManager* manager;

  CL_PopupMenu* popupmenu;
  CL_MenuData*  menu_data;

  EditorTileMap* tilemap;

  static Editor* current_;
public:
  static Editor* current() { return current_; }

  Editor();
  ~Editor();

  EditorTileMap* get_editor_tilemap() { return tilemap; }

  void run();

  void popup_menu();

  void load(const std::string& filename);

private:
  Editor (const Editor&);
  Editor& operator= (const Editor&);
};

#endif

/* EOF */
