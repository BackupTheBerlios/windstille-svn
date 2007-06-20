/*  $Id: windstille.hpp 1460 2007-06-18 04:03:31Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
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

#include <config.h>

#include "sprite2d/manager.hpp"
#include "sprite2d/data.hpp"
#include "sprite2d/sprite.hpp"
#include <iostream>

sprite2d::Manager* sprite2d_manager = 0;

namespace sprite2d
{

Manager::Manager()
{
}

Manager::~Manager()
{
}

DataPtr
Manager::create_data(const std::string& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i != datas.end())
    {
      return i->second;
    }
  else
    {  
      DataPtr data(new Data(filename));
      datas.insert(std::make_pair(filename, data));
      return data;
    }
}

void
Manager::cleanup()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    {
      if (i->second.use_count() == 1)
        {
          datas.erase(i);
        }
    }
}

} // namespace sprite2d

/* EOF */
