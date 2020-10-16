#ifndef __INPUT__H__
#define __INPUT__H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/input.h>

#include "ctx.h"
#include "utils.h"

struct Mouse {
    int x, y, w, h, fd;
    pthread_t thread_id;
    pthread_mutex_t mutex;
};

void mouse_init(struct Mouse* m, struct Context* ctx, int x, int y);

void mouse_cleanup(struct Mouse* m);

void mouse_render(struct Context* ctx, struct Mouse* m);

struct Keyboard {
    int fd;
    pthread_t thread_id;
    pthread_mutex_t mutex;
};

void keyboard_init(struct Keyboard* kb, struct Context* ctx);

void keyboard_cleanup(struct Keyboard* kb);

#endif