
#include "input.h"

void mouse_init(struct Mouse* m, struct Context* ctx, int x, int y){

    pthread_mutex_init(&m->mutex, NULL);
    m->x = x;
    m->y = y;
    m->w = 100;
    m->h = 100;

    char* event_file = find_event_file("B: EV=17");
    if(event_file == NULL){
        context_cleanup(ctx);
        fprintf(stderr, "Error: failed to get mouse event file name\n");
        exit(EXIT_FAILURE);
    }

    m->fd = open(event_file, O_RDONLY | O_NONBLOCK);
    free(event_file);
    if(m->fd == -1){
        context_cleanup(ctx);
        fprintf(stderr, "Error: failed to open mouse event file\n");
        exit(EXIT_FAILURE);
    }
}

void mouse_cleanup(struct Mouse* m){
    close(m->fd);
    pthread_mutex_destroy(&m->mutex);
}

// 60 fps
void mouse_render(struct Context* ctx, struct Mouse* m){
    pthread_mutex_lock(&m->mutex);
    int x = m->x;
    int y = m->y;
    int w = m->w;
    int h = m->h;
    fill_rect(ctx, x, y, w, h, 0xff0000);
    draw_rect(ctx, x, y, w, h, 0xffffff);
    pthread_mutex_unlock(&m->mutex);
}

void keyboard_init(struct Keyboard* kb, struct Context* ctx){

    pthread_mutex_init(&kb->mutex, NULL);

    char* event_file = find_event_file("B: EV=120013");
    if(event_file == NULL){
        context_cleanup(ctx);
        fprintf(stderr, "Error: failed to get keyboard event file name\n");
        exit(EXIT_FAILURE);
    }

    kb->fd = open(event_file, O_RDONLY | O_NONBLOCK);
    free(event_file);
    if(kb->fd == -1){
        context_cleanup(ctx);
        fprintf(stderr, "Error: failed to open keyboard event file\n");
        exit(EXIT_FAILURE);
    }
}

void keyboard_cleanup(struct Keyboard* kb){
    close(kb->fd);
    pthread_mutex_destroy(&kb->mutex);
}