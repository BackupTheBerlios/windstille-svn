//  $Id: tile_map.cxx,v 1.18 2003/11/05 11:09:36 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <ClanLib/gl.h>
#include <sstream>
#include "lisp/list_iterator.hpp"
#include "tile_map.hxx"
#include "tile.hxx"
#include "tile_factory.hxx"
#include "globals.hxx"
#include "view.hxx"
#include "display/drawing_request.hxx"

extern CL_ResourceManager* resources;

TileMap::TileMap(const lisp::Lisp* lisp)
{
  int width = -1;
  int height = -1;
  z_pos = 0;
  
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "name") {
      name = iter.value().get_string();
    } else if(iter.item() == "z-pos") {
      z_pos = iter.value().get_float();
    } else if(iter.item() == "width") {
      width = iter.value().get_int();
    } else if(iter.item() == "height") {
      height = iter.value().get_int();
    } else if(iter.item() == "data") {
      if(width <= 0 || height <= 0) {
        throw std::runtime_error(
            "Invalid width or height defined or "
            "data defined before width and height");
      }
      Field<int> tmpfield(width, height);
      iter.lisp()->get_vector(tmpfield.get_vector());
    
      field = Field<Tile*>(width, height);

      for (int y = 0; y < field.get_height (); ++y) 
        {
          for (int x = 0; x < field.get_width (); ++x)
            {
              field(x, y) = TileFactory::current()->create(tmpfield(x, y));
            }
        }
    } else {
      std::cout << "Skipping unknown Tag '" << iter.item() << "' in tilemap\n";
    }
  }

  if(field.size() == 0)
    throw std::runtime_error("No tiles defined in tilemap");  
}

TileMap::TileMap (Field<int>* data)
  : field(data->get_width(),
          data->get_height())
{
  if (debug)
    {
      std::cout << "TileMap: Size: " 
                << data->get_width() << "x" << data->get_height() << std::endl;
    }

  for (int y = 0; y < field.get_height (); ++y) 
    {
      for (int x = 0; x < field.get_width (); ++x)
        {
          field(x, y) = TileFactory::current()->create((*data)(x, y));
        }
    }
}

TileMap::~TileMap()
{
}

void 
TileMap::update (float )
{
  /*for (FieldIter i = field.begin (); i != field.end (); ++i)
    {
      (*i)->update (delta);
      }*/
}

class TileMapDrawingRequest : public DrawingRequest
{
private:
  TileMap* tilemap;
  bool highlight;
  CL_Rect rect;
public:
  TileMapDrawingRequest(TileMap* tilemap_, bool highlight_,
                        const CL_Rect& rect_,
                        const CL_Vector& pos, const CL_Matrix4x4& modelview)
    : DrawingRequest(pos, modelview),
      tilemap(tilemap_),
      highlight(highlight_),
      rect(rect_)
  {}

  void draw(CL_GraphicContext* gc)
  {
    gc->push_modelview();
    gc->add_modelview(modelview);

    draw_classic(gc);

    gc->pop_modelview();
  }

