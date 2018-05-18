/***********************************************************/
/* This source is part of SNEmulDS                         */
/* ------------------------------------------------------- */
/* (c) 1997-1999, 2006 archeide, All rights reserved.      */
/* Free for non-commercial use                             */
/***********************************************************/

#ifndef __gfx_h__
#define __gfx_h__

#include "common.h"

typedef
       struct {
              unsigned char   *ptr;
              unsigned char   pos_x;
              unsigned char   pf:2;
              unsigned char   first_x:3;
              unsigned char   trans:1;
       } line_sprite_Info;

typedef
       struct {
		   unsigned  pos_x:8;
		   unsigned  pos_y:8;
	      unsigned   fst_tile:9;
	      unsigned   palette:3;
	      unsigned   pf_priority:2;
	      unsigned   flip:2;
       } sprite_Info;

typedef
       unsigned char TTile[8][8];

typedef
       char TRLE_Tile[8][13];

typedef
       struct {
         unsigned char*  line[240];
       } _BITMAP;

typedef
       struct {
         uchar	b, g, r, t;
       } RealColor;

struct s_gfx
{
  int		ScreenWidth, ScreenHeight;

/* tiles */
//  uint8		*tiles_def[3];
//  TRLE_Tile	**tiles[3];

/* sprites*/
  sprite_Info	spr_info[128];
  uchar		spr_info_ext[32]; /* additional information */
  int		Sprites_table_dirty;
  uchar		spr_cnt[240];
  line_sprite_Info line_spr[240][128];
  int		spr_addr_select;
  int		spr_addr_base;

  int		tiles_dirty;

  uchar		display_frame;
  int		BG_scroll_reg;
  int		Graph_enabled;
  int		Blank_Screen;
  int		was_not_blanked;

  short         tiles_x[8][33];
  uchar		tiles_ry[4];
  uchar		tiles_cnt[8];
  char		HighestSprite;
  int		old_scrollx[4], old_scrolly[4];
  int		tile_address[4];
  int		old_tile_address[4];

  int		SC_incr;
  int		FS_incr;
  int		FS_cnt, FS_cnt2;

  int		OAM_upper_byte;
  int		Old_SpriteAddress;
  int		old_brightness;
  int		Dummy_VRAMRead;

  uchar		new_colors[256], new_color;
  uchar		need_update;
  int		pal;
  int		FIXED_notblack;
  uint16	CG_RAM_mem_temp;
  RealColor	RealPal[256], SNESPal[256];
  RealColor	FIXED, BACK;
  uint16	Palette[256];

  int		frame_d;
  int		nb_frames;
  int		real_nb_frames;

//  _BITMAP       buf_screen;
  uchar		subscreen[256];
  uchar		tmp_scr[16];    /* REMOVE ME */
  uint16	buf_scr_t[288];

  uint16	WIN_X1, WIN_X2, CWIN_X1, CWIN_X2;
  int           SubScreen, FIXED_color_addsub, SUBSCREEN_addsub;
//  uint16	AddSubTable[0x10000];
	
	  uint32	VRAM_ptr; // Start of BG Tiles mem, used by convert function

  uint32	YScroll; 


};

extern struct s_gfx	GFX;

void	init_GFX();
void	PPU_setScreen(int screen);
void	check_sprite_addr();
void	update_scrollx();
void	update_scrolly();
void	check_tile();
void    check_tile_addr();
void	PPU_setPalette(int c, int r, int g, int b);
int		get_joypad();
void	update_joypads();
void	PPU_reset();
void	draw_screen();



#endif