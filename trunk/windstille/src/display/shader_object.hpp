/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP
#define HEADER_WINDSTILLE_DISPLAY_SHADER_OBJECT_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <boost/shared_ptr.hpp>

class ShaderObjectImpl;

/** */
class ShaderObject
{
public:
  ShaderObject(GLenum type, const std::string& filename);
  ~ShaderObject();

  GLhandleARB get_handle() const;
  void load(const std::string& filename);
  void compile();
  void print_log();

private:
  boost::shared_ptr<ShaderObjectImpl> impl;
};

#endif

/* EOF */
