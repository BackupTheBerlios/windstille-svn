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

#ifndef HEADER_IMAGE_HPP
#define HEADER_IMAGE_HPP

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include "job_handle.hpp"
#include "math/vector2f.hpp"
#include "math/size.hpp"
#include "grid.hpp"
#include "surface.hpp"

class Surface;
class Size;
class Rectf;
class Vector2f;
class ImageImpl;
class FileEntry;
class TileEntry;

class Image
{
public:
  struct SurfaceStruct {
    enum Status { SURFACE_OK,
                  SURFACE_REQUESTED,
                  SURFACE_FAILED };

    Status  status;
    Surface surface;
  };

  typedef std::map<uint32_t, SurfaceStruct> Cache; 
  typedef std::vector<JobHandle> Jobs;

private:
  Surface get_tile(int x, int y, int tile_scale);

  void cache_cleanup();
  void process_queue();
  Surface find_smaller_tile(int x, int y, int tiledb_scale, int& downscale_in);

public:
  Image();
  Image(const FileEntry& file_entry);

  void draw_tile(int x, int y, int tiledb_scale, const Vector2f& rect, float scale);
  void draw_tiles(const Rect& rect, int tiledb_scale, const Vector2f& pos, float scale);
  void draw(const Rectf& cliprect, float scale);

  void update_pos(float progress);

  void set_target_pos(const Vector2f& target_pos);

  void set_pos(const Vector2f& pos);
  Vector2f get_pos() const;

  void  set_scale(float f);
  float get_scale() const;

  float get_scaled_width() const;
  float get_scaled_height() const;

  int get_original_width() const;
  int get_original_height() const;

  operator bool() const { return impl.get(); }

  /** Syncronized function to require data from other threads */
  void receive_tile(const TileEntry& tile_entry);

private:
  boost::shared_ptr<ImageImpl> impl;
};

#endif

/* EOF */
