// drawScene.cpp
// implementazione dei metodi definiti in drawScene.h

#include <stdio.h>
#include <math.h>

#include <vector>

#include "drawScene.h"
#include "point3.h"
#include "mesh.h"
#include "tractor.h"

extern Tractor tractor;
extern int corn[150][150];
extern int scrH, scrW;
extern bool useWireframe;

// mesh Barn
Mesh barn_house((char *)"object/barn_house.obj");
Mesh barn_frame((char *)"object/barn_frame.obj");
Mesh barn_roof((char *)"object/barn_roof.obj");
Mesh barn_vane((char *)"object/barn_vane.obj");
Mesh barn_window((char *)"object/barn_window.obj");

// mesh Fence
Mesh fence_horizontal((char *)"object/fence_horizontal.obj");
Mesh fence_vertical((char *)"object/fence_vertical.obj");

// mesh scarecrow
Mesh scarecrow_base((char *)"object/scarecrow_base.obj");
Mesh scarecrow_dress((char *)"object/scarecrow_dress.obj");
Mesh scarecrow_hands((char *)"object/scarecrow_hands.obj");
Mesh scarecrow_hat((char *)"object/scarecrow_hat.obj");

void drawSphere(double r, int lats, int longs) {
    int i, j;
    glPushMatrix();
    for(i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            //le normali servono per l'EnvMap
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
    glPopMatrix();
}

void drawSky() {
    int H = 100;

    if (useWireframe) {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0,0,0);
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        drawSphere(100.0, 20, 20);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glColor3f(1,1,1);
        glEnable(GL_LIGHTING);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D,2);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
        glColor3f(1,1,1);
        glDisable(GL_LIGHTING);

        drawSphere(100.0, 20, 20);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }

}

void drawFloor()
{
    const float S=100; // size
    const float H=0;   // altezza
    const int K=150; //disegna K x K quads

    glColor3f(1,1,1);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 3);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // disegna KxK quads
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);       // normale verticale uguale x tutti
    for (int x=0; x<K; x++)
        for (int z=0; z<K; z++) {
            float x0=-S + 2*(x+0)*S/K;
            float x1=-S + 2*(x+1)*S/K;
            float z0=-S + 2*(z+0)*S/K;
            float z1=-S + 2*(z+1)*S/K;
            glTexCoord2f(0.0, 0.0);
            glVertex3d(x0, H, z0);
            glTexCoord2f(1.0, 0.0);
            glVertex3d(x1, H, z0);
            glTexCoord2f(1.0, 1.0);
            glVertex3d(x1, H, z1);
            glTexCoord2f(0.0, 1.0);
            glVertex3d(x0, H, z1);
        }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    for (int x=0; x<K; x++)
        for (int z=0; z<K; z++)
            switch (corn[x][z]) {
                case 1:
                    drawCornBox(-S + 2*x*S/K, 0, -S + 2*z*S/K);
                    break;
                case 2:
                    drawCornGround(-S + 2*x*S/K, 0.01, -S + 2*z*S/K);
                    break;
            }
    glPopMatrix();
}


