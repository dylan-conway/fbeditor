#ifndef CTX_H
#define CTX_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define itoa(x) #x

typedef enum{
  MAINMENU,
  LEVEL0,
} game_state;

typedef enum{
  NORTH,
  EAST,
  SOUTH,
  WEST,
  NORTHEAST,
  SOUTHEAST,
  SOUTHWEST,
  NORTHWEST,
} faces;

typedef struct{
  int xres;
  int yres;
  uint32_t *f_buffer;
  uint32_t *d_buffer;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  int fd;
  int bpp;
} Context;

typedef struct{
  int xres, yres;
  int bpp, size;
  uint32_t *data;
} fb_img;

void context_setup();
void create_context(Context *ctx);
void blit(Context *ctx);
void plot_pixel(Context *ctx, uint32_t color, int x, int y);
void draw_horizontal_line(Context *ctx, uint32_t color, int x1, int x2, int y);
void draw_vertical_line(Context *ctx, uint32_t color, int y1, int y2, int x);
void draw_line(Context *ctx, uint32_t color, int x1, int y1, int x2, int y2);
void draw_circle(Context *ctx, uint32_t color, int cx, int cy, int radius);
void fill_rect(Context *ctx, uint32_t color, int x, int y, int width, int height);
void clear_context(Context *ctx);
void destroy_context(Context *ctx);
void read_bmp(fb_img *img, char *filename);
void draw_fb_img(Context *ctx, fb_img img, int sx, int sy, int xres, int yres, int dx, int dy);

char *find_kb_file();

typedef struct{
  int xres, yres;
  fb_img img;
} StaticSprite;
void StaticSprite_init(StaticSprite *s, char *filename);
void StaticSprite_render(Context *ctx, StaticSprite *s, int x, int y);
void StaticSprite_deallocate(StaticSprite *s);

typedef struct{
  int rate, draw_counter;
  int xres, yres;
  int sx, sy;
  fb_img img;
} Sprite;
void Sprite_init(Sprite *s, char *filename, int xres, int yress);
void Sprite_render(Context *ctx, Sprite *s, int x, int y, faces direction);
void Sprite_deallocate(Sprite *s);

typedef struct{
  int x, y;
  int xres, yres;
  StaticSprite sprite;
} Text;
void Text_init(Text *t, int x, int y);
void Text_render(Text *t, Context *ctx);
void Text_deallocate(Text *t);

typedef struct{
  int x, y, vx, vy;
  int width, height;
  uint32_t color;
} Box;
void Box_init(Box *b);
void Box_update(Context *ctx, Box *b);
void Box_render(Context *ctx, Box *b);

typedef struct{
  int x, y, vx, vy;
  int xres, yres;
  int health, level;
  int up, right, down, left;
  int speed, diag_speed;
  faces facing;
  Sprite sprite;
} Player;
void Player_init(Player *p);
void Player_update_speed(Player *p, int speed);
void Player_update(Context *ctx, Player *p);
void Player_render(Context *ctx, Player *p);
void Player_deallocate(Player *p);

typedef struct{
  int x1, y1, x2, y2, x3, y3;
  int vx1, vy1, vx2, vy2, vx3, vy3;
  uint32_t color1, color2, color3;
} ShiftingTriangle;
void ShiftingTriangle_init(ShiftingTriangle *t, uint32_t color, int *x, int *y);
void ShiftingTriangle_update(Context *ctx, ShiftingTriangle *t);
void ShiftingTriangle_render(Context *ctx, ShiftingTriangle *t);

typedef struct{
  Player player;
  ShiftingTriangle *shifting_triangles;
  Text press_enter_text;
} Entities;

typedef struct{
  int kbfd;
  struct input_event kbie;
} Inputs;

typedef struct{
  int running;
  game_state state;
  Entities entities;
  Inputs inputs;
} Game;

void Inputs_init(Inputs *inputs, Game *g);
void Inputs_process_keyboard(Inputs *inputs, Game *g);

void Game_init(Game *g);
void Game_process_input(Game *g);
void Game_update(Game *g, Context *ctx);
void Game_render(Game *g, Context *ctx);
void Game_deallocate(Game *g);

#endif
