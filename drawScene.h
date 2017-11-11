#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

void drawAxis();
void drawSphere(double r, int lats, int longs);
void drawSky();
void drawFloor();
void drawBarn ();
void drawScarecrow ();
void drawFence();
void drawCornBox(int pos_x, int pos_y, int pos_z);
void drawCornGround(int pos_x, float pos_y, int pos_z);

void finalScreen (SDL_Window *win, TTF_Font *font, int scrH, int scrW);

void drawText(TTF_Font *font, Uint8 R, Uint8 G, Uint8 B, char *text, int x, int y);
void drawMinimap(int scrH, int scrW);
void  SetCoordToPixel(int scrW, int scrH);

void drawFrameBar(float fps);
void drawForageBar(int forage, int MaxForage);
void drawTimeBar(int time, int MaxTime);