void drawBarn () {
    glPushMatrix();
    glTranslatef(0,0,-60);
    glScalef(0.5, .5, .5);
    glColor3f(.647059,.164706,.164706); // brown
    barn_house.RenderNxF();

    glColor3f(.35,.16,.14); // very dark brown
    barn_frame.RenderNxF();
    barn_roof.RenderNxF();

    glColor3f(.36,.25,.20); // dark brown
    barn_vane.RenderNxF();

    glDisable(GL_LIGHTING);
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA);
    glColor4f(0,1,1,0.3);
    // glColor3f(0,1,1); // cyan
    barn_window.RenderNxF();
    glDisable(GL_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawScarecrow () {
    glPushMatrix();
    glTranslatef(7,0,-61);
    glScalef(0.5, .5, .5);
    glColor3f(.647059,.164706,.164706); // brown
    scarecrow_base.RenderNxF();

    glColor3f(.73,.16,.96); // med purple
    scarecrow_dress.RenderNxF();

    glColor3f(.858824,.439216,.858824); // Orchid
    scarecrow_hands.RenderNxF();

    glColor3f(.8,.498039,.196078); // Gold
    scarecrow_hat.RenderNxF();

    // disegno la mia faccia sotto al cappello
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, 7);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glTranslatef(-0.4,3.75,0.1);
    glBegin(GL_QUADS);
    /* Front. */
    glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(.5, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(.5, .75, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.0, .75, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawFence () {
    // top
    glPushMatrix();
    glScalef(0.5, .5, .5);
    glColor3f(.36,.25,.20);

    glTranslatef(0,0,-120);
    glRotatef(90, 0, 1, 0);
    fence_vertical.RenderNxF();
    fence_horizontal.RenderNxF();
    glPopMatrix();

    // bottom
    glPushMatrix();
    glScalef(0.5, .5, .5);

    glTranslatef(0,0,120);
    glRotatef(90, 0, 1, 0);
    fence_vertical.RenderNxF();
    fence_horizontal.RenderNxF();
    glPopMatrix();

    // left
    glPushMatrix();
    glScalef(0.5, .5, .5);

    glTranslatef(-120,0,0);
    fence_vertical.RenderNxF();
    fence_horizontal.RenderNxF();
    glPopMatrix();

    // right
    glPushMatrix();
    glScalef(0.5, .5, .5);

    glTranslatef(120,0,0);
    fence_vertical.RenderNxF();
    fence_horizontal.RenderNxF();
    glPopMatrix();
}

void drawCornBox(int pos_x, int pos_y, int pos_z) {

    // disegno le faccia laterali di un cubo con la texture Corn_side
    glPushMatrix();
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, 4);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glTranslatef(pos_x,pos_y,pos_z);

    glBegin(GL_QUADS);
    /* Front. */
    glTexCoord2f(0.0, 0.3); glVertex3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.5, 0.3); glVertex3f(1.0, 0.0, 1.0);
    glTexCoord2f(0.5, 1.0); glVertex3f(1.0, 2.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 2.0, 1.0);

    /* Back. */
    glTexCoord2f(0.0, 0.3); glVertex3f(0.0, 2.0, 0.0);
    glTexCoord2f(0.5, 0.3); glVertex3f(1.0, 2.0, 0.0);
    glTexCoord2f(0.5, 1.0); glVertex3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, 0.0);

    /* SideLeft. */
    glTexCoord2f(0.0, 0.3); glVertex3f(0.0, 2.0, 0.0);
    glTexCoord2f(0.5, 0.3); glVertex3f(0.0, 2.0, 1.0);
    glTexCoord2f(0.5, 1.0); glVertex3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, 0.0);

    /* SideRight. */
    glTexCoord2f(0.0, 0.3); glVertex3f(1.0, 2.0, 0.0);
    glTexCoord2f(0.5, 0.3); glVertex3f(1.0, 2.0, 1.0);
    glTexCoord2f(0.5, 1.0); glVertex3f(1.0, 0.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 0.0, 0.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // disegno la faccia sopra del cubo con la texture Corn_top
    glBindTexture(GL_TEXTURE_2D, 5);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glBegin(GL_QUADS);
    /* Up. */
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 2.0, 1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 2.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 2.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 2.0, 0.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawCornGround(int pos_x, float pos_y, int pos_z) {
    glPushMatrix();
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, 6);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glTranslatef(pos_x,pos_y,pos_z);

    // disegna KxK quads
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0.0, 0.0);glVertex3d(0, 0, 0);
    glTexCoord2f(1.0, 0.0);glVertex3d(1.0, 0, 0);
    glTexCoord2f(1.0, 1.0);glVertex3d(1.0, 0, 1.0);
    glTexCoord2f(0.0, 1.0);glVertex3d(0, 0, 1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void finalScreen (SDL_Window *win, TTF_Font *font, int scrH, int scrW, int f, int max) {
    // settiamo il viewport
    glViewport(0,0, scrW, scrH);

    // colore di sfondo (fuori dal mondo)
    glClearColor(.0,.0,.0,1);

    // riempe tutto lo screen buffer di pixel color sfondo
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    SetCoordToPixel(scrW, scrH);

    glLineWidth(2);

    char stringa_punti[23];
    char game_over[] = "TEMPO SCADUTO";
    char continuare[] = "Premi un <ESC> per uscire";

    sprintf(stringa_punti, "Raccolto: %dKg - %d%", f, f/max);

    drawText(font, 255, 255, 255, 255, 0, 0, 0, 255, stringa_punti, scrW/2-80, scrH/2+100, false);
    drawText(font, 255, 255, 255, 255, 0, 0, 0, 255, game_over, scrW/2-90, scrH/3+20, false);
    drawText(font, 255, 255, 255, 255, 0, 0, 0, 255, continuare, scrW/2-130, scrH/4+20, false);
    glFinish();

    SDL_GL_SwapWindow(win);
}

void drawText(TTF_Font *font, // font
    Uint8 fgR, Uint8 fgG, Uint8 fgB, Uint8 fgA, // colore testo
    Uint8 bgR, Uint8 bgG, Uint8 bgB, Uint8 bgA, // colore background
    char *text, int x, int y, // testo e posizione
    bool border ) {

    Uint32 rmask, gmask, bmask, amask;
    SDL_Color fontcolor = {fgR,fgG,fgB,fgA};
    SDL_Color bgcolor = {bgR, bgG, bgB, bgA};
    SDL_Surface *text_surface;
    SDL_Surface *screen;
    uint TextureID = -1;

    if(!(text_surface = TTF_RenderText_Shaded(font, text, fontcolor, bgcolor))){
        //handle error here, perhaps print TTF_GetError at least
    } else {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
        #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
        #endif
        /* Allochiamo una nuova surface RGB */
        screen = SDL_CreateRGBSurface(0, text_surface->w, text_surface->h, 32,rmask, gmask, bmask, amask);

        /* Copiamo il contenuto dalla prima alla seconda surface */
        SDL_BlitSurface(text_surface, 0, screen, 0);

        /* Informiamo GL della nuova texture */
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, text_surface->w, text_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (border) {
            glLineWidth(2);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(x-2, y-2);
            glVertex2f(x + screen->w+2, y-2);
            glVertex2f(x + screen->w+2, y + screen->h+2);
            glVertex2f(x-2, y + screen->h+2);
            glEnd();
        }

        /* prepariamoci al rendering del testo */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x + screen->w, y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x + screen->w, y + screen->h);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + screen->h);
        glEnd();

        /* Clean up */
        glDisable(GL_TEXTURE_2D);
        SDL_FreeSurface(text_surface);
        SDL_FreeSurface(screen);
        glDeleteTextures(1, &TextureID);
    }
}

