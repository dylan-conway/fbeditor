#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

typedef struct {
  int xres;
  int yres;
  uint32_t *f_buffer;
  uint32_t *d_buffer;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  int fd;
  int bpp;
} context;

void create_context(context *ctx);
void buffer_swap(context *ctx);
void plot_pixel(context *ctx, uint32_t color, int x, int y);
void draw_horizontal_line(context *ctx, uint32_t color, int x1, int x2, int y);
void draw_vertical_line(context *ctx, uint32_t color, int y1, int y2, int x);
void draw_line(context *ctx, uint32_t color, int x1, int y1, int x2, int y2);
void fill_rect(context *ctx, uint32_t color, int x, int y, int width, int height);
void clear_context(context *ctx);
void destroy_context(context *ctx);
