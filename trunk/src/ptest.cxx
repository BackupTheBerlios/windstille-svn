/*
  Loads a surface from a pcx file.
*/

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include "particle_system.hxx"

void screenshot(const char* filename)
{
  glReadBuffer(GL_FRONT);

  unsigned char data1[512*512*3];
  glReadPixels(0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, data1);

  unsigned char data2[512*512*3];
  for(int i = 0; i < 512; ++i)
    {
      memcpy(&data2[i*(512*3)], &data1[(511-i)*(512*3)], 512*3);
    }

  FILE* f = fopen(filename, "wb");
  fprintf(f, "P6\n"
          "# CREATOR: The GIMP's PNM Filter Version 1.0\n"
          "512 512\n"
          "255\n");
  fwrite(data2, sizeof(unsigned char), sizeof(data2), f);
  fclose(f);
  glReadBuffer(GL_BACK);
}

class SurfaceApp : public CL_ClanApplication
{
public:
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

        unsigned int last_time = CL_System::get_time();

        ParticleSystem psystem2;
        psystem2.set_drawer(new SparkDrawer());
        psystem2.set_pos(0,0);
        psystem2.set_speed(300, 550);
        psystem2.set_cone(-25-90, 25-90);
        psystem2.set_gravity(0, 20);
        psystem2.set_line_distribution(-50, 0, 50, 0);

        ParticleSystem psystem3;
        psystem3.set_lifetime(8);
        psystem3.set_count(30);
        surface2.set_blend_func(blend_src_alpha, blend_one_minus_src_alpha);
        surface2.set_alignment(origin_center);
        psystem3.set_drawer(new SurfaceDrawer(surface2));
        psystem3.set_pos(0,0);
        psystem3.set_speed(70, 100);
        psystem3.set_cone(-25-90, 25-90);
        psystem3.set_gravity(0, -1);
        psystem3.set_size(1.0f, 3.0f);
        psystem3.set_line_distribution(-50, 0, 50, 0);
 
        ParticleSystem psystem;
        psystem.set_count(100);
        surface1.set_blend_func(blend_src_alpha, blend_one);
        surface1.set_alignment(origin_center);
        psystem.set_drawer(new SurfaceDrawer(surface1));
        psystem.set_pos(0,0);
        psystem.set_speed(200, 300);
        psystem.set_cone(-5-90, 5-90);
        psystem.set_gravity(0, 0);
        psystem.set_line_distribution(-50, 0, 50, 0);
        //psystem.set_circle_distribution(100.0f);
        //psystem.set_rect_distribution(200.0f, 50);

        // Loop until the user hits escape:
        int count = 0;
        unsigned int start_time =  CL_System::get_time();
        while (CL_Keyboard::get_keycode(CL_KEY_ESCAPE) == false)
          {
            CL_Display::clear(CL_Color(0, 0, 0));

            psystem.set_spawn_point(CL_Mouse::get_x(), CL_Mouse::get_y());
            psystem2.set_spawn_point(CL_Mouse::get_x(), CL_Mouse::get_y());
            psystem3.set_spawn_point(CL_Mouse::get_x(), CL_Mouse::get_y());
        
            unsigned int cur_time = CL_System::get_time();

            /*psystem.update((cur_time - last_time)/1000.f);
            psystem2.update((cur_time - last_time)/1000.f);
            psystem3.update((cur_time - last_time)/1000.f);*/
            psystem.update(0.033f);
            psystem2.update(0.033f);
            psystem3.update(0.033f);

            last_time = cur_time;

            psystem3.draw();
            psystem.draw();
            psystem2.draw();

            // Flip front and backbuffer. This makes the changes visible:
            CL_Display::flip();

            if (CL_Keyboard::get_keycode(CL_KEY_SPACE))
              {
                screenshot("/tmp/myshot.pnm");
              }

            if (1)
              {char str[1024];
              snprintf(str, 1024, "/tmp/shot%04d.pnm", count);
              //printf("%02d\n");
              screenshot(str);
              if (count == 356)
                exit(0);
              }
            count += 1;

            // Improve responsiveness:
            //CL_System::sleep(30);
				
            // Update keyboard input and handle system events:
            CL_System::keep_alive();
          }

        std::cout << count << " " << CL_System::get_time() - start_time << std::endl;
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
