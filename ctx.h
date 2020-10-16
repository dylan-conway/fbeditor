#ifndef __CTX__H__
#define __CTX__H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>

#define uint64 unsigned long
#define uint unsigned int
#define uint16 unsigned short
#define uint8 unsigned char

// Contains framebuffer and terminal data.
struct Context {
    int fd, xres, yres;
    int buffer_size;
    int running;
    int bits_per_pixel;
    int bytes_per_pixel;
    unsigned int* f_buffer;
    unsigned int* d_buffer;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    struct termios original_term_settings;
};

// The file descriptor for the current tty. Needs to be static
// and global for use in signal handler function.
static int ttyfd;

/**
 * Prepare terminal for drawing and fill the context struct.
 * @param ctx Contains framebuffer and terminal data.
 * @param mode The mode for the terminal(KD_TEXT or KD_GRAPHICS)
 */
void context_init(struct Context* ctx, int mode);

/**
 * Reverts terminal to original state and unmaps mapped memory used
 * for the framebuffer.
 * @param ctx Contains framebuffer and terminal data.
 */
void context_cleanup(struct Context* ctx);

/**
 * Transfers contents of the double buffer to the front buffer. Used to
 * update the entire buffer at the end of a render iteration.
 * @param ctx Contains framebuffer and terminal data.
 */
void blit(struct Context* ctx);

/**
 * Clears the entire double buffer to one color.
 * @param ctx Contains framebuffer and terminal data.
 * @param color The color used to clear the screen.
 */
void clear_screen(struct Context* ctx, uint color);

/**
 * Colors a single pixel.
 * @param ctx Contains framebuffer and terminal data.
 * @param x x coord.
 * @param y y coord.
 * @param color The color for the pixel.
 */
void plot_pixel(struct Context* ctx, int x, int y, uint color);

/**
 * Fills a rectangle with one color.
 * @param ctx Contains framebuffer and terminal data.
 * @param x Top left corner x coord.
 * @param y Top left corner y coord.
 * @param w Width of rect.
 * @param h Height of rect.
 * @param color The color to fill with.
 */
void fill_rect(struct Context* ctx, int x, int y, int w, int h, uint color);

/**
 * Draw the outline of a rectangle one pixel thick.
 * @param ctx Contains framebuffer and terminal data.
 * @param x Top left corner x coord.
 * @param y Top left corner y coord.
 * @param w Width of rect.
 * @param h Height of rect.
 * @param color The color to draw with.
 */
void draw_rect(struct Context* ctx, int x, int y, int w, int h, uint color);

void draw_horizontal_line(struct Context* ctx, int x, int y, int l, uint color);

void draw_vertical_line(struct Context* ctx, int x, int y, int l, uint color);

#endif