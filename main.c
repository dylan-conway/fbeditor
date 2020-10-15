#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <linux/input.h>

#include "ctx.h"

enum COMMANDS {
    UP,
    RIGHT,
    DOWN,
    LEFT,
};

int mouse_x = 10;
int mouse_y = 10;
pthread_t mouse_thread_id;
pthread_mutex_t mouse_mutex;

struct Context ctx;

void* mouse_input(void* args);
char* find_event_file(char* ev);

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

    pthread_mutex_init(&mouse_mutex, NULL);
    pthread_create(&mouse_thread_id, NULL, mouse_input, NULL);

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGSTKFLT, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);

    context_init(&ctx, KD_GRAPHICS);

    char* keyboard_file_name = find_event_file("B: EV=120013");
    if(keyboard_file_name == NULL){
        context_cleanup(&ctx);
        fprintf(stderr, "Error: failed to get keyboard event file name\n");
        exit(EXIT_FAILURE);
    }
    int kbfd = open(keyboard_file_name, O_RDONLY | O_NONBLOCK);
    free(keyboard_file_name);
    if(kbfd == -1){
        context_cleanup(&ctx);
        fprintf(stderr, "Error: failed to open keyboard event file\n");
        exit(EXIT_FAILURE);
    }

    int running = 1;
    float ms_start, ms_finish;
    struct input_event kbie = {};

    while(ctx.running){

        ms_start = clock() * 1000.0 / CLOCKS_PER_SEC;

        read(kbfd, &kbie, sizeof(struct input_event));
        if(kbie.type == EV_KEY){
            switch(kbie.code){
                case KEY_ESC:{
                    switch(kbie.value){
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

        clear_screen(&ctx, 0x00000000);
        pthread_mutex_lock(&mouse_mutex);
        plot_pixel(&ctx, mouse_x, mouse_y, 0xffffffff);
        pthread_mutex_unlock(&mouse_mutex);
        blit(&ctx);

        do{
            ms_finish = clock() * 1000.0 / CLOCKS_PER_SEC;
        } while(ms_finish - ms_start < 16.67);
    }

    pthread_join(mouse_thread_id, NULL);
    context_cleanup(&ctx);
    return 0;
}

void* mouse_input(void* args){

    char* mouse_file_name = NULL;
    int fd = -1;
    struct input_event e = {};

    mouse_file_name = find_event_file("B: EV=17");
    if(mouse_file_name == NULL){
        context_cleanup(&ctx);
        fprintf(stderr, "Error: failed to get mouse event file name\n");
        exit(EXIT_FAILURE);
    }

    fd = open(mouse_file_name, O_RDONLY | O_NONBLOCK);
    if(fd == -1){
        context_cleanup(&ctx);
        fprintf(stderr, "Error: failed to open mouse event file\n");
        exit(EXIT_FAILURE);
    }

    while(ctx.running){
        read(fd, &e, sizeof(struct input_event));
        if(e.type == EV_REL){
            switch(e.code){
                case REL_X:{
                    pthread_mutex_lock(&mouse_mutex);
                    mouse_x += e.value;
                    pthread_mutex_unlock(&mouse_mutex);
                    break;
                }
                case REL_Y:{
                    pthread_mutex_lock(&mouse_mutex);
                    mouse_y += e.value;
                    pthread_mutex_unlock(&mouse_mutex);
                    break;
                }
            }
        }
    }

    free(mouse_file_name);
    close(fd);
    return NULL;
}

char* find_event_file(char* ev){

    FILE* fp = NULL;
    char* line = NULL;
    size_t size = 0;
    int event = 0;
    int length = 0;
    
    char* path = "/dev/input/event";
    char* filename;
    char* event_file_number;

    fp = fopen("/proc/bus/input/devices", "r");
    length = getline(&line, &size, fp);
    while(length != -1){
        if(strncmp(line, "B: EV=", 6) == 0){
            if(strncmp(line, ev, strlen(ev)) == 0){
                event_file_number = malloc(sizeof(char) * 10);
                sprintf(event_file_number, "%d", event);
                int size = sizeof(char) * (strlen(path) + strlen(event_file_number)) + 1;
                filename = malloc(size);
                strcpy(filename, path);
                strcat(filename, event_file_number);
                free(event_file_number);
                free(line);
                fclose(fp);
                return filename;
            }
            event++;
        }
        length = getline(&line, &size, fp);
    }

    free(line);
    fclose(fp);
    return NULL;
}
