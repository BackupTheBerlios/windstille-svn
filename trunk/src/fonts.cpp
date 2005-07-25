//  $Id$
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
#include "fonts.hpp"

CL_Font Fonts::dialog;
CL_Font Fonts::dialog_h;

CL_Font Fonts::menu;
CL_Font Fonts::menu_h;

CL_Font Fonts::copyright;

TTFFont* Fonts::ttffont = 0;

TTFFont* Fonts::ttfdialog = 0;

void
Fonts::init()
{
  dialog   = CL_Font("font", resources);
  dialog_h = CL_Font("font_h", resources);

  menu   = CL_Font("menu", resources);
  menu_h = CL_Font("menu_h", resources);

  copyright = CL_Font("copyright", resources);

  ttffont = new TTFFont("fonts/VeraMono.ttf", 12);
  ttfdialog  = new TTFFont("fonts/Vera.ttf", 20);
}

void
Fonts::deinit()
{
  dialog   = CL_Font();
  dialog_h = CL_Font();

  menu   = CL_Font();
  menu_h = CL_Font();

  copyright = CL_Font();

  delete ttffont;
  delete ttfdialog;
}

/* EOF */
