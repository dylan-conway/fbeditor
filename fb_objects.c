
#include "fb_objects.h"

void Box_init(Box *b){
  b->x = 500;
  b->y = 500;
  b->vx = 5;
  b->vy = 5;
  b->width = 55;
  b->height = 7;
  b->color = 0xff0000ff;
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

void ShiftingTriangle_init(ShiftingTriangle *t){
  t->x1 = 50;
  t->y1 = 50;
  t->x2 = 150;
  t->y2 = 600;
  t->x3 = 700;
  t->y3 = 1000;
  t->vx1 = -3;
  t->vy1 = 3;
  t->vx2 = 2;
  t->vy2 = 2;
  t->vx3 = 1;
  t->vy3 = -1;
  t->color1 = 0xffff0000;
  t->color2 = 0xff00ff00;
  t->color3 = 0xff0000ff;
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