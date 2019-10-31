
#include "ctx.h"

void StaticSprite_init(StaticSprite *s, char *filename){

  int fd;
  fd = open(filename, O_RDONLY);
  if(fd == -1){
    perror("Error opening image file");
  }

  int xres, yres;
  lseek(fd, 18, SEEK_SET);
  read(fd, &xres, 4);
  read(fd, &yres, 4);

  int offset;
  lseek(fd, 10, SEEK_SET);
  read(fd, &offset, 4);

  s->img.data = (uint32_t *)malloc(sizeof(uint32_t) * xres * yres);
  lseek(fd, offset, SEEK_SET);
  int index = 0;
  while(index < xres * yres){
    read(fd, s->img.data + index, 4);
    *(s->img.data + index) = *(s->img.data + index) << 8;
    index++;
  }

  s->img.xres = xres;
  s->img.yres = yres;

  close(fd);
}

// void StaticSprite_render(Context *ctx, StaticSprite *s, int x, int y){
//   draw_fb_img(ctx, s->img, x, y);
// }

void StaticSprite_deallocate(StaticSprite *s){
  free(s->img.data);
}
