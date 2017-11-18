#include <math.h>

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

#include "tractor.h"
#include "drawScene.h"

#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5


float viewAlpha=20, viewBeta=40;    // angoli che definiscono la vista
float eyeDist=5.0;                  // distanza dell'occhio dall'origine
int scrH=750, scrW=750;             // altezza e larghezza viewport (in pixels)
bool useWireframe=false;
bool useEnvmap=false;
bool useHeadlight=false;
bool useShadow=true;
int cameraType=0;

Tractor tractor;                    // la nostra macchina
int nstep=0;                        // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP=10;    // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000;       // lunghezza intervallo di calcolo fps
float fps=0;                        // valore di fps dell'intervallo precedente
int fpsNow=0;                       // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval=0;          // quando e' cominciato l'ultimo intervallo
Uint32 startTime;
Uint32 endTime = 60000;             // dopo quanti millesecondi termina il gioco
TTF_Font *font;                     // variabile del font

int corn[150][150];                 // matrice del granoturco
int MaxForage = 0;                  // numero massimo di granoturco disponibile
int forage = 0;                     // granoturco raccolto fino a questo momento


// esegue il binding della texture
bool LoadTexture(int textbind,char *filename){
    SDL_Surface *s = IMG_Load(filename);
    if (!s) return false;

    glBindTexture(GL_TEXTURE_2D, textbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,s->w, s->h,GL_RGB,GL_UNSIGNED_BYTE,s->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
    return true;
}


// setto la posizione della camera
void setCamera(){

    double px = tractor.px;
    double py = tractor.py;
    double pz = tractor.pz; //-1.75
    double angle = tractor.facing;
    double cosf = cos(angle*M_PI/180.0);
    double sinf = sin(angle*M_PI/180.0);
    double camd, camh, ex, ey, ez, cx, cy, cz;
    double cosff, sinff;

    // controllo la posizione della camera a seconda dell'opzione selezionata
    switch (cameraType) {
        case CAMERA_BACK_CAR:
        camd = 2.5;
        camh = 1.65;
        ex = px + camd*sinf;
        ey = py + camh;
        ez = pz + camd*cosf;
        cx = px - camd*sinf;
        cy = py + camh;
        cz = pz - camd*cosf;
        gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
        break;
        case CAMERA_TOP_FIXED:
        camd = -2.5;
        camh = 1;
        angle = tractor.facing + 40.0;
        cosff = cos(angle*M_PI/180.0);
        sinff = sin(angle*M_PI/180.0);
        ex = px + camd*sinff;
        ey = py + camh;
        ez = pz + camd*cosff;
        cx = px - camd*sinf;
        cy = py + camh;
        cz = pz - camd*cosf;
        gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
        break;
        case CAMERA_TOP_CAR:
        camd = 2.5;
        camh = 1.0;
        ex = px + camd*sinf;
        ey = py + camh;
        ez = pz + camd*cosf;
        cx = px - camd*sinf;
        cy = py + camh;
        cz = pz - camd*cosf;
        gluLookAt(ex,ey+5,ez,cx,cy,cz,0.0,1.0,0.0);
        break;
        case CAMERA_PILOT:
        camd = 0.35;
        camh = 1.5;
        ex = px + camd*sinf;
        ey = py + camh;
        ez = pz + camd*cosf;
        cx = px - camd*sinf;
        cy = py + camh;
        cz = pz - camd*cosf;
        gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
        break;
        case CAMERA_MOUSE:
        glTranslatef(0,0,-eyeDist);
        glRotatef(viewBeta,  1,0,0);
        glRotatef(viewAlpha, 0,1,0);
        break;
    }
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win){

    fpsNow++;

    glLineWidth(3);     // linee larghe

    // setta il viewport
    glViewport(0,0, scrW, scrH);

    // colore sfondo = bianco
    glClearColor(1,1,1,1);


    // settiamo la matrice di proiezione
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 70,         //fovy,
        ((float)scrW) / scrH,   //aspect Y/X,
        0.2,                    //distanza del NEAR CLIPPING PLANE in coordinate vista
        1000                    //distanza del FAR CLIPPING PLANE in coordinate vista
    );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // riempe tutto lo screen buffer di pixel color sfondo
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // setto la posizione luce
    float tmpv[4] = {0,1,2,  0};                // ultima comp=0 => luce direzionale
    glLightfv(GL_LIGHT0, GL_POSITION, tmpv );


    setCamera();

    static float tmpcol[4] = {1,1,1,  1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

    glEnable(GL_LIGHTING);

    // disegna la scena
    drawSky();          // disegna il cielo come sfondo
    drawBarn();         // disegna la fattoria
    drawFence();        // disegna la staccionata
    drawScarecrow ();   // disegna lo spaventapasseri
    drawFloor();        // disegna il terreno



    tractor.Render();   // disegna il trattore

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // passo nelle cordinate dello schermo
    SetCoordToPixel(scrW, scrH);

    // disegno le barre dell'interfaccia
    drawFrameBar(fps);
    drawForageBar(forage, MaxForage);
    drawTimeBar(SDL_GetTicks() - startTime, endTime);

    // disegna la minimappa in alto a destra + il raccolto
    drawMinimap(font, forage);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);


    glFinish();
    // mostro il buffer di lavoro
    SDL_GL_SwapWindow(win);
}

