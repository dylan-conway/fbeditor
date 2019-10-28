#ifndef FB_OBJECTS_H
#define FB_OBJECTS_H

#include "context.h"

typedef struct{
  int x, y, vx, vy;
  int width, height;
  uint32_t color;
} Box;
void Box_init(Box *b);
void Box_update(Context *ctx, Box *b);
void Box_render(Context *ctx, Box *b);

typedef struct{
  int x, y, vx, vy;
  int width, height;
  int health, level;
  uint32_t color;
} Player;
void Player_init(Player *p);
void Player_update(Context *ctx, Player *p);
void Player_render(Context *ctx, Player *p);

typedef struct{
  int x1, y1, x2, y2, x3, y3;
  int vx1, vy1, vx2, vy2, vx3, vy3;
  uint32_t color1, color2, color3;
} ShiftingTriangle;
void ShiftingTriangle_init(ShiftingTriangle *t);
void ShiftingTriangle_update(Context *ctx, ShiftingTriangle *t);
void ShiftingTriangle_render(Context *ctx, ShiftingTriangle *t);

#endif
