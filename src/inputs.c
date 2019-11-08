
#include "ctx.h"

void Inputs_init(Inputs *inputs, Game *g){
  char *kb_file = find_kb_file();
  inputs->kbfd = open(kb_file, O_RDONLY|O_NONBLOCK);
  if(inputs->kbfd == -1){
    perror("Opening keyboard file failed");
    g->running = 0;
  }
  free(kb_file);
}
void Inputs_process_keyboard(Inputs *inputs, Game *g){
  if(read(inputs->kbfd, &inputs->kbie, sizeof(struct input_event)) != -1){
    if(inputs->kbie.type == EV_KEY){

      switch(inputs->kbie.code){

        case KEY_ESC:
          switch(inputs->kbie.value){
            case 0:
              /* key up */
              break;
            case 1:
              /* key down */
              g->running = 0;
              break;
            case 2:
              /* key repeat */
              break;
          }
          break;
        case KEY_ENTER:
          switch(inputs->kbie.value){
            case 0:
              /* key up */
              break;
            case 1:
              /* key down */
              g->state = LEVEL0;
              break;
            case 2:
              /* key repeat */
              break;
          }
      }
    }
  }
}

// input
    // if(read(kefd, &kie, sizeof(struct input_event)) != -1){
    //   if(kie.type == EV_KEY){
    //     switch(kie.code){
    //       case KEY_ESC:
    //         if(kie.value == 0){
    //           /* key up */
    //           ctx.running = 0;
    //         }else if(kie.value == 1){
    //           /* key down */
    //         }else if(kie.value == 2){
    //           /* key repeat */
    //         }
    //         break;
    //       case KEY_UP:
    //         if(kie.value == 0){
    //           p.up = 0;
    //         }else if(kie.value == 1){
    //           p.up = 1;
    //         }else if(kie.value == 2){
    //           p.up = 1;
    //         }
    //         break;
    //       case KEY_RIGHT:
    //         if(kie.value == 0){
    //           p.right = 0;
    //         }else if(kie.value == 1){
    //           p.right = 1;
    //         }else if(kie.value == 2){
    //           p.right = 1;
    //         }
    //         break;
    //       case KEY_DOWN:
    //         if(kie.value == 0){
    //           p.down = 0;
    //         }else if(kie.value == 1){
    //           p.down = 1;
    //         }else if(kie.value == 2){
    //           p.down = 1;
    //         }
    //         break;
    //       case KEY_LEFT:
    //         if(kie.value == 0){
    //           p.left = 0;
    //         }else if(kie.value == 1){
    //           p.left = 1;
    //         }else if(kie.value == 2){
    //           p.left = 1;
    //         }
    //         break;
    //       case KEY_SPACE:
    //         if(kie.value == 0){

    //         }else if(kie.value == 1){

    //         }else if(kie.value == 2){

    //         }
    //         break;
    //     }
    //   }
    // }
