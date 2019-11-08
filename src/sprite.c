
#include "ctx.h"

void StaticSprite_init(StaticSprite *s, char *filename){
  read_bmp(&s->img, filename);
  s->xres = s->img.xres;
  s->yres = s->img.yres;
}

void StaticSprite_render(Context *ctx, StaticSprite *s, int x, int y){
  draw_fb_img(ctx, s->img, 0, 0, s->xres, s->yres, x, y);
}

void StaticSprite_deallocate(StaticSprite *s){
  free(s->img.data);
}

void Sprite_init(Sprite *s, char *filename, int xres, int yres){
  s->rate = 10;
  s->draw_counter = 0;
  s->xres = xres;
  s->yres = yres;
  s->sx = 0;
  s->sy = 0;
  read_bmp(&s->img, filename);
}

void Sprite_render(Context *ctx, Sprite *s, int x, int y, faces direction){
  if(s->draw_counter == s->rate){
    s->draw_counter = 0;
    if(s->sx == s->img.xres - s->xres){
      s->sx = 0;
    }else{
      s->sx += s->xres;
    }
  }
  draw_fb_img(ctx, s->img, s->sx, s->sy + s->yres * direction, s->xres, s->yres, x, y);

  s->draw_counter++;
}

void Sprite_deallocate(Sprite *s){
  free(s->img.data);
}
