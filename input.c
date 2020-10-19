
#include "input.h"

void mouse_init(struct Mouse* m, struct Context* ctx, int x, int y){

    pthread_mutex_init(&m->mutex, NULL);
    m->x = x;
    m->y = y;

    char* event_file = find_event_file("B: EV=17");
    if(event_file == NULL){
        context_free(ctx);
        fprintf(stderr, "Error: failed to get mouse event file name\n");
        exit(EXIT_FAILURE);
    }

    m->fd = open(event_file, O_RDONLY | O_NONBLOCK);
    free(event_file);
    if(m->fd == -1){
        context_free(ctx);
        fprintf(stderr, "Error: failed to open mouse event file\n");
        exit(EXIT_FAILURE);
    }

    if(image_init(&m->img, "images/cursor.png")){
        context_free(ctx);
        fprintf(stderr, "Error: failed to create cursor image\n");
        exit(EXIT_FAILURE);
    }

    m->w = m->img.w;
    m->h = m->img.h;
    m->left_button_down = 0;

    image_print(&m->img);
}

void mouse_free(struct Mouse* m){
    image_free(&m->img);
    close(m->fd);
    pthread_mutex_destroy(&m->mutex);
}

// 60 fps
void mouse_draw(struct Context* ctx, struct Mouse* m){
    pthread_mutex_lock(&m->mutex);
    int x = m->x;
    int y = m->y;
    image_draw(ctx, &m->img, x, y);
    pthread_mutex_unlock(&m->mutex);
}

void keyboard_init(struct Keyboard* kb, struct Context* ctx){

    pthread_mutex_init(&kb->mutex, NULL);

    char* event_file = find_event_file("B: EV=120013");
    if(event_file == NULL){
        context_free(ctx);
        fprintf(stderr, "Error: failed to get keyboard event file name\n");
        exit(EXIT_FAILURE);
    }

    kb->fd = open(event_file, O_RDONLY | O_NONBLOCK);
    free(event_file);
    if(kb->fd == -1){
        context_free(ctx);
        fprintf(stderr, "Error: failed to open keyboard event file\n");
        exit(EXIT_FAILURE);
    }
}

void keyboard_free(struct Keyboard* kb){
    close(kb->fd);
    pthread_mutex_destroy(&kb->mutex);
}