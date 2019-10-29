
#include "fb_objects.h"

void Box_init(Box *b){
  b->x = 500;
  b->y = 500;
  b->vx = 1;
  b->vy = 1;
  b->width = 32;
  b->height = 32;
  b->color = 0x000000ff;
}
void Box_update(Context *ctx, Box *b){
  b->x += b->vx;
  b->y += b->vy;
  if(b->x < 0){
    b->x = 0;
    b->vx = -b->vx;
  }
  if(b->x + b->width >= ctx->xres){
    b->x = ctx->xres - b->width - 1;
    b->vx = -b->vx;
  }
  if(b->y < 0){
    b->y = 0;
    b->vy = -b->vy;
  }
  if(b->y + b->height >= ctx->yres){
    b->y = ctx->yres - b->height - 1;
    b->vy = -b->vy;
  }
}
void Box_render(Context *ctx, Box *b){
  fill_rect(ctx, b->color, b->x, b->y, b->width, b->height);
}

void Player_init(Player *p){
  p->x = 150;
  p->y = 1000;
  p->width = 32;
  p->height = 32;
  p->vx = 0;
  p->vy = 0;
  p->health = 100;
  p->level = 1;
  StaticSprite_init(&p->sprite, "images/character_sprite.bmp");
}
void Player_update(Context *ctx, Player *p);
void Player_render(Context *ctx, Player *p){
  // StaticSprite_render(ctx, &p->sprite, p->x, p->y);
  draw_fb_img(ctx, p->sprite.img, p->x, p->y);
}
void Player_deallocate(Player *p){
  StaticSprite_deallocate(&p->sprite);
}

void ShiftingTriangle_init(ShiftingTriangle *t){
  t->x1 = 50;
  t->y1 = 50;
  t->x2 = 150;
  t->y2 = 600;
  t->x3 = 700;
  t->y3 = 1000;
  t->vx1 = -10;
  t->vy1 = 10;
  t->vx2 = 10;
  t->vy2 = 10;
  t->vx3 = 10;
  t->vy3 = -10;
  t->color1 = 0xff00ff00;
  t->color2 = 0xff00ff00;
  t->color3 = 0xff00ff00;
}
void ShiftingTriangle_update(Context *ctx, ShiftingTriangle *t){
  t->x1 += t->vx1;
  t->y1 += t->vy1;
  t->x2 += t->vx2;
  t->y2 += t->vy2;
  t->x3 += t->vx3;
  t->y3 += t->vy3;

  if(t->x1 < 0){
    t->x1 = 0;
    t->vx1 = -t->vx1;
  }
  if(t->x2 < 0){
    t->x2 = 0;
    t->vx2 = -t->vx2;
  }
  if(t->x3 < 0){
    t->x3 = 0;
    t->vx3 = -t->vx3;
  }
  if(t->y1 < 0){
    t->y1 = 0;
    t->vy1 = -t->vy1;
  }
  if(t->y2 < 0){
    t->y2 = 0;
    t->vy2 = -t->vy2;
  }
  if(t->y3 < 0){
    t->y3 = 0;
    t->vy3 = -t->vy3;
  }
  if(t->x1 >= ctx->xres){
    t->x1 = ctx->xres - 1;
    t->vx1 = -t->vx1;
  }
  if(t->x2 >= ctx->xres){
    t->x2 = ctx->xres - 1;
    t->vx2 = -t->vx2;
  }
  if(t->x3 >= ctx->xres){
    t->x3 = ctx->xres - 1;
    t->vx3 = -t->vx3;
  }
  if(t->y1 >= ctx->yres){
    t->y1 = ctx->yres - 1;
    t->vy1 = -t->vy1;
  }
  if(t->y2 >= ctx->yres){
    t->y2 = ctx->yres - 1;
    t->vy2 = -t->vy2;
  }
  if(t->y3 >= ctx->yres){
    t->y3 = ctx->yres - 1;
    t->vy3 = -t->vy3;
  }
}
void ShiftingTriangle_render(Context *ctx, ShiftingTriangle *t){
  draw_line(ctx, t->color1, t->x1, t->y1, t->x2, t->y2);
  draw_line(ctx, t->color2, t->x2, t->y2, t->x3, t->y3);
  draw_line(ctx, t->color3, t->x3, t->y3, t->x1, t->y1);
}