void redraw(){
    // ci automandiamo un messaggio che (s.o. permettendo)
    // ci fara' ridisegnare la finestra
    SDL_Event e;
    e.type=SDL_WINDOWEVENT;
    e.window.event=SDL_WINDOWEVENT_EXPOSED;
    SDL_PushEvent(&e);
}

int main(int argc, char* argv[]) {
    SDL_Window *win;
    SDL_GLContext mainContext;
    Uint32 windowID;
    SDL_Joystick *joystick;
    static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

    // inizializzazione di SDL
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    // inizializzazione di SDL_TTF
    if(TTF_Init() < 0) {
        fprintf(stderr, "Impossibile inizializzare TTF: %s\n",SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // facciamo una finestra di scrW x scrH pixels
    win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    // crea il contesto openGL e lo attacca alla finestra
    mainContext=SDL_GL_CreateContext(win);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);             // opengl rinormalizza le normali
    glFrontFace(GL_CW);                 // consideriamo Front Facing le facce ClockWise
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    // openGL sposta i frammenti generati dalla rasterizzazione poligoni indietro di 1
    glPolygonOffset(1,1);

    // carica le textures
    if (!LoadTexture(0,(char *)"img/logo.jpg")) return 0;
    if (!LoadTexture(1,(char *)"img/metal_yellow.jpg")) return 0;
    if (!LoadTexture(2,(char *)"img/sky_ok.jpg")) return -1;
    if (!LoadTexture(3,(char *)"img/grass.jpg")) return -1;
    if (!LoadTexture(4,(char *)"img/Corn_side.jpg")) return -1;
    if (!LoadTexture(5,(char *)"img/Corn_top.jpg")) return -1;
    if (!LoadTexture(6,(char *)"img/Corn_ground.jpg")) return -1;
    if (!LoadTexture(7,(char *)"img/lorenzo.jpg")) return -1;

    // carica il font
    if (!(font = TTF_OpenFont ("font/FreeSans.ttf", 22)))
        printf("TTF_OpenFont: %s\n", TTF_GetError());

    // inizializza la matrice del granoturco
    for (int x=0; x<150; x++)
        for (int z=0; z<150; z++)
            if ((50 < z && z < 100) && ((40 < x && x < 65) || (110 > x && x > 85))) {
                corn[x][z] = 1;
                MaxForage ++;
            } else {
                corn[x][z] = 0;
            }


    bool done=0;
    startTime = SDL_GetTicks();  // tempo di inizio
    // inizio del ciclo degli eventi
    while (!done) {

        SDL_Event e;

        // guardo se c'e' un evento:
        if (SDL_PollEvent(&e)) {
            // se si: processa evento
            switch (e.type) {
                case SDL_KEYDOWN:
                tractor.controller.EatKey(e.key.keysym.sym, keymap , true);
                if (e.key.keysym.sym==SDLK_F1) cameraType=(cameraType+1)%CAMERA_TYPE_MAX;
                if (e.key.keysym.sym==SDLK_F2) useWireframe=!useWireframe;
                if (e.key.keysym.sym==SDLK_F3) useEnvmap=!useEnvmap;
                if (e.key.keysym.sym==SDLK_F4) useHeadlight=!useHeadlight;
                if (e.key.keysym.sym==SDLK_F5) useShadow=!useShadow;
                break;
                case SDL_KEYUP:
                tractor.controller.EatKey(e.key.keysym.sym, keymap , false);
                break;
                case SDL_QUIT:
                done=1;   break;
                case SDL_WINDOWEVENT:
                // dobbiamo ridisegnare la finestra
                if (e.window.event==SDL_WINDOWEVENT_EXPOSED)
                rendering(win);
                else{
                    windowID = SDL_GetWindowID(win);
                    if (e.window.windowID == windowID)  {
                        switch (e.window.event)  {
                            case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                                scrW = e.window.data1;
                                scrH = e.window.data2;
                                glViewport(0,0,scrW,scrH);
                                rendering(win);
                                // redraw(); // richiedi ridisegno
                                break;
                            }
                        }
                    }
                }
                break;

                case SDL_MOUSEMOTION:
                if (e.motion.state & SDL_BUTTON(1) & cameraType==CAMERA_MOUSE) {
                    viewAlpha+=e.motion.xrel;
                    viewBeta +=e.motion.yrel;
                    if (viewBeta<+5) viewBeta=+5;   //per non andare sotto la macchina
                    if (viewBeta>+90) viewBeta=+90;
                }
                break;

                case SDL_MOUSEWHEEL:
                if (e.wheel.y < 0 ) {
                    // avvicino il punto di vista (zoom in)
                    eyeDist=eyeDist*0.9;
                    if (eyeDist<1) eyeDist = 1;
                };
                if (e.wheel.y > 0 ) {
                    // allontano il punto di vista (zoom out)
                    eyeDist=eyeDist/0.9;
                };
                break;

                case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
                if( e.jaxis.axis == 0)
                {
                    if ( e.jaxis.value < -3200  )
                    {
                        tractor.controller.Joy(0 , true);
                        tractor.controller.Joy(1 , false);
                    }
                    if ( e.jaxis.value > 3200  )
                    {
                        tractor.controller.Joy(0 , false);
                        tractor.controller.Joy(1 , true);
                    }
                    if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 )
                    {
                        tractor.controller.Joy(0 , false);
                        tractor.controller.Joy(1 , false);
                    }
                    rendering(win);
                }
                break;
                case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
                if ( e.jbutton.button == 0 )
                {
                    tractor.controller.Joy(2 , true);
                }
                if ( e.jbutton.button == 2 )
                {
                    tractor.controller.Joy(3 , true);
                }
                break;
                case SDL_JOYBUTTONUP: /* Handle Joystick Button Presses */
                tractor.controller.Joy(2 , false);
                tractor.controller.Joy(3 , false);
                break;
            }
        } else {
            // nessun evento: siamo IDLE

            Uint32 timeNow=SDL_GetTicks();      // tempo corrente

            if (timeLastInterval+fpsSampling<timeNow) {
                fps = 1000.0*((float)fpsNow) /(timeNow-timeLastInterval);
                fpsNow=0;
                timeLastInterval = timeNow;
            }

            bool doneSomething=false;
            int guardia=0; // sicurezza da loop infinito

            // finche' il tempo simulato e' rimasto indietro rispetto
            // al tempo reale...
            while (nstep*PHYS_SAMPLING_STEP < timeNow ) {
                tractor.DoStep();
                nstep++;
                doneSomething=true;
                timeNow=SDL_GetTicks();
                if (guardia++>1000) {done=true; break;} // siamo troppo lenti!
            }

            if (doneSomething)
                rendering(win);

            // tempo di gioco esaurito
            if (SDL_GetTicks() - startTime > endTime) {
                done = 0;
                while (!done) {
                    SDL_Event e;
                    if (SDL_PollEvent(&e)) {
                        switch (e.type) {
                            case SDL_KEYDOWN:
                            if (e.key.keysym.sym==SDLK_ESCAPE) done = 1;
                            break;
                            case SDL_JOYBUTTONDOWN:
                            done = 1;
                            break;
                            case SDL_QUIT:
                            done = 1;
                            break;
                            case SDL_WINDOWEVENT:
                            windowID = SDL_GetWindowID(win);
                            if (e.window.windowID == windowID) {
                                switch (e.window.event)  {
                                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                                        scrW = e.window.data1;
                                        scrH = e.window.data2;
                                        glViewport(0,0,scrW,scrH);
                                        break;
                                    }
                                }
                            }
                        }
                    } else { // disegno la schermata di fine partita
                        finalScreen(win, font, scrH, scrW, forage, MaxForage);
                    }
                }
            }
        }
    }



    SDL_GL_DeleteContext(mainContext);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit ();
    return (0);
}
