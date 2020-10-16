
#include "ctx.h"

void context_init(struct Context* ctx, int mode){

    // Get current terminal name and set to desired mode.
    int status;
    char* current_tty;
    current_tty = ttyname(STDIN_FILENO);
    if(current_tty == NULL){
        fprintf(stderr, "Error: failed to get current tty name\n");
        exit(EXIT_FAILURE);
    }
    ttyfd = open(current_tty, O_RDWR);
    if(ttyfd == -1){
        fprintf(stderr, "Error: failed to open current tty\n");
        exit(EXIT_FAILURE);
    }
    status = ioctl(ttyfd, KDSETMODE, mode);
    if(status == -1){
        fprintf(stderr, "Error: failed to set mode to KD_GRAPHICS\n");
        exit(EXIT_FAILURE);
    }

    // Turn off terminal echo and store original terminal settings
    // for cleanup.
    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    // Gather necessary framebuffer attributes and create memory for
    // framebuffer and double buffer.
    int fd;
    int xres, yres;
    int buffer_size;
    unsigned int* f_buffer;
    unsigned int* d_buffer;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    fd = open("/dev/fb0", O_RDWR);
    if(fd == -1){
        fprintf(stderr, "Error: failed to open /dev/fb0\n");
        context_cleanup(ctx);
        exit(EXIT_FAILURE);
    }

    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    vinfo.grayscale = 0;
    vinfo.bits_per_pixel = 32;
    ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

    xres = vinfo.xres;
    yres = vinfo.yres;
    buffer_size = xres * yres * (32 / 8);

    f_buffer = mmap(
        NULL, buffer_size, PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd, (off_t)0
    );
    
    d_buffer = mmap(
        NULL, buffer_size, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, (off_t)0
    );

    // Fill the context struct.
    ctx->fd = fd;
    ctx->xres = xres;
    ctx->yres = yres;
    ctx->buffer_size = buffer_size;
    ctx->running = 1;
    ctx->bits_per_pixel = 32;
    ctx->bytes_per_pixel = 4;
    ctx->vinfo = vinfo;
    ctx->finfo = finfo;
    ctx->f_buffer = f_buffer;
    ctx->d_buffer = d_buffer;
    ctx->original_term_settings = old_term;
}

void context_cleanup(struct Context* ctx){
    
    // In the case that one or more threads have been successfully
    // created but the program exits early, set running to 0 to exit
    // input threads.
    ctx->running = 0;
    
    // Set mode back to text if it was not already. Unmap mapped memory
    // for both buffers and close the framebuffer pseudo file.
    int status;
    status = ioctl(ttyfd, KDSETMODE, KD_TEXT);
    status = munmap(ctx->f_buffer, ctx->buffer_size);
    if(status == -1){
        fprintf(stderr, "Error: failed to unmap framebuffer memory\n");
    }
    status = munmap(ctx->d_buffer, ctx->buffer_size);
    if(status == -1){
        fprintf(stderr, "Error: failed to unmap double buffer memory\n");
    }
    status = close(ctx->fd);
    if(status == -1){
        fprintf(stderr, "Error: failed to to close /dev/fb0\n");
    }

    // Revert terminal to original settings (turns echo back on).
    tcsetattr(STDIN_FILENO, TCSANOW, &ctx->original_term_settings);
}


void blit(struct Context* ctx){
    memcpy(ctx->f_buffer, ctx->d_buffer, ctx->buffer_size);
}

void clear_screen(struct Context* ctx, uint color){
    memset(ctx->d_buffer, color, ctx->buffer_size);
}

void plot_pixel(struct Context* ctx, int x, int y, uint color){
    int index = x + (y * ctx->xres);
    ctx->d_buffer[index] = color;
}

void fill_rect(struct Context* ctx, int x, int y, int w, int h, uint color){
    int i, index;
    for(i = 0; i < h; i ++){
        draw_horizontal_line(ctx, x, y + i, w, color);
    }
}

void draw_rect(struct Context* ctx, int x, int y, int w, int h, uint color){
    draw_horizontal_line(ctx, x, y, w, color);
    draw_vertical_line(ctx, x, y + 1, h - 2, color);
    draw_vertical_line(ctx, x + w - 1, y + 1, h - 2, color);
    draw_horizontal_line(ctx, x, y + h - 1, w, color);
}

void draw_horizontal_line(struct Context* ctx, int x, int y, int l, uint color){
    int i, index;
    for(i = 0; i < l; i ++){
        index = (x + i) + (y * ctx->xres);
        ctx->d_buffer[index] = color;
    }
}

void draw_vertical_line(struct Context* ctx, int x, int y, int l, uint color){
    int i, index;
    for(i = 0; i < l; i ++){
        index = x + ((y + i) * ctx->xres);
        ctx->d_buffer[index] = color;
    }
}
