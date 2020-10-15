#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>

#include "../headers/ctx.h"

int main(){

  int running = 1;

  int mfd = -1;
  int kfd = -1;
  struct input_event mie = {};
  struct input_event kie = {};
  char *mouse_file = find_event_file("B: EV=17");
  char *keyboard_file = find_event_file("B: EV=120013");

  mfd = open(mouse_file, O_RDONLY|O_NONBLOCK);
  if(mfd == -1){
    fprintf(stderr, "Error opening mouse file\n");
    running = 0;
  }

  kfd = open(keyboard_file, O_RDONLY|O_NONBLOCK);
  if(kfd == -1){
    fprintf(stderr, "Error opening keyboard file\n");
    running = 0;
  }

  free(mouse_file);
  free(keyboard_file);
  
  while(running){

    read(mfd, &mie, sizeof(struct input_event));
    if(mie.type == EV_REL){
      switch(mie.code){
        case REL_WHEEL:
          running = 0;
          break;
        case REL_X:
          printf("mouse x\n");
          break;
        case REL_Y:
          printf("mouse y\n");
          break;
      }
    }

    read(kfd, &kie, sizeof(struct input_event));
    if(kie.type == EV_KEY){
      switch(kie.code){
        case KEY_UP:
          printf("up\n");
          break;
        case KEY_RIGHT:
          printf("right\n");
          break;
        case KEY_DOWN:
          printf("down\n");
          break;
        case KEY_LEFT:
          printf("left\n");
          running = 0;
          break;
      }
    }
  }

  return 0;
}
