#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <linux/input.h>

#include "ctx.h"
#include "input.h"

struct Context ctx;
struct Mouse mouse;
struct Keyboard kb;

void* mouse_input(void* args);
void* keyboard_input(void* args);

void sig_handler(int sig){
    if(sig == SIGINT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGINT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    } else if(sig == SIGSEGV){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGSEGV\nexiting...\n\n");
        exit(EXIT_FAILURE);
    } else if(sig == SIGSTKFLT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGSTKFLT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    } else if(sig == SIGABRT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGABRT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv){

    if(getuid() != 0){
        fprintf(stderr, "Error: please run program as superuser\n");
        exit(EXIT_FAILURE);
    }

    // Set up signal handling for sudden stops of the
    // program. This is necessary because the terminal must
    // be set back to KD_TEXT mode if not already.
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGSTKFLT, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);


    // Initialize.
    context_init(&ctx, KD_GRAPHICS);
    mouse_init(&mouse, &ctx, 100, 100);
    keyboard_init(&kb, &ctx);


    // Add input threads.
    pthread_create(&mouse.thread_id, NULL, mouse_input, NULL);
    pthread_create(&kb.thread_id, NULL, keyboard_input, NULL);


    // Main loop. Update then render. Process input happens
    // in separate threads.
    float ms_start, ms_finish;
    while(ctx.running){
        ms_start = clock() * 1000.0 / CLOCKS_PER_SEC;

        // Update


        // Render
        clear_screen(&ctx, 0x00000000);
        mouse_render(&ctx, &mouse);
        blit(&ctx);


        do{
            ms_finish = clock() * 1000.0 / CLOCKS_PER_SEC;
        } while(ms_finish - ms_start < 16.67);
    }


    // Join input threads and cleanup context.
    pthread_join(mouse.thread_id, NULL);
    pthread_join(kb.thread_id, NULL);
    context_cleanup(&ctx);

    return 0;
}

// Mouse input thread. Mutex lock is needed for coordinates
// because values might change during a render function
// call resulting in misalignment.
void* mouse_input(void* args){

    struct input_event e = {};
    while(ctx.running){
        read(mouse.fd, &e, sizeof(struct input_event));
        if(e.type == EV_REL){
            switch(e.code){
                case REL_X:{
                    int new_x = mouse.x += e.value;
                    if(new_x < 0){
                        new_x = 0;
                    }
                    if(new_x + mouse.w > ctx.xres - 1){
                        new_x = ctx.xres - mouse.w - 1;
                    }
                    pthread_mutex_lock(&mouse.mutex);
                    mouse.x = new_x;
                    pthread_mutex_unlock(&mouse.mutex);
                    break;
                }
                case REL_Y:{
                    int new_y = mouse.y += e.value;
                    if(new_y < 0){
                        new_y = 0;
                    }
                    if(new_y + mouse.h > ctx.yres - 1){
                        new_y = ctx.yres - mouse.h - 1;
                    }
                    pthread_mutex_lock(&mouse.mutex);
                    mouse.y = new_y;
                    pthread_mutex_unlock(&mouse.mutex);
                    break;
                }
            }
        }
    }

    // Cleanup mouse struct before exit.
    mouse_cleanup(&mouse);
    return NULL;
}

// Keyboard input thread.
void* keyboard_input(void* args){

    struct input_event e = {};
    while(ctx.running){
        read(kb.fd, &e, sizeof(struct input_event));
        if(e.type == EV_KEY){
            switch(e.code){
                case KEY_ESC:{
                    switch(e.value){
                        case 0:{
                            // key up
                            break;
                        }
                        case 1:{
                            // key down
                            ctx.running = 0;
                            break;
                        }
                        case 2:{
                            // key repeat
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    // Cleanup keyboard struct before exit.
    keyboard_cleanup(&kb);
    return NULL;
}
