#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "context.h"

int main(int argc, char **argv){

  context ctx;
  create_context(&ctx);
  
  int x = 500;
  int y = 500;
  int vx = 1;
  int vy = 1;

  while(ctx.running){
    
    x += vx;
    y += vy;

    if(x < 0){
      x = 0;
      vx = -vx;
    }
    if(x + 30 >= ctx.xres){
      x = ctx.xres - 30 - 1;
      vx = -vx;
    }
    if(y < 0){
      y = 0;
      vy = -vy;
    }
    if(y + 30 >= ctx.yres){
      y = ctx.yres - 30 - 1;
      vy = -vy;
    }
    
    clear_context(&ctx);
    plot_pixel(&ctx, 0xffffffff, 1820, 980);
    fill_rect(&ctx, 0xffff0000, x, y, 30, 30);
    draw_horizontal_line(&ctx, 0xff00ff00, 700, 1000, 600);
    draw_line(&ctx, 0xffff0000, 70, 500, 700, 50);
    buffer_swap(&ctx);
  }


  // clean up
  destroy_context(&ctx);

  return 0;
}
