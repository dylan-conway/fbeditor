
#include "context.h"

int ttyfd = -1;

void sig_handler(int sig){
  if(sig == SIGINT){
    printf("\nExiting...\n");
    ioctl(ttyfd, KDSETMODE, KD_TEXT);
    exit(EXIT_SUCCESS);
  }
  if(sig == SIGSEGV){
    ioctl(ttyfd, KDSETMODE, KD_TEXT);
    fprintf(stderr, "SEG FAULT\n");
    exit(EXIT_FAILURE);
  }
}

void create_context(context *ctx){

  // catch signals
  if(signal(SIGINT, sig_handler) == SIG_ERR){
    perror("Can't catch SIGINT");
  }
  if(signal(SIGSEGV, sig_handler) == SIG_ERR){
    perror("Can't catch SIGSEGV");
  }

  // open the current tty and change to graphics mode
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
      // exit(EXIT_FAILURE);
    };
  }

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

void draw_line(context *ctx, uint32_t color, int x1, int y1, int x2, int y2){
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

void clear_context(context *ctx){
  memset(ctx->d_buffer, 0, ctx->xres * ctx->yres * (ctx->bpp / 8));
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
  if(ioctl(ttyfd, KDSETMODE, KD_TEXT) == -1){
    perror("Failed to set text mode");
  }
  close(ttyfd);
}
