
#include "ctx.h"

int ttyfd = -1;

void sig_handler(int sig){
  if(sig == SIGINT){
    printf("\nDON'T EXIT WITH CTRL+C\n");
  }
  if(sig == SIGSEGV){
    ioctl(ttyfd, KDSETMODE, KD_TEXT);
    fprintf(stderr, "SEG FAULT\n");
    exit(EXIT_FAILURE);
  }
}

void context_setup(){
  if(signal(SIGINT, sig_handler) == SIG_ERR){
    perror("Cant't catch SIGINT");
  }
  if(signal(SIGSEGV, sig_handler) == SIG_ERR){
    perror("Can't catch SIGSEGV");
  }
  char *current_tty = ttyname(STDIN_FILENO);
  if(current_tty == NULL){
    perror("Failed to get current tty");
    exit(EXIT_FAILURE);
  }
  ttyfd = open(current_tty, O_RDWR);
  if(ttyfd == -1){
    perror("Failed to open current tty");
    exit(EXIT_FAILURE);
  }else{
    if(ioctl(ttyfd, KDSETMODE, KD_GRAPHICS) == -1){
      perror("Failed to set graphics mode");
      exit(EXIT_FAILURE);
    }
  }
}

void create_context(Context *ctx){

  int fd, xres, yres;
  uint32_t *f_buffer, *d_buffer;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  int bpp = 32;
  int running = 1;

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
  ctx->running = running;
}

void blit(Context *ctx){
  memcpy(ctx->f_buffer, ctx->d_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8));
}

void plot_pixel(Context *ctx, uint32_t color, int x, int y){
  int index = x + (y * ctx->xres);
  *(ctx->d_buffer + index) = color;
}

void draw_horizontal_line(Context *ctx, uint32_t color, int x1, int x2, int y){
  int index;
  for(int i = x1; i < x2; i ++){
    plot_pixel(ctx, color, i, y);
  }
}

void draw_vertical_line(Context *ctx, uint32_t color, int y1, int y2, int x){
  int index;
  for(int j = y1; j < y2; j ++){
    plot_pixel(ctx, color, x, j);
  }
}

void draw_line(Context *ctx, uint32_t color, int x1, int y1, int x2, int y2){
  int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;
  dx = x2 - x1;
  dy = y2 - y1;
  dxabs = abs(dx);
  dyabs = abs(dy);
  sdx = (dx > 0) ? 1 : -1;
  sdy = (dy > 0) ? 1 : -1;
  x = dyabs >> 1;
  y = dxabs >> 1;
  px = x1;
  py = y1;

  if(dxabs >= dyabs){
    for(i = 0; i < dxabs; i ++){
      y += dyabs;
      if(y >= dxabs){
        y -= dxabs;
        py += sdy;
      }
      px += sdx;
      plot_pixel(ctx, color, px, py);
    }
  }else{
    for(i = 0; i < dyabs; i ++){
      x += dxabs;
      if(x >= dyabs){
        x -= dyabs;
        px += sdx;
      }
      py += sdy;
      plot_pixel(ctx, color, px, py);
    }
  }
}

void draw_circle(Context *ctx, uint32_t color, int cx, int cy, int radius){
  // inline void plot4pixels(uint32_t color, int cx, int cy, int x, int y){
  //   plot_pixel(ctx, color, cx + x, cy + y);
  //   plot_pixel(ctx, color, cx - x, cy + y);
  //   plot_pixel(ctx, color, cx + x, cy - y);
  //   plot_pixel(ctx, color, cx - x, cy - y);
  // }
  // inline void plot8pixels(uint32_t color, int cx, int cy, int x, int y){ 
  //   plot4pixels(color, cx, cy, x, y);
  //   plot4pixels(color, cx, cy, y, x);
  // }

  // int error = -radius;
  // int x = radius;
  // int y = 0;

  // while(x >= y){
  //   plot8pixels(color, cx, cy, x, y);
  //   error += y;
  //   y++;
  //   error += y;
  //   if(error >= 0){
  //     error += -x;
  //     x--;
  //     error += -x;
  //   }
  // }
}

void draw_fb_img(Context *ctx, fb_img img, int x, int y){
  int src_index, dest_index;
  for(int j = 0; j < img.yres; j++){
    src_index = j * img.xres;
    dest_index = x + ((y + j) * ctx->xres);
    memcpy(ctx->d_buffer + dest_index, img.data + src_index, img.xres * (ctx->bpp / 8));
  }
}

void fill_rect(Context *ctx, uint32_t color, int x, int y, int width, int height){
  int index;
  for(int i = 0; i < width; i++){
    index = (x + i) + (y * ctx->xres);
    *(ctx->d_buffer + index) = color;
  }
  int dest_index;
  int src_index = x + (y * ctx->xres);
  for(int j = 1; j < height; j++){
    dest_index = x + ((y + j) * ctx->xres);
    memcpy(ctx->d_buffer + dest_index, ctx->d_buffer + src_index, width * (ctx->bpp / 8));
  }
}

void clear_context(Context *ctx){
  memset(ctx->d_buffer, 0xff000000, ctx->xres * ctx->yres * (ctx->bpp / 8));
}

void destroy_context(Context *ctx){
  if(munmap(ctx->f_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8)) == -1){
    perror("Failed to unmap frame buffer memory");
  }
  if(munmap(ctx->d_buffer, ctx->xres * ctx->yres * (ctx->bpp / 8)) == -1){
    perror("Failed to unmap double buffer memory");
  };
  if(close(ctx->fd) == -1){
    perror("Failed to close fb0");
  };
  if(ioctl(ttyfd, KDSETMODE, KD_TEXT) == -1){
    perror("Failed to set text mode");
  }
  close(ttyfd);
}
