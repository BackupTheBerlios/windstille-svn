/*
**  Galapix - an image viewer for large image collections
**  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_TILE_DATABASE_HPP
#define HEADER_TILE_DATABASE_HPP

#include "sqlite.hpp"
#include "math/vector2i.hpp"
#include "software_surface.hpp"

class TileEntry;

/** 
 */
class TileDatabase
{
private:
  SQLiteConnection* db;
  SQLiteStatement store_stmt;
  SQLiteStatement get_stmt;
  SQLiteStatement get_all_stmt;
  SQLiteStatement has_stmt;

public:
  TileDatabase(SQLiteConnection* db);
  
  bool has_tile(uint32_t file_id, const Vector2i& pos, int scale);
  bool get_tile(uint32_t file_id, int scale, const Vector2i& pos, TileEntry& tile);
  void store_tile(const TileEntry& tile);
  void check();
private:
  TileDatabase (const TileDatabase&);
  TileDatabase& operator= (const TileDatabase&);
};

#endif

/* EOF */
