#ifndef CTX_H
#define CTX_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define itoa(x) #x

typedef struct{
  int xres;
  int yres;
  uint32_t *f_buffer;
  uint32_t *d_buffer;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  int fd;
  int bpp;
  int running;
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
void draw_fb_img(Context *ctx, fb_img img, int x, int y);

char *find_kb_file();

typedef struct{
  fb_img img;
} StaticSprite;
void StaticSprite_init(StaticSprite *s, char *filename);
void StaticSprite_render(Context *ctx, StaticSprite *s, int x, int y);
void StaticSprite_deallocate(StaticSprite *s);

typedef struct{
  uint32_t *data;
} Sprite;
void Sprite_init(Sprite *s, char *filename);
void Sprite_deallocate(Sprite *s);

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
  int width, height;
  int health, level;
  int right, left;
  int speed;
  StaticSprite sprite;
} Player;
void Player_init(Player *p);
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

#endif
