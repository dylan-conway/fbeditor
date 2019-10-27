#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "context.h"

int running = 1;
int ttyfd = -1;

void sig_handler(int sig){
  if(sig == SIGINT){
    running = 0;
  }
  if(sig == SIGSEGV){
    ioctl(ttyfd, KDSETMODE, KD_TEXT);
    fprintf(stderr, "SEG FAULT\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv){

  // catch signals
  if(signal(SIGINT, sig_handler) == SIG_ERR){
    perror("Can't catch SIGINT");
  }
  if(signal(SIGSEGV, sig_handler) == SIG_ERR){
    perror("Can't catch SIGSEGV");
  }

  // open the current tty and change to graphics mode
  char *current_tty = ttyname(STDIN_FILENO);
  if(current_tty == NULL){
    perror("Failed to get current tty");
    exit(EXIT_FAILURE);
  }
  ttyfd = open(current_tty, O_RDWR);
  if(ttyfd == -1){
    perror("Failed to open current tty");
    exit(EXIT_FAILURE);
  }else{
    if(ioctl(ttyfd, KDSETMODE, KD_GRAPHICS) == -1){
      perror("Failed to set graphics mode");
      // exit(EXIT_FAILURE);
    };
  }

  context ctx;
  create_context(&ctx);
  
  int x = 500;
  int y = 500;
  int vx = 2;
  int vy = 2;
  while(running){
    
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

    plot_pixel(&ctx, 0xffffffff, 1820, 980);
    fill_rect(&ctx, 0xffff0000, x, y, 30, 30);
    draw_horizontal_line(&ctx, 0xff00ff00, 700, 1000, 600);
    buffer_swap(&ctx);
  }


  // clean up
  destroy_context(&ctx);
  if(ioctl(ttyfd, KDSETMODE, KD_TEXT) == -1){
    perror("Failed to set text mode");
  }
  close(ttyfd);

  return 0;
}