  void draw_new(CL_GraphicContext* gc)
  {
    Field<Tile*>& field = tilemap->field;
    std::vector<float> raw_texcoords;
    std::vector<float> raw_vertices;

    // FIXME: handle texture somewhere here
    for (int y = rect.top;   y < rect.bottom; ++y)
      for (int x = rect.left; x < rect.right; ++x)
        {
          Tile* tile = field(x, y);
          if (tile && !highlight)
            {
              raw_vertices.push_back(x * TILE_SIZE);
              raw_vertices.push_back(y * TILE_SIZE);
              raw_vertices.push_back(0);

              raw_texcoords.push_back(tile->color_rect.left / 1024.0f);
              raw_texcoords.push_back(tile->color_rect.top  / 1024.0f);


              raw_vertices.push_back(x * TILE_SIZE + TILE_SIZE);
              raw_vertices.push_back(y * TILE_SIZE);
              raw_vertices.push_back(0);

              raw_texcoords.push_back(tile->color_rect.right / 1024.0f);
              raw_texcoords.push_back(tile->color_rect.top  / 1024.0f);


              raw_vertices.push_back(x * TILE_SIZE + TILE_SIZE);
              raw_vertices.push_back(y * TILE_SIZE + TILE_SIZE);
              raw_vertices.push_back(0);

              raw_texcoords.push_back(tile->color_rect.right / 1024.0f);
              raw_texcoords.push_back(tile->color_rect.bottom  / 1024.0f);


              raw_vertices.push_back(x * TILE_SIZE);
              raw_vertices.push_back(y * TILE_SIZE + TILE_SIZE);
              raw_vertices.push_back(0);

              raw_texcoords.push_back(tile->color_rect.left / 1024.0f);
              raw_texcoords.push_back(tile->color_rect.bottom  / 1024.0f);
            }
        }

    std::vector<float> raw_data;

    // FIXME: this can be optimized away
    std::copy(raw_vertices.begin(),  raw_vertices.end(),  std::back_inserter(raw_data));
    std::copy(raw_texcoords.begin(),  raw_texcoords.end(),  std::back_inserter(raw_data));

    int texcoord_offset = raw_vertices.size() * sizeof(float);

    float* data = &*raw_data.begin();

    clVertexPointer  (3, CL_FLOAT, 0, data);
    clTexCoordPointer(2, CL_FLOAT, 0, data + texcoord_offset);
    
    clEnableClientState(CL_TEXTURE_COORD_ARRAY);
    clEnableClientState(CL_VERTEX_ARRAY);

    // Draw arrays
    clDrawArrays(CL_QUADS, 0, raw_vertices.size()/4);

    // Disable arrays
    clDisableClientState(CL_TEXTURE_COORD_ARRAY);
    clDisableClientState(CL_VERTEX_ARRAY);
  }

  void draw_classic(CL_GraphicContext* gc)
  {
    Field<Tile*>& field = tilemap->field;

    for (int y = rect.top;   y < rect.bottom; ++y)
      for (int x = rect.left; x < rect.right; ++x)
        {
          if (field(x,y))
            {
              if (!highlight)
                {
                  field(x,y)->get_color_sprite().draw(x * TILE_SIZE, y * TILE_SIZE);
                }
              else
                {
                  if (field(x, y)->get_highlight_sprite())
                    field(x,y)->get_highlight_sprite().draw(x * TILE_SIZE, y * TILE_SIZE);
                }
            }
        }
  }
};

void
TileMap::draw (SceneContext& sc)
{
  CL_Rect rect = CL_Rect(View::current()->get_clip_rect());

  int start_x = std::max(0, rect.left/TILE_SIZE);
  int start_y = std::max(0, rect.top/TILE_SIZE);
  int end_x   = std::min(field.get_width(),  rect.right/TILE_SIZE + 1);
  int end_y   = std::min(field.get_height(), rect.bottom/TILE_SIZE + 1);

  sc.color().draw(new TileMapDrawingRequest(this, false, CL_Rect(start_x, start_y,
                                                                 end_x,   end_y),
                                            CL_Vector(0,0,z_pos),
                                            sc.color().get_modelview()));
  sc.highlight().draw(new TileMapDrawingRequest(this, true, CL_Rect(start_x, start_y,
                                                                    end_x,   end_y),
                                                CL_Vector(0,0,z_pos),
                                                sc.color().get_modelview()));
}

unsigned int
TileMap::get_pixel(int x, int y)
{
  if (x < 0 || y < 0 
      || x >= int(field.get_width())
      || y >= int(field.get_height()))
    {
      //std::cout << "Out of bounce: " << x << ", " << y << std::endl;
      return 0;
    }
  else
    {
      Tile* tile = field(x, y);
      
      if (tile)
        return tile->get_colmap();
      else
        return 0;     
    }
}

bool
TileMap::is_ground (float x, float y)
{
  int x_pos = int(x) / TILE_SIZE;
  int y_pos = int(y) / TILE_SIZE;

  if (x < 0 || x_pos >= field.get_width())
    {
      //std::cout << "TileMap::is_ground (): Out of range: " << x_pos << " " << y_pos << std::endl;
      return 1;
    }
  else if (y < 0 || y_pos >= field.get_height())
    {
      return 0;
    }


  if (field (x_pos, y_pos))
    return field(x_pos, y_pos)->get_colmap();
  else
    return 0;
}

/* EOF */
