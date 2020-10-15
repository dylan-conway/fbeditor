#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/input.h>

#include "ctx.h"

char* find_event_file(char* ev);

struct Context ctx;

void sig_handler(int sig){
    if(sig == SIGINT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGINT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    }
    if(sig == SIGSEGV){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGSEGV\nexiting...\n\n");
        exit(EXIT_FAILURE);
    }
    if(sig == SIGSTKFLT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGSTKFLT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    }
    if(sig == SIGABRT){
        context_cleanup(&ctx);
        fprintf(stderr, "\nSIGABRT\nexiting...\n\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv){

    context_init(&ctx, KD_GRAPHICS);

    clear_screen(&ctx, 0xffdeb887);
    blit(&ctx);

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGSTKFLT, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);

    char* keyboard_file_name = find_event_file("B: EV=120013");
    if(keyboard_file_name == NULL){
        fprintf(stderr, "Error: failed to get keyboard event file name\n");
        exit(EXIT_FAILURE);
    }

    int kbfd = open(keyboard_file_name, O_RDONLY | O_NONBLOCK);
    free(keyboard_file_name);
    if(kbfd == -1){
        fprintf(stderr, "Error: failed to open keyboard file\n");
        exit(EXIT_FAILURE);
    }

    int running = 1;
    struct input_event kbie = {};

    while(running){
        read(kbfd, &kbie, sizeof(struct input_event));
        if(kbie.type == EV_KEY){
            switch(kbie.code){
                case KEY_ESC:{
                    switch(kbie.value){
                        case 0:{
                            // key up
                        }
                        case 1:{
                            // key down
                            running = 0;
                        }
                        case 2:{
                            // key repeat
                        }
                    }
                }

            }
        }
    }


    context_cleanup(&ctx);
    return 0;
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
