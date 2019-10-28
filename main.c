#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "context.h"
#include "fb_objects.h"

int main(int argc, char **argv){

  Context ctx;
  create_context(&ctx);

  struct timespec sleepy_time;
  sleepy_time.tv_nsec = 16.7 * 1000000;
  sleepy_time.tv_sec = 0;

  // initialize objects
  Box box;
  Box_init(&box);

  ShiftingTriangle triangle;
  ShiftingTriangle_init(&triangle);

  while(ctx.running){
  
    // update
    Box_update(&ctx, &box);
    ShiftingTriangle_update(&ctx, &triangle);
    
    clear_context(&ctx);
    plot_pixel(&ctx, 0xffffffff, 1820, 980);
    draw_line(&ctx, 0xffff0000, 70, 500, 700, 50);
    Box_render(&ctx, &box);
    ShiftingTriangle_render(&ctx, &triangle);
    draw_horizontal_line(&ctx, 0xff00ff00, 700, 1000, 600);
    blip(&ctx);

    nanosleep(&sleepy_time, NULL);
  }


  // clean up
  destroy_context(&ctx);

  return 0;
}
