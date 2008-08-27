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

#include <iostream>
#include <boost/bind.hpp>
#include "math/rect.hpp"
#include "tile_generator.hpp"
#include "math.hpp"
#include "jpeg.hpp"
#include "database_thread.hpp"
#include "tile_generator_thread.hpp"
 
TileGeneratorThread* TileGeneratorThread::current_ = 0; 

TileGeneratorThread::TileGeneratorThread()
  : Thread("TileGeneratorThread"),
    quit(false)
{
  current_ = this;
  working  = false;
}

TileGeneratorThread::~TileGeneratorThread()
{
}

void
TileGeneratorThread::request_tiles(const FileEntry& file_entry, int min_scale, int max_scale,
                                   const boost::function<void (TileEntry)>& callback)
{
  assert(working == false);
  working = true;

  TileGeneratorThreadJob job;

  job.entry     = file_entry;
  job.min_scale = min_scale;
  job.max_scale = max_scale;
  job.callback  = callback;

  assert(msg_queue.empty());
  msg_queue.push(job);
}

void
TileGeneratorThread::stop()
{
  quit = true;
}

void
TileGeneratorThread::process_message(const TileGeneratorThreadJob& job)
{
  std::cout << "Processing: scale: " << job.min_scale << "-" << job.max_scale << " " << job.entry.filename << "..." << std::flush;

  // Find scale at which the image fits on one tile
  int width  = job.entry.size.width;
  int height = job.entry.size.height;
  int scale  = job.min_scale;

  int jpeg_scale = Math::pow2(scale);
  SoftwareSurface surface;

  if (jpeg_scale > 8)
    {
      // The JPEG class can only scale down by factor 2,4,8, so we have to
      // limit things (FIXME: is that true? if so, why?)
      surface = JPEG::load_from_file(job.entry.filename, 8);
      
      surface = surface.scale(Size(width  / Math::pow2(scale),
                                   height / Math::pow2(scale)));
    }
  else
    {
      surface = JPEG::load_from_file(job.entry.filename, jpeg_scale);
    }

  do
    {
      if (scale != job.min_scale)
        {
          surface = surface.halve();
        }

      for(int y = 0; 256*y < surface.get_height(); ++y)
        for(int x = 0; 256*x < surface.get_width(); ++x)
          {
            SoftwareSurface croped_surface = surface.crop(Rect(Vector2i(x * 256, y * 256),
                                                               Size(256, 256)));

            job.callback(TileEntry(job.entry.fileid, scale, Vector2i(x, y), croped_surface));
          }

      scale += 1;

    } while (scale <= job.max_scale);

  working = false;

  std::cout << "done" << std::endl;
}

int
TileGeneratorThread::run()
{
  quit = false;

  TileGenerator generator;

  while(!quit)
    {
      while(!msg_queue.empty())
        {
          TileGeneratorThreadJob job = msg_queue.front();
          msg_queue.pop();
         
          try 
            {
              process_message(job);
            }
          catch(std::exception& err)
            {
              // FIXME: We need a better way to communicate errors back
              std::cout << "\Error: nTileGeneratorThread: Loading failure: " << job.entry.filename << std::endl;
              job.callback(TileEntry());
            }
        }
            
      msg_queue.wait();
    }
  
  return 0;
}

bool
TileGeneratorThread::is_working() const
{
  return working;
}

/* EOF */
