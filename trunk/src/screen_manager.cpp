/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "game_session.hpp"
#include "globals.hpp"
#include "screen.hpp"
#include "screen_manager.hpp"

ScreenManager::ScreenManager()
{
  screen = 0;
}

ScreenManager::~ScreenManager()
{
}

void
ScreenManager::run()
{
  bool quit = false;
  while (!quit)
    {
      switch (game_main_state)
        {
        case RUN_GAME:
          screen->display();
          break;

        case LOAD_MENU:
          // Fall through and load the game directly as long as we don't
          // have a new menu
        case LOAD_GAME_SESSION:
          delete screen;
          screen = new GameSession("levels/newformat2.wst");
          game_main_state = RUN_GAME;
          break;

        case QUIT_GAME:
          delete screen;
          screen = 0;
          quit = true;
          break;
        }
    }
 
}

void
ScreenManager::set_screen(Screen* s)
{
  delete screen;
  screen = s;
}

/* EOF */