void drawMinimap(TTF_Font *font, int u) {
    /* calcolo delle coordinate reali dell'oggetto su minimappa */
    float minimap_posx;
    float minimap_posz;
    minimap_posx = ((50*tractor.px)/67) + 50 + 20;
    minimap_posz = ((50*tractor.pz)/67) + 50 + scrH-20-100;
    char point[15];

    float minimap_cubex, minimap_cubez, pos_x, pos_z;

    /* disegno minimappa */
    glColor3ub(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(20,scrH -20 -100);
    glVertex2d(20,scrH -20);
    glVertex2d(120,scrH-20);
    glVertex2d(120,scrH-20-100);
    glEnd();

    glColor3ub(210,210,210);
    glBegin(GL_POLYGON);
      glVertex2d(20,scrH -20 -100);
      glVertex2d(20,scrH -20);
      glVertex2d(120,scrH -20);
      glVertex2d(120,scrH-20-100);
     glEnd();

    /* disegno del target */
    glColor3ub(255,0,0);
    for (int x=0; x<150; x++)
        for (int z=0; z<150; z++)
            if (corn[x][z] == 1) {
                pos_x = -100 + 2*x*100/150;
                pos_z = -100 + 2*z*100/150;
                minimap_cubex = ((50*pos_x)/67) + 50 + 20;
                minimap_cubez = ((50*pos_z)/67) + 50 + scrH-20-100;
                glBegin(GL_POINTS);
                  glVertex2d(minimap_cubex, minimap_cubez);
                glEnd();
            }

    /* disegno del cursore */
    glColor3ub(0,0,255);
    glBegin(GL_QUADS);
    glVertex2d(minimap_posx, minimap_posz + 3);
    glVertex2d(minimap_posx + 3, minimap_posz);
    glVertex2d(minimap_posx, minimap_posz - 3);
    glVertex2d(minimap_posx - 3, minimap_posz);
    glEnd();

    sprintf(point, "Raccolto %dKg", u);

    // scrivo la percentuale di frumento raccolto
    drawText(font, 255, 255, 255, 255, 0, 0, 0, 0, point, 140, scrH-20-30, true);
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schermo
void  SetCoordToPixel(int scrW, int scrH){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1,-1,0);
    glScalef(2.0/scrW, 2.0/scrH, 1);
}

void drawFrameBar(float fps) {
    glBegin(GL_QUADS);
    float y=scrH*fps/100;
    float ramp=fps/100;
    glColor3f(1-ramp,0,ramp);
    glVertex2d(10,0);
    glVertex2d(10,y);
    glVertex2d(0,y);
    glVertex2d(0,0);
    glEnd();
}

void drawForageBar(int forage, int MaxForage) {
    glBegin(GL_QUADS);
    float y=scrH*forage/MaxForage;
    float ramp=forage/(float)MaxForage;
    glColor3f(1-ramp,ramp,0);
    glVertex2d(20,0);
    glVertex2d(20,y);
    glVertex2d(10,y);
    glVertex2d(10,0);
    glEnd();
}

void drawTimeBar(int time, int MaxTime) {
    float remainTime = MaxTime - time;
    glBegin(GL_QUADS);
    float y=scrH*remainTime/MaxTime;
    float ramp=remainTime/MaxTime;
    glColor3f(1-ramp,ramp,0);
    glVertex2d(scrW,0);
    glVertex2d(scrW,y);
    glVertex2d(scrW - 10,y);
    glVertex2d(scrW - 10,0);
    glEnd();
}
