#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <dirent.h>

#include "src/ctx.h"

int running = 1;

int main(int argc, char **argv){

  FILE *output_fp = fopen("output.txt", "w");

  context_setup();

  Context ctx;
  create_context(&ctx);

  char *kb_file = find_kb_file();
  int kefd;
  struct input_event kie;
  kefd = open(kb_file, O_RDONLY | O_NONBLOCK);
  if(kefd == -1){
    perror("Opening keyboard file failed");
    ctx.running = 0;
  }

  // initialize objects
  Player p;
  Player_init(&p);

  int xc[3];
  int yc[3];

  ShiftingTriangle triangle1;
  xc[0] = 20; xc[1] = 900; xc[2] = 300;
  yc[0] = 800; yc[1] = 700; yc[2] = 700;
  ShiftingTriangle_init(&triangle1, 0xff0000ff, xc, yc);
  ShiftingTriangle triangle2;
  xc[0] = 200; xc[1] = 450; xc[2] = 1500;
  yc[0] = 1000; yc[1] = 750; yc[2] = 100;
  ShiftingTriangle_init(&triangle2, 0xff0000ff, xc, yc);
  ShiftingTriangle triangle3;
  xc[0] = 1700; xc[1] = 550; xc[2] = 150;
  yc[0] = 890; yc[1] = 525; yc[2] = 150;
  ShiftingTriangle_init(&triangle3, 0xff0000ff, xc, yc);

  while(ctx.running){
    float ms_start, ms_finish;
    ms_start = clock() * 1000.0 / CLOCKS_PER_SEC;

    // input
    if(read(kefd, &kie, sizeof(struct input_event)) != -1){
      if(kie.type == EV_KEY){
        switch(kie.code){
          case KEY_ESC:
            if(kie.value == 0){
              /* key up */
              ctx.running = 0;
            }else if(kie.value == 1){
              /* key down */
            }else if(kie.value == 2){
              /* key repeat */
            }
            break;
          case KEY_UP:
            if(kie.value == 0){
              p.up = 0;
            }else if(kie.value == 1){
              p.up = 1;
            }else if(kie.value == 2){
              p.up = 1;
            }
            break;
          case KEY_RIGHT:
            if(kie.value == 0){
              p.right = 0;
            }else if(kie.value == 1){
              p.right = 1;
            }else if(kie.value == 2){
              p.right = 1;
            }
            break;
          case KEY_DOWN:
            if(kie.value == 0){
              p.down = 0;
            }else if(kie.value == 1){
              p.down = 1;
            }else if(kie.value == 2){
              p.down = 1;
            }
            break;
          case KEY_LEFT:
            if(kie.value == 0){
              p.left = 0;
            }else if(kie.value == 1){
              p.left = 1;
            }else if(kie.value == 2){
              p.left = 1;
            }
            break;
          case KEY_SPACE:
            if(kie.value == 0){

            }else if(kie.value == 1){

            }else if(kie.value == 2){

            }
            break;
        }
      }
    }
      
    // update
    ShiftingTriangle_update(&ctx, &triangle1);
    ShiftingTriangle_update(&ctx, &triangle2);
    ShiftingTriangle_update(&ctx, &triangle3);
    Player_update(&ctx, &p);
    
    // render
    clear_context(&ctx);
    fill_rect(&ctx, 0xffdeb887, 0, 0, ctx.xres - 1, ctx.yres - 1);
    ShiftingTriangle_render(&ctx, &triangle1);
    ShiftingTriangle_render(&ctx, &triangle2);
    ShiftingTriangle_render(&ctx, &triangle3);
    Player_render(&ctx, &p);
    blit(&ctx);

    do{
      ms_finish = clock() * 1000.0 / CLOCKS_PER_SEC;
    }while(ms_finish - ms_start < 16.67);

    // fprintf(output_fp, "%f\n", ms_finish - ms_start);
  }

  // clean up
  Player_deallocate(&p);
  destroy_context(&ctx);
  fclose(output_fp);
  free(kb_file);
  close(kefd);

  printf("exiting...\n");
  return 0;
}
