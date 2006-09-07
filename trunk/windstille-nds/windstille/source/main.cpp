
// Simple console print demo
// -- dovoto

#include <nds.h>
#include <nds/arm9/console.h> //basic print funcionality
#include <stdio.h>
#include <nds/arm9/sound.h>             // sound functions
#include <nds/arm9/video.h>             // sound functions

//makefile automaticly makes a header file for access
//to the binary data in any file ending in .bin that is in
//the data folder.  It also links in that data to your project

#include "map_img_bin.h"
#include "health_img_bin.h"
#include "log_img_bin.h"
#include "debug_img_bin.h"

#include "items_img_bin.h"
#include "scan_img_bin.h"
#include "comm_img_bin.h"
#include "weapon_img_bin.h"

#include "submap_img_bin.h"
#include "subhealth_img_bin.h"
#include "sublog_img_bin.h"
#include "subdebug_img_bin.h"

#include "subitems_img_bin.h"
#include "subscan_img_bin.h"
#include "subcomm_img_bin.h"
#include "subweapon_img_bin.h"

#include "realmap_img_bin.h"
#include "subscreen_pal_bin.h"

#include "sprite_img_bin.h"
#include "sprite_pal_bin.h"

#include "topscreen_bin.h"
#include "blaster_bin.h"
#include "numbers_img_bin.h"
#include "numbers_pal_bin.h"

#include "titlescreen_pal_bin.h"
#include "titlescreen_img_bin.h"

#include "subtitlescreen_pal_bin.h"
#include "subtitlescreen_img_bin.h"

#include "walk_frame00_img_bin.h"
#include "walk_frame01_img_bin.h"
#include "walk_frame02_img_bin.h"
#include "walk_frame03_img_bin.h"
#include "walk_frame04_img_bin.h"
#include "walk_frame05_img_bin.h"
#include "walk_frame06_img_bin.h"
#include "walk_frame07_img_bin.h"
#include "walk_frame08_img_bin.h"
#include "walk_frame09_img_bin.h"
#include "walk_frame10_img_bin.h"
#include "walk_frame11_img_bin.h"
#include "walk_frame12_img_bin.h"
#include "walk_frame13_img_bin.h"
#include "walk_frame14_img_bin.h"
#include "walk_frame15_img_bin.h"

#include "walk_frame00_pal_bin.h"
#include "walk_frame01_pal_bin.h"
#include "walk_frame02_pal_bin.h"
#include "walk_frame03_pal_bin.h"
#include "walk_frame04_pal_bin.h"
#include "walk_frame05_pal_bin.h"
#include "walk_frame06_pal_bin.h"
#include "walk_frame07_pal_bin.h"
#include "walk_frame08_pal_bin.h"
#include "walk_frame09_pal_bin.h"
#include "walk_frame10_pal_bin.h"
#include "walk_frame11_pal_bin.h"
#include "walk_frame12_pal_bin.h"
#include "walk_frame13_pal_bin.h"
#include "walk_frame14_pal_bin.h"
#include "walk_frame15_pal_bin.h"


#include "topscreen_img_bin.h"
#include "topscreen_pal_bin.h"

SpriteEntry sprites[128];

//turn off all the sprites
void initSprites(void)
{
  for(int i = 0; i < 128; i++)
    {
      sprites[i].attribute[0] = ATTR0_DISABLED;
      sprites[i].attribute[1] = 0;
      sprites[i].attribute[2] = 0;
      sprites[i].attribute[3] = 0;
    }
}

int mod(int a, int b)
{
  if (a < 0)
    return (a % b) + b;
  else
    return (a % b);
}

//copy our sprite to object attribute memory
void updateOAM(void)
{
  DC_FlushAll();
  dmaCopy(sprites, OAM, 128 * sizeof(SpriteEntry));
}

