
#include "ctx.h"

void context_init(struct Context* ctx, int mode){

    // Get current terminal name and set to desired mode.
    int status;
    char* current_tty;
    int ttyfd;
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
    ctx->ttyfd = ttyfd;
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

    // Ensure the buffers are initialized before rgba calculations.
    clear_screen(ctx);
    blit(ctx);
}

void context_cleanup(struct Context* ctx){
    
    // In the case that one or more threads have been successfully
    // created but the program exits early, set running to 0 to exit
    // input threads.
    ctx->running = 0;
    
    // Set mode back to text if it was not already. Unmap mapped memory
    // for both buffers and close the framebuffer pseudo file.
    int status;
    status = ioctl(ctx->ttyfd, KDSETMODE, KD_TEXT);
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

void clear_screen(struct Context* ctx){
    memset(ctx->d_buffer, 0xffffffff, ctx->buffer_size);
}

void plot_pixel(struct Context* ctx, int x, int y, uint color){
    // If there is an alpha value other than 0, draw the color.
    // As of right now, a transparent pixel has alpha value of 0x00.
    if(color & 0xff000000){
        int index = x + (y * ctx->xres);
        ctx->d_buffer[index] = color;
    }
    // uint dst_color = ctx->d_buffer[index];
    // uint src_color = color;
    // uint8 dst_alpha = (0xff000000 & dst_color) >> 24;
    // uint8 src_alpha = (0xff000000 & src_color) >> 24;
    // dst_color = ctx->d_buffer[index] & (uint)0x00ffffff;
    // src_color = color & (uint)0x00ffffff;
    
    // uint8 out_alpha = src_alpha + dst_alpha * ((uint8)0xff - src_alpha);

    // if(src_alpha == 0x00000000){
    //     ctx->d_buffer[index] = (uint)0x00000000;
    // } else {
    //     uint blended_color = ((src_color * src_alpha) + (dst_color * dst_alpha * ((uint8)0xff - src_alpha))) / out_alpha;
    //     ctx->d_buffer[index] = blended_color;
    // }

}

void fill_rect(struct Context* ctx, int x, int y, int w, int h, uint color){
    int i;
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
        plot_pixel(ctx, x + i, y, color);
    }
}

void draw_vertical_line(struct Context* ctx, int x, int y, int l, uint color){
    int i, index;
    for(i = 0; i < l; i ++){
        plot_pixel(ctx, x, y + i, color);
    }
}

int image_init(struct Image* img, char* filename){

    png_image image;
    uint* img_data;
    png_bytep b;
    int i, k;

    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if(png_image_begin_read_from_file(&image, filename) == 0){
        return 1;
    }
    
    image.format = PNG_FORMAT_ARGB;
    b = malloc(PNG_IMAGE_SIZE(image));
    png_image_finish_read(&image, NULL, b, 0, NULL);

    img->w = image.width;
    img->h = image.height;

    img_data = malloc(sizeof(uint) * img->w * img->h);

    // Condense array of uint8 to array of uint32.
    for(k = 0; k < PNG_IMAGE_SIZE(image); k += 4){
        uint pixel = (b[k]<<24) | (b[k+1]<<16) | (b[k+2]<<8) | (b[k+3]);
        img_data[k / 4] = pixel;
    }

    img->data = img_data;

    png_image_free(&image);
    free(b);

    return 0;
}

void image_cleanup(struct Image* img){
    free(img->data);
}

void image_draw(struct Context* ctx, struct Image* img, int x, int y){

    // The image data and context buffer are a long array, not 2d, so
    // the indexneeds to be calculated for both.
    int i, j, img_index;
    for(i = 0; i < img->h; i ++){
        for(j = 0; j < img->w; j ++){
            img_index = j + (i * img->w);
            plot_pixel(ctx, x + j, y + i, img->data[img_index]);
        }
    }
}

void image_print(struct Image* img){
    printf("image print:\n");
    int i, j, index;
    for(i = 0; i < img->h; i ++){
        for(j = 0; j < img->w; j ++){
            index = j + (i * img->w);
            printf("%8x ", img->data[index]);
        }
        printf("\n");
    }
}
