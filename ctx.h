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
#include <png.h>

#include "utils.h"

#define uint64 unsigned long
#define uint unsigned int
#define uint16 unsigned short
#define uint8 unsigned char

// Contains framebuffer and terminal data.
struct Context {
    int fd, ttyfd, xres, yres;
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
void context_free(struct Context* ctx);

/**
 * Transfers contents of the double buffer to the front buffer. Used to
 * update the entire buffer at the end of a draw iteration.
 * @param ctx Contains framebuffer and terminal data.
 */
void blit(struct Context* ctx);

/**
 * Clears the entire double buffer to white. Currently does not support
 * any colors until another memset alternative is found.
 * @param ctx Contains framebuffer and terminal data.
 */
void clear_screen(struct Context* ctx);

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

/**
 * Draw a horizontal line on the screen in one color.
 * @param ctx Framebuffer data.
 * @param x Starting left of the line.
 * @param y y coord.
 * @param l The length of the line.
 * @param color Color of the line.
 */
void draw_horizontal_line(struct Context* ctx, int x, int y, int l, uint color);

/**
 * Draw a vertical line in one color.
 * @param ctx Framebuffer data.
 * @param x x coord.
 * @param y y coord.
 * @param l Length of the line.
 * @param color Color of the line.
 */
void draw_vertical_line(struct Context* ctx, int x, int y, int l, uint color);

// Used to store data of an image to be drawn
// in the framebuffer.
struct Image {
    uint* data;
    int w, h;
};

/**
 * Initializes an Image struct with the given filename.
 * @param img Pass by reference Image struct.
 * @param filename Path to png file.
 */
int image_init(struct Image* img, char* filename);

/**
 * Cleanup memory used by Image struct.
 * @param img Pass by reference Image struct.
 */
void image_free(struct Image* img);

/**
 * Draw an image on the framebuffer.
 * @param ctx Framebuffer data.
 * @param img Image to draw.
 * @param x Top left x coord.
 * @param y Top left y coord.
 */
void image_draw(struct Context* ctx, struct Image* img, int x, int y);

/**
 * Print image to stdout. Useful for checking if image is read correctly.
 * @param img Image to print.
 */
void image_print(struct Image* img);

struct DynamicSprite {
    struct Image img;
};

void dynamic_sprite_init(struct DynamicSprite* ds, char* filename);

void dynamic_sprite_free(struct DynamicSprite* ds);

void dynamic_sprite_draw(struct DynamicSprite* ds);

#endif