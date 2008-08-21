/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_TILE_GENERATOR_HPP
#define HEADER_TILE_GENERATOR_HPP

#include <boost/function.hpp>
#include <string>
#include "software_surface.hpp"
#include "tile_entry.hpp"

class FileEntry;

class TileGenerator
{
private:

public:
  TileGenerator();
  ~TileGenerator();

  /** Slow brute force approach to generate tiles, works with all
      image formats */
  void generate_all(int fileid, const SoftwareSurface& surface,
                    const boost::function<void (TileEntry)>& callback);

  void generate_all(int fileid, const std::string& filename,
                    const boost::function<void (TileEntry)>& callback);

  void generate_quick(const FileEntry& entry,
                      const boost::function<void (TileEntry)>& callback);

private:
  TileGenerator (const TileGenerator&);
  TileGenerator& operator= (const TileGenerator&);
};

#endif

/* EOF */
