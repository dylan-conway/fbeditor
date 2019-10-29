#ifndef CONTEXT_H
#define CONTEXT_H

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

#endif
