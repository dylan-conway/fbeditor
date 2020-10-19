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
    int left_button_down;
    struct Image img;
    pthread_t thread_id;
    pthread_mutex_t mutex;
};

/**
 * Initialize Mouse struct. Needs framebuffer context for exiting on error.
 * @param m Mouse struct to initialize.
 * @param ctx Already initialized framebuffer data.
 * @param x Starting x coord.
 * @param y Starting y coord.
 */
void mouse_init(struct Mouse* m, struct Context* ctx, int x, int y);

/**
 * Deallocate any memory in an initialized Mouse struct.
 * @param m Mouse to deallocate.
 */
void mouse_free(struct Mouse* m);

/**
 * Draw Mouse struct to framebuffer.
 * @param ctx Framebuffer data.
 * @param m Mouse to draw.
 */
void mouse_draw(struct Context* ctx, struct Mouse* m);

struct Keyboard {
    int fd;
    pthread_t thread_id;
    pthread_mutex_t mutex;
};

void keyboard_init(struct Keyboard* kb, struct Context* ctx);

void keyboard_free(struct Keyboard* kb);

#endif