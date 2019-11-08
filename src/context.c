
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
  memset(ctx->d_buffer, 0x00000000, ctx->xres * ctx->yres * (ctx->bpp / 8));
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

void read_bmp(fb_img *img, char *filename){
  int fd = 0;
  fd = open(filename, O_RDONLY);
  if(fd == -1){
    perror("Error opening static sprite image");
  }
  
  int xres = 0;
  int yres = 0;
  lseek(fd, 18, SEEK_SET);
  read(fd, &xres, 4);
  read(fd, &yres, 4);

  int bpp = 0;
  lseek(fd, 28, SEEK_SET);
  read(fd, &bpp, 2);

  int offset = 0;
  lseek(fd, 10, SEEK_SET);
  read(fd, &offset, 4);

  int size = sizeof(uint32_t) * xres * yres;
  img->data = (uint32_t *)malloc(size);
  memset(img->data, 0x00000000, size);
  uint32_t *buffer = (uint32_t*)alloca(size);
  memset(buffer, 0x00000000, size);
  lseek(fd, offset, SEEK_SET);
  
  // read the contents of the file into a buffer
  int index = 0;
  while(index < xres * yres){
    read(fd, buffer + index, 3);
    index++;
  }

  // flip the contents of the buffer while copying them to the image data pointer
  index = 0;
  while(index < yres){
    memcpy(img->data + (((yres - 1) * xres) - (index * xres)), buffer + index * xres, xres * 4);
    index++;
  }

  printf("%dx%d, %dbpp\n", xres, yres, bpp);

  img->xres = xres;
  img->yres = yres;
  img->bpp = bpp;
  img->size = size;

  close(fd);
}

void draw_fb_img(Context *ctx, fb_img img, int sx, int sy, int xres, int yres, int dx, int dy){
  int src_index = 0;
  int dest_index = 0;
  int i = 0;
  int j = 0;
  
  for(i = 0; i < xres; i++){
    for(j = 0; j < yres; j++){
      src_index = (sx + i) + (sy + j) * img.xres;
      if(*(img.data + src_index) != 0x00000000){
        plot_pixel(ctx, *(img.data + src_index), dx + i, dy + j);
      }
    }
  }
}
