/*
  Loads a surface from a pcx file.
*/

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include "collision/collision_manager.hxx"

class SurfaceApp : public CL_ClanApplication
{
public:
  void draw(CL_Sizef p, CL_Vector pos, CL_Color color)
  {
    CL_Display::fill_rect(CL_Rect(CL_Point((int)pos.x, (int)pos.y), CL_Size((int)p.width, (int)p.height)),
                          color);
  }
  
  virtual int main(int, char **)
  {
    // Create a console window for text-output if not available
    CL_ConsoleWindow console("Console");
    console.redirect_stdio();

    try
      {
        CL_SetupCore setup_core;
        CL_SetupDisplay setup_display;
        CL_SetupGL setup_gl;

        CL_DisplayWindow window("Surface Example", 512, 512);

        CL_Surface surface1("smoke.png");
        CL_Surface surface2("smoke2.png");

        CollisionManager colmgr;

        // Loop until the user hits escape:

        CL_Sizef rect1(150, 150);
        CL_Sizef rect2(100, 30);
        
        CL_Vector p0;
        while (CL_Keyboard::get_keycode(CL_KEY_ESCAPE) == false)
          {
            CL_Display::clear(CL_Color(0, 0, 0));

            CL_Vector p1(CL_Mouse::get_x(), CL_Mouse::get_y());
            CL_Vector p2(256, 256);
            CL_Vector p3(256, 256);
            CL_Sizef a(50, 25);
            CL_Sizef b(100, 125);

            CollisionManager::CollisionResult res = colmgr.sweep(a, p0, p1,
                                                                 b, p2, p3);
            
            if (res.side != CollisionManager::CO_NONE && res.side != CollisionManager::CO_UNKNOWN)
              {
                std::cout << "Collision: " << res.side
                          << " " << p0 << " " << p1
                          << std::endl;;
              }

            draw(a, p0, CL_Color(155, 0, 0));
            draw(a, p1, CL_Color(255, 0, 0));

            draw(b, p3, CL_Color(155, 155, 155));

            if (res.side != CollisionManager::CO_NONE)
              draw(b, p2, CL_Color(255, 255, 255));
            else
              draw(b, p2, CL_Color(255, 0, 255));
            
            p0 = p1;
#if 0
            CL_Vector p1(CL_Mouse::get_x(), CL_Mouse::get_y());
            CL_Vector p2(256, 256);
            
            float a1 = p0.x - 50;
            float a2 = p0.x + 50;

            float b1 = p2.x - 50;
            float b2 = p2.x + 50;

            float u0 = 0.0f;
            float u1 = 1.0f;

            colmgr.sweep_helper(a1, a2, b1, b2, p1.x - p0.x, u0, u1);

            CL_Display::fill_rect(CL_Rect((int)std::min(a1, (p1.x - 50)), 0,
                                          (int)std::max(a2, (p1.x + 50)), 50),
                                  CL_Color(255, 0, 255));

            CL_Display::fill_rect(CL_Rect((int)(p1.x - 50), 0, (int)(p1.x + 50), 50),
                                  CL_Color(255, 0, 0));

            CL_Display::draw_rect(CL_Rect((int)(p0.x - 50), 0, (int)p0.x + 50, 50),
                                  CL_Color(255, 255, 0));

            CL_Display::fill_rect(CL_Rect((int)b1, (int)25, (int)b2, 75),
                                  CL_Color(255, 255, 0));

            if (u0 > 0 && u0 < 1.0f) {
              CL_Display::draw_rect(CL_Rect((int)a1 + u0*(p1.x - p0.x), (int)40, 
                                            (int)a2 + u0*(p1.x - p0.x), 60),
                                    CL_Color(255, 255, 255));
            } else {
              CL_Display::draw_rect(CL_Rect((int)a1 + u0*(p1.x - p0.x), (int)40, 
                                            (int)a2 + u0*(p1.x - p0.x), 60),
                                    CL_Color(255, 155, 255));
            }

            if (u1< 1.0f ) {
              CL_Display::draw_rect(CL_Rect((int)a1 + u1*(p1.x - p0.x), (int)40, 
                                            (int)a2 + u1*(p1.x - p0.x), 60),
                                    CL_Color(155, 155, 155));
            }

            std::cout << u0 << " " << u1 << " " << (p1.x - p0.x) << std::endl;
#endif


            // Flip front and backbuffer. This makes the changes visible:
            CL_Display::flip();

            // Improve responsiveness:
            CL_System::sleep(30);
				
            // Update keyboard input and handle system events:
            CL_System::keep_alive();
          }

        //std::cout << std::endl;
      }
    catch (CL_Error err)
      {
        std::cout << "Error: " << err.message.c_str() << std::endl;

        // Display console close message and wait for a key
        console.display_close_message();
      }

    return 0;
  }
} app;


/* Compile with:

g++-3.4 -Wall -O2 `pkg-config --cflags --libs clanDisplay-0.7 clanApp-0.7 clanGL-0.7` ptest.cxx particle_system.cxx random.cxx -o ptest

*/
