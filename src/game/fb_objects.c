
#include "../headers/ctx.h"

void Text_init(Text *t, int x, int y){
  StaticSprite_init(&t->sprite, "images/mainmenu-press-enter-to-start.bmp");
  t->xres = t->sprite.xres;
  t->yres = t->sprite.yres;
  t->x = x;
  t->y = y;
}
void Text_render(Text *t, Context *ctx){
  StaticSprite_render(ctx, &t->sprite, t->x, t->y);
}
void Text_deallocate(Text *t){
  StaticSprite_deallocate(&t->sprite);
}

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
  p->y = 64;
  p->vx = 0;
  p->vy = 0;
  p->health = 100;
  p->level = 1;
  p->up = 0;
  p->right = 0;
  p->down = 0;
  p->left = 0;
  p->speed = 3;
  p->diag_speed = round(sqrt((p->speed * p->speed) / 2));
  p->facing = SOUTH;
  Sprite_init(&p->sprite, "images/character_sprite_sheet.bmp", 32, 48);
  p->xres = p->sprite.xres;
  p->yres = p->sprite.yres;
}

void Player_update_speed(Player *p, int speed){
  p->speed = speed;
  p->diag_speed = round(sqrt((p->speed * p->speed) / 2));
}

void Player_update(Context *ctx, Player *p){
  if(p->up && p->right){
    p->facing = NORTHEAST;
    p->x += p->diag_speed;
    p->y -= p->diag_speed;
  }else if(p->right && p->down){
    p->facing = SOUTHEAST;
    p->x += p->diag_speed;
    p->y += p->diag_speed;
  }else if(p->down && p->left){
    p->facing = SOUTHWEST;
    p->x -= p->diag_speed;
    p->y += p->diag_speed;
  }else if(p->left && p->up){
    p->facing = NORTHWEST;
    p->x -= p->diag_speed;
    p->y -= p->diag_speed;
  }else if(p->up){
    p->facing = NORTH;
    p->y -= p->speed;
  }else if(p->right){
    p->facing = EAST;
    p->x += p->speed;
  }else if(p->down){
    p->facing = SOUTH;
    p->y += p->speed;
  }else if(p->left){
    p->facing = WEST;
    p->x -= p->speed;
  }
  
  if(p->x < 0){
    p->x = 0;
  }
  if(p->x + p->xres >= ctx->xres){
    p->x = ctx->xres - p->xres - 1;
  }
  if(p->y < 0){
    p->y = 0;
  }
  if(p->y + p->yres >= ctx->yres){
    p->y = ctx->yres - p->yres - 1;
  }
}
void Player_render(Context *ctx, Player *p){
  Sprite_render(ctx, &p->sprite, p->x, p->y, p->facing);
}
void Player_deallocate(Player *p){
  Sprite_deallocate(&p->sprite);
}

void ShiftingTriangle_init(ShiftingTriangle *t, uint32_t color, int *x, int *y){
  t->x1 = x[0];
  t->y1 = y[0];
  t->x2 = x[1];
  t->y2 = y[1];
  t->x3 = x[2];
  t->y3 = y[2];
  t->vx1 = -5;
  t->vy1 = 5;
  t->vx2 = 5;
  t->vy2 = 5;
  t->vx3 = 5;
  t->vy3 = -5;
  t->color1 = color;
  t->color2 = color;
  t->color3 = color;
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
