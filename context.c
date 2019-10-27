
#include "context.h"

void create_context(context *ctx){

  int fd, xres, yres;
  uint32_t *f_buffer, *d_buffer;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  int bpp = 32;

  fd = open("/dev/fb0", O_RDWR);
  if(fd == -1){
    perror("Failed to open /dev/fb0");
    exit(EXIT_FAILURE);
  }

  ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
  ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
  vinfo.grayscale = 0;
  vinfo.bits_per_pixel = bpp;
  ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo);
  ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

  xres = vinfo.xres;
  yres = vinfo.yres;
  bpp = vinfo.bits_per_pixel;

  if(vinfo.grayscale && vinfo.bits_per_pixel != 32){
    fprintf(stderr, "Failed to set grayscale or bits per pixel\n");
    exit(EXIT_FAILURE);
  }

  f_buffer = (uint32_t *)mmap(
    0, xres * yres * bpp / 8,
    PROT_READ | PROT_WRITE,
    MAP_SHARED, fd, (off_t)0
  );
  if(f_buffer == NULL){
    perror("Failed to create frame buffer data");
    exit(EXIT_FAILURE);
  }

  d_buffer = (uint32_t *)mmap(
    0, xres * yres * bpp / 8,
    PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS,
    -1, (off_t)0
  );
  if(d_buffer == NULL){
    perror("Failed to allocate double buffer");
    exit(EXIT_FAILURE);
  }

  ctx->fd = fd;
  ctx->xres = xres;
  ctx->yres = yres;
  ctx->bpp = bpp;
  ctx->finfo = finfo;
  ctx->vinfo = vinfo;
  ctx->f_buffer = f_buffer;
  ctx->d_buffer = d_buffer;
}

void buffer_swap(context *ctx){
  memcpy(ctx->f_buffer, ctx->d_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8));
}

void plot_pixel(context *ctx, uint32_t color, int x, int y){
  int index = x + (y * ctx->xres);
  *(ctx->d_buffer + index) = color;
}

void draw_horizontal_line(context *ctx, uint32_t color, int x1, int x2, int y){
  int index;
  for(int i = x1; i < x2; i ++){
    plot_pixel(ctx, color, i, y);
  }
}

void draw_vertical_line(context *ctx, uint32_t color, int y1, int y2, int x){
  int index;
  for(int j = y1; j < y2; j ++){
    plot_pixel(ctx, color, x, j);
  }
}

void fill_rect(context *ctx, uint32_t color, int x, int y, int width, int height){
  int index;
  for(int i = 0; i < width; i ++){
    index = (x + i) + (y * ctx->xres);
    *(ctx->d_buffer + index) = color;
  }
  int dest_index;
  int src_index = x + (y * ctx->xres);
  for(int j = 1; j < height; j ++){
    dest_index = x + ((y + j) * ctx->xres);
    memcpy(ctx->d_buffer + dest_index, ctx->d_buffer + src_index, width * (ctx->bpp / 8));
  }
}

void destroy_context(context *ctx){
  if(munmap(ctx->f_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8)) == -1){
    perror("Failed to unmap frame buffer memory");
  }
  if(munmap(ctx->d_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8)) == -1){
    perror("Failed to unmap double buffer memory");
  };
  if(close(ctx->fd) == -1){
    perror("Failed to close fb0");
  };
}