void titlescreen()
{
  videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

  vramSetMainBanks(VRAM_A_MAIN_BG_0x6000000, VRAM_B_LCD,
                   VRAM_C_SUB_BG, VRAM_D_LCD); 

  BG3_CR     = BG_BMP8_256x256 | BG_WRAP_ON;
  SUB_BG3_CR = BG_BMP8_256x256 | BG_WRAP_ON;

  // these are rotation backgrounds so you must set the rotation attributes:
  // these are fixed point numbers with the low 8 bits the fractional part
  // this basicaly gives it a 1:1 translation in x and y so you get a nice flat bitmap
  BG3_XDX = 1 << 8;
  BG3_XDY = 0;
  BG3_YDX = 0;
  BG3_YDY = 1 << 8;

  SUB_BG3_XDX = 1 << 8;
  SUB_BG3_XDY = 0;
  SUB_BG3_YDX = 0;
  SUB_BG3_YDY = 1 << 8;

  BG3_CX = 0;
  BG3_CY = 0;

  SUB_BG3_CX = 0;
  SUB_BG3_CY = 0;

  for(int i = 0; i < 256*192; ++i)
    BG_GFX[i] = ((u16*)titlescreen_img_bin)[i];
  for(int i = 0; i < 256; ++i)
    BG_PALETTE[i] = ((u16*)titlescreen_pal_bin)[i];

  for(int i = 0; i < 256*192; ++i)
    BG_GFX_SUB[i] = ((u16*)subtitlescreen_img_bin)[i];
  for(int i = 0; i < 256; ++i)
    BG_PALETTE_SUB[i] = ((u16*)subtitlescreen_pal_bin)[i];
  
  int count = 0;
  while(1) 
    {
      swiWaitForVBlank();
      scanKeys();    

      int pressed = keysDown();
      if (pressed & KEY_TOUCH)
        {
          //touchPosition touch_down = touchReadXY();
          count += 1;

          if (count > 0)
            break;
        }
    }
}

