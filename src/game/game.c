
#include "../headers/ctx.h"

void Game_init(Game *g){

  g->running = 1;

  g->state = MAINMENU;
  Player_init(&g->entities.player);

  g->entities.shifting_triangles = (ShiftingTriangle*)malloc(sizeof(ShiftingTriangle) * 3);
  int xc[3];
  int yc[3];
  xc[0] = 20; xc[1] = 900; xc[2] = 300;
  yc[0] = 800; yc[1] = 700; yc[2] = 700;
  ShiftingTriangle_init(&g->entities.shifting_triangles[0], 0xff0000ff, xc, yc);
  xc[0] = 200; xc[1] = 450; xc[2] = 1500;
  yc[0] = 1000; yc[1] = 750; yc[2] = 100;
  ShiftingTriangle_init(&g->entities.shifting_triangles[1], 0xff0000ff, xc, yc);
  xc[0] = 1700; xc[1] = 550; xc[2] = 150;
  yc[0] = 890; yc[1] = 525; yc[2] = 150;
  ShiftingTriangle_init(&g->entities.shifting_triangles[2], 0xff0000ff, xc, yc);

  Text_init(&g->entities.press_enter_text, 64, 0);

  Inputs_init(&g->inputs, g);
}
void Game_process_input(Game *g){
  Inputs_process_mouse(&g->inputs, g);
  Inputs_process_keyboard(&g->inputs, g);
}
void Game_update(Game *g, Context *ctx){
  switch(g->state){
    case MAINMENU:
      ShiftingTriangle_update(ctx, &g->entities.shifting_triangles[0]);
      ShiftingTriangle_update(ctx, &g->entities.shifting_triangles[1]);
      ShiftingTriangle_update(ctx, &g->entities.shifting_triangles[2]);
      break;
    case LEVEL0:
      Player_update(ctx, &g->entities.player);
  }
}
void Game_render(Game *g, Context *ctx){
  clear_context(ctx);
  switch(g->state){
    case MAINMENU:
      fill_rect(ctx, 0xffdeb887, 0, 0, ctx->xres - 1, ctx->yres - 1);
      ShiftingTriangle_render(ctx, &g->entities.shifting_triangles[0]);
      ShiftingTriangle_render(ctx, &g->entities.shifting_triangles[1]);
      ShiftingTriangle_render(ctx, &g->entities.shifting_triangles[2]);
      Text_render(&g->entities.press_enter_text, ctx);
      break;
    case LEVEL0:
      fill_rect(ctx, 0xffdeb887, 0, 0, ctx->xres - 1, ctx->yres - 1);
      Player_render(ctx, &g->entities.player);
      break;
  }
  Mouse_render(&g->inputs.ms, ctx);
  blit(ctx);
}
void Game_deallocate(Game *g){
  Player_deallocate(&g->entities.player);
  Text_deallocate(&g->entities.press_enter_text);
  free(g->entities.shifting_triangles);
  close(g->inputs.kbfd);
}