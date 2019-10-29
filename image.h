#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

#include "context.h"

struct fb_img{
  int xres, yres;
  uint32_t *data;
};

typedef struct{
  struct fb_img img;
} StaticSprite;
void StaticSprite_init(StaticSprite *s, char *filename);
// void StaticSprite_render(Context *ctx, StaticSprite *s, int x, int y);
void StaticSprite_deallocate(StaticSprite *s);

typedef struct{
  uint32_t *data;
} Sprite;
void Sprite_init(Sprite *s, char *filename);
void Sprite_deallocate(Sprite *s);

#endif
