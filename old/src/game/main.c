#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../headers/ctx.h"

int running = 1;

int main(int argc, char **argv){

  FILE *output_fp = fopen("output.txt", "w");

  context_setup();

  Context ctx;
  create_context(&ctx);

  // initialize game struct
  Game g;
  Game_init(&g);

  while(g.running){
    float ms_start, ms_finish;
    ms_start = clock() * 1000.0 / CLOCKS_PER_SEC;

    Game_process_input(&g);
    Game_update(&g, &ctx);
    Game_render(&g, &ctx);

    do{
      ms_finish = clock() * 1000.0 / CLOCKS_PER_SEC;
    }while(ms_finish - ms_start < 16.67);

    // fprintf(output_fp, "%f\n", ms_finish - ms_start);
  }

  // clean up
  Game_deallocate(&g);
  destroy_context(&ctx);
  fclose(output_fp);

  printf("exiting...\n");
  return 0;
}
