#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <dirent.h>

#include "src/ctx.h"

char *get_keyboard_file();

int main(int argc, char **argv){

  char *keyboard_file = get_keyboard_file();
  printf("%s\n", keyboard_file);
  free(keyboard_file);

  // context_setup();

  // Context ctx;
  // create_context(&ctx);

  // struct timespec sleepy_time;
  // sleepy_time.tv_nsec = 16.7 * 1000000;
  // sleepy_time.tv_sec = 0;

  // int kefd;
  // struct input_event kie;
  // kefd = open("/dev/input/event3", O_RDONLY | O_NONBLOCK);
  // if(kefd == -1){
  //   perror("Opening keyboard file failed");
  //   ctx.running = 0;
  // }

  // // initialize objects
  // Player p;
  // Player_init(&p);

  // Box box;
  // Box_init(&box);

  // ShiftingTriangle triangle;
  // ShiftingTriangle_init(&triangle);

  // while(ctx.running){
  //   // input
  //   if(read(kefd, &kie, sizeof(struct input_event)) != -1){
  //     if(kie.type == EV_KEY){
  //       switch(kie.code){
  //         case KEY_ESC:
  //           if(kie.value == 0){
  //             ctx.running = 0;
  //           }
  //           break;
  //       }
  //     }
  //   }
  
  //   // update
  //   Box_update(&ctx, &box);
  //   ShiftingTriangle_update(&ctx, &triangle);
    
  //   // render
  //   clear_context(&ctx);
  //   Box_render(&ctx, &box);
  //   ShiftingTriangle_render(&ctx, &triangle);
  //   Player_render(&ctx, &p);
  //   blit(&ctx);
    
  //   // sleep
  //   nanosleep(&sleepy_time, NULL);
  // }

  // // clean up
  // Player_deallocate(&p);
  // destroy_context(&ctx);

  return 0;
}

char *get_keyboard_file(){

  char *file_name;
  char *dir_name = "/dev/input/";

  DIR *dir = opendir(dir_name);
  struct dirent *dir_content = readdir(dir);
  while(dir_content){
    if(strcmp(dir_content->d_name, "event3") == 0){
      int length = strlen(dir_name) + strlen(dir_content->d_name) + 1;
      file_name = (char *)malloc(sizeof(char) * length);
      strcpy(file_name, dir_name);
      strcat(file_name, dir_content->d_name);
    }
    dir_content = readdir(dir);
  }

  free(dir_content);
  closedir(dir);

  return file_name;
}
