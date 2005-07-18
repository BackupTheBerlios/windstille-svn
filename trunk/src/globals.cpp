//  $Id: globals.cxx,v 1.6 2003/11/07 22:41:18 grumbel Exp $
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

#include "globals.hpp"

std::string datadir;

int TILE_SIZE    = 32;
int TILE_RESOLUTION = 64;

float text_speed = 15.0f;
float game_speed = 1.0f;
#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif
bool collision_debug = false;
bool sound_disabled = false;
GameMainState game_main_state;

/* EOF */
