#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

void drawSphere(double r, int lats, int longs);
void drawSky();
void drawFloor();
void drawBarn ();
void drawScarecrow ();
void drawFence();
void drawCornBox(int pos_x, int pos_y, int pos_z);
void drawCornGround(int pos_x, float pos_y, int pos_z);

void finalScreen (SDL_Window *win, TTF_Font *font, int scrH, int scrW, int f, int max);

void drawText(TTF_Font *font, Uint8 fgR, Uint8 fgG, Uint8 fgB, Uint8 fgA, Uint8 bgR, Uint8 bgG, Uint8 bgB, Uint8 bgA, char *text, int x, int y, bool background);
void drawMinimap(TTF_Font *font, int u);
void  SetCoordToPixel(int scrW, int scrH);

void drawFrameBar(float fps);
void drawForageBar(int forage, int MaxForage);
void drawTimeBar(int time, int MaxTime);
