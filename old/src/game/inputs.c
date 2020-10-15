
#include "../headers/ctx.h"

void Mouse_init(Mouse *m){
  m->x = 1000;
  m->y = 500;
}
void Mouse_render(Mouse *m, Context *ctx){
  fill_rect(ctx, 0x9932cc, m->x, m->y, 10, 10);
}

char *find_event_file(char *ev){
  FILE *fp = fopen("/proc/bus/input/devices", "r");
  char *line = NULL;
  size_t size = 0;
  int event = 0;

  char *path = "/dev/input/event";
  char *filename;
  char *c_event;

  int length = getline(&line, &size, fp);
  while(length != -1){
    if(strncmp(line, "B: EV=", 6) == 0){
      if(strncmp(line, ev, strlen(ev)) == 0){
        c_event = (char*)malloc(sizeof(char) * 10);
        sprintf(c_event, "%d", event);
        int size = sizeof(char) * (strlen(path) + strlen(c_event)) + 1;
        filename = (char*)malloc(size);
        strcpy(filename, path);
        strcat(filename, c_event);
        free(c_event);
        free(line);
        fclose(fp);
        return filename;
      }
      event++;
    }
    length = getline(&line, &size, fp);
  }
}

void Inputs_init(Inputs *inputs, Game *g){

  char *kb_file = find_event_file("B: EV=120013");
  inputs->kbfd = open(kb_file, O_RDONLY|O_NONBLOCK);
  if(inputs->kbfd == -1){
    perror("Opening keyboard file failed");
    g->running = 0;
  }
  free(kb_file);

  char *ms_file = find_event_file("B: EV=17");
  inputs->msfd = open(ms_file, O_RDONLY|O_NONBLOCK);
  if(inputs->msfd == -1){
    perror("Opening mouse file failed");
    g->running = 0;
  }
  free(ms_file);

  Mouse_init(&inputs->ms);
}

void Inputs_process_mouse(Inputs *inputs, Game *g){
  read(inputs->msfd, &inputs->msie, sizeof(struct input_event));
  if(inputs->msie.type == EV_REL){
    switch(inputs->msie.code){
      case REL_X:
        inputs->ms.x += inputs->msie.value;
        break;
      case REL_Y:
        inputs->ms.y += inputs->msie.value;
        break;
    }
  }
}

void Inputs_process_keyboard(Inputs *inputs, Game *g){
  if((read(inputs->kbfd, &inputs->kbie, sizeof(struct input_event))) != -1){
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
              break;
            case 1:
              g->state = LEVEL0;
              break;
            case 2:
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