void gamescreen()
{
  // set the mode for 2 text layers and two extended background layers
  videoSetMode(MODE_5_2D |
               //DISPLAY_BG1_ACTIVE |
               DISPLAY_BG3_ACTIVE |
               DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT
               );

  // set the sub background up for text display (we could just print to one
  // of the main display text backgrounds just as easily
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_BG2_ACTIVE);

  // set the first bank as background memory and the third as sub background memory
  // B and D are not used (if you want a bitmap greater than 256x256 you will need more
  // memory so another vram bank must be used and mapped consecutivly
  vramSetMainBanks(VRAM_A_MAIN_BG_0x6000000, VRAM_B_MAIN_SPRITE, //VRAM_B_MAIN_BG_0x6020000,
                   VRAM_C_SUB_BG, VRAM_D_MAIN_BG_0x6020000);
  
  // set up our bitmap background
  BG3_CR     = BG_BMP8_512x256 | BG_WRAP_ON| BG_BMP_BASE(0);
  //BG2_CR     = BG_BMP8_256x256 | BG_BMP_BASE(8);
  
  BG1_CR = BG_32x32 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_256_COLOR;

  SUB_BG2_CR = BG_BMP8_256x256 | BG_WRAP_ON | BG_BMP_BASE(4);
  SUB_BG3_CR = BG_BMP8_256x256 | BG_WRAP_ON;

  // these are rotation backgrounds so you must set the rotation attributes:
  // these are fixed point numbers with the low 8 bits the fractional part
  // this basicaly gives it a 1:1 translation in x and y so you get a nice flat bitmap
  BG3_XDX = 1 << 8;
  BG3_XDY = 0;
  BG3_YDX = 0;
  BG3_YDY = 1 << 8;

  BG2_XDX = 1 << 8;
  BG2_XDY = 0;
  BG2_YDX = 0;
  BG2_YDY = 1 << 8;

  SUB_BG3_XDX = 1 << 8;
  SUB_BG3_XDY = 0;
  SUB_BG3_YDX = 0;
  SUB_BG3_YDY = 1 << 8;

  SUB_BG2_XDX = 1 << 8;
  SUB_BG2_XDY = 0;
  SUB_BG2_YDX = 0;
  SUB_BG2_YDY = 1 << 8;

  //our bitmap looks a bit better if we center it so scroll down (256 - 192) / 2
  BG3_CX = 0;
  BG3_CY = 0;

  BG2_CX = 0;
  BG2_CY = 0;

  SUB_BG3_CX = 0;
  SUB_BG3_CY = 0; //32 << 8;

  SUB_BG2_CX = 0;//2000;
  SUB_BG2_CY = 0;//2000; //32 << 8;

  for(int i = 0; i < 512*256; i++)
    BG_GFX[i] = ((u16*)topscreen_img_bin)[i];

  for(int i = 0; i < 256; ++i)
    BG_PALETTE[i] = ((u16*)topscreen_pal_bin)[i];
  //for(int i = 0; i < 256; ++i)
  //BG_PALETTE[i] = ((u16*)numbers_pal_bin)[i];

  for(int i = 0; i < 256; ++i)
    BG_PALETTE_SUB[i] = ((u16*)subscreen_pal_bin)[i];

  for(int i = 0; i < 256*192/2; ++i)
    ((u16*)BG_BMP_RAM_SUB(4))[i] = ((u16*)health_img_bin)[i];

  for(int i = 0; i < 256*256/2; ++i)
    ((u16*)BG_BMP_RAM_SUB(0))[i] = ((u16*)realmap_img_bin)[i];

  TransferSoundData blaster = {
    blaster_bin,            /* Sample address */
    blaster_bin_size,       /* Sample length */
    11025,                  /* Sample rate */
    127,                    /* Volume */
    64,                     /* panning */
    1                       /* format */
  };
        
  int x = 0;
  int y = 0;
  int px = 128-32;
  int py = 103;
  int oldx = x;
  int oldy = y;
  bool drag = false;
  touchPosition touch_down = touchReadXY();

  initSprites();
  
  for(int i=0;i<256*256;i++)
    {
      SPRITE_GFX[i] = i % 256 | ((i % 256) << 8);
    }

  //for(int i = 0; i < 8*8*256/2; ++i)
  //    ((u16*)BG_TILE_RAM(0))[i] = ((u16*)numbers_img_bin)[i];

  //for(int i = 0; i < 32*32; ++i)
  //((u16*)BG_MAP_RAM(8))[i] = i%256;

  /*    for(int y=0;y<64; ++y)
        for(int x=0;x<32; ++x)
        {
        SPRITE_GFX[y*64+x] = ((u16*)sprite_img_bin)[y*32+x];
        }*/

  const u8* frames[16];
  frames[0]  = walk_frame00_img_bin;
  frames[1]  = walk_frame01_img_bin;
  frames[2]  = walk_frame02_img_bin;
  frames[3]  = walk_frame03_img_bin;
  frames[4]  = walk_frame04_img_bin;
  frames[5]  = walk_frame05_img_bin;
  frames[6]  = walk_frame06_img_bin;
  frames[7]  = walk_frame07_img_bin;
  frames[8]  = walk_frame08_img_bin;
  frames[9]  = walk_frame09_img_bin;
  frames[10] = walk_frame10_img_bin;
  frames[11] = walk_frame11_img_bin;
  frames[12] = walk_frame12_img_bin;
  frames[13] = walk_frame13_img_bin;
  frames[14] = walk_frame14_img_bin;
  frames[15] = walk_frame15_img_bin;

  const u8* pal_frames[16];
  pal_frames[0]  = walk_frame00_pal_bin;
  pal_frames[1]  = walk_frame01_pal_bin;
  pal_frames[2]  = walk_frame02_pal_bin;
  pal_frames[3]  = walk_frame03_pal_bin;
  pal_frames[4]  = walk_frame04_pal_bin;
  pal_frames[5]  = walk_frame05_pal_bin;
  pal_frames[6]  = walk_frame06_pal_bin;
  pal_frames[7]  = walk_frame07_pal_bin;
  pal_frames[8]  = walk_frame08_pal_bin;
  pal_frames[9]  = walk_frame09_pal_bin;
  pal_frames[10] = walk_frame10_pal_bin;
  pal_frames[11] = walk_frame11_pal_bin;
  pal_frames[12] = walk_frame12_pal_bin;
  pal_frames[13] = walk_frame13_pal_bin;
  pal_frames[14] = walk_frame14_pal_bin;
  pal_frames[15] = walk_frame15_pal_bin;

  for(int i=0;i<64*32; ++i)
    SPRITE_GFX[i] = ((u16*)sprite_img_bin)[i];

  for(int i=0;i<64*32; ++i)
    SPRITE_GFX[i] = ((u16*)walk_frame00_img_bin)[i];

  for(int i = 0; i < 256; ++i)
    SPRITE_PALETTE[i] = ((u16*)walk_frame00_pal_bin)[i];


  sprites[0].attribute[0] = ATTR0_SQUARE | ATTR0_COLOR_256 | px;
  sprites[0].attribute[1] = ATTR1_SIZE_64 | py  | ATTR1_FLIP_Y;
  sprites[0].attribute[2] = 0;

  int frame_index = 0;
  int sx = 0;
  //int sy = 0;
  bool allow_scrolling_x = true;
  bool allow_scrolling_y = true;
  bool left = true;;
  while(1) 
    {
      swiWaitForVBlank();

      frame_index += 1;
      frame_index = frame_index % 80;

      //SPRITE_GFX[i] = ((u16*)numbers_img_bin)[i];


      // read the button states
      scanKeys();
      touchPosition touch = touchReadXY();

      int pressed = keysDown();	// buttons pressed this loop
      int held = keysHeld();		// buttons currently held

      
      sprites[0].attribute[0] = ATTR0_SQUARE | ATTR0_COLOR_256 | mod(103,512);
      sprites[0].attribute[1] = ATTR1_SIZE_64 | mod(128-32,512) | (left ? 0: ATTR1_FLIP_X);
      sprites[0].attribute[2] = 0;

      if ((held & KEY_L) && (held & KEY_R))
        swiSoftReset();

      if ((pressed & KEY_LEFT) || (pressed & KEY_RIGHT))
        {
          frame_index = 0;
        }

      if (held & KEY_LEFT)
        {
          left = true;
          px -= 1;
          sprites[0].attribute[1] = sprites[0].attribute[1] & ~ATTR1_FLIP_X;

          for(int i=0;i<64*32; ++i)
            SPRITE_GFX[i] = ((u16*)frames[frame_index/5])[i];
      
          for(int i = 0; i < 256; ++i)
            SPRITE_PALETTE[i] = ((u16*)pal_frames[frame_index/5])[i];
        }
      else if (held & KEY_RIGHT)
        {
          left = false;
          px += 1;
          sprites[0].attribute[1] = sprites[0].attribute[1] | ATTR1_FLIP_X;

          for(int i=0;i<64*32; ++i)
            SPRITE_GFX[i] = ((u16*)frames[frame_index/5])[i];
      
          for(int i = 0; i < 256; ++i)
            SPRITE_PALETTE[i] = ((u16*)pal_frames[frame_index/5])[i];
        }
      else
        {
          for(int i=0;i<64*32; ++i)
            SPRITE_GFX[i] = ((u16*)frames[0])[i];
      
          for(int i = 0; i < 256; ++i)
            SPRITE_PALETTE[i] = ((u16*)pal_frames[0])[i];
        }

      if (held & KEY_DOWN)
        py += 1;
      else if (held & KEY_UP)
        py -= 1;

      sx = px;
      //sx += 1;

      if ((held & KEY_TOUCH)  && drag)
        {
          x = oldx + (touch_down.px - touch.px);
          y = oldy + (touch_down.py - touch.py);
        }

      if (!(held & KEY_TOUCH))
        drag = false;

      BG3_CX  = x*500 + sx<<8 ;
      BG3_CY  = y*500 + (32<<8);

      //2_CX  = -x;
      //2_CY  = -y;

      if (allow_scrolling_x)
        SUB_BG3_CX  = x<<8;
      else
        SUB_BG3_CX  = 0;

      if (allow_scrolling_y)
        SUB_BG3_CY  = y<<8;
      else
        SUB_BG3_CY  = 0;
      
      //SUB_BG2_CX  = -x;
      //SUB_BG2_CY  = -y;

      if (pressed & KEY_TOUCH)
        {
          const u8* picture = 0;
          const u8* subpicture = 0;
          
          if (touch.px < 48)
            {
              if (touch.py < 48)
                {
                  picture = map_img_bin;
                  subpicture = submap_img_bin;
                  allow_scrolling_x = true;
                  allow_scrolling_y = true;
                }
              else if (touch.py >= 48 && touch.py < 48*2)
                {
                  picture = health_img_bin;
                  subpicture = subhealth_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = false;
                }
              else if (touch.py >= 2*48 && touch.py < 48*3)
                {
                  picture = log_img_bin;
                  subpicture = sublog_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = true;
                }
              else if (touch.py >= 3*48 && touch.py < 48*4)
                {
                  picture = debug_img_bin;
                  subpicture = subdebug_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = false;
                }
            }
          else if (touch.px > 256 - 48)
            {
              if (touch.py < 48)
                {
                  picture = items_img_bin;
                  subpicture = subitems_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = false;
                }
              else if (touch.py >= 48 && touch.py < 48*2)
                {
                  picture = scan_img_bin;
                  subpicture = subscan_img_bin;
                  allow_scrolling_x = true;
                  allow_scrolling_y = true;
                }
              else if (touch.py >= 2*48 && touch.py < 48*3)
                {
                  picture = comm_img_bin;
                  subpicture = subcomm_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = false;
                }
              else if (touch.py >= 3*48 && touch.py < 48*4)
                {
                  picture = weapon_img_bin;
                  subpicture = subweapon_img_bin;
                  allow_scrolling_x = false;
                  allow_scrolling_y = false;
                }
            }

          if (picture)
            {
              for(int i = 0; i < 256*192/2; ++i)
                ((u16*)BG_BMP_RAM_SUB(4))[i] = ((u16*)picture)[i];
              
              playSound(&blaster);

              if (subpicture)
                {
                  for(int i = 0; i < 256*256/2; ++i)
                    ((u16*)BG_BMP_RAM_SUB(0))[i] = ((u16*)subpicture)[i];
                }
            }
          else 
            {
              // Didn't click on anything, so we assume middle region
              touch_down = touch;
              oldx = x;
              oldy = y;
              drag = true;
            }
        }
      updateOAM();
    }
}

int main(void)
{
  // irqs are nice
  irqInit();
  irqEnable(IRQ_VBLANK);

  titlescreen();
  gamescreen();

  return 0;
}
 
/* EOF */
