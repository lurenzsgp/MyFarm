class Controller{
public:
    enum { LEFT=0, RIGHT=1, ACC=2, DEC=3, NKEYS=4};
    bool key[NKEYS];
    void Init();
    void EatKey(int keycode, int* keymap, bool pressed_or_released);
    void Joy(int keymap, bool pressed_or_released);
    Controller(){Init();} // costruttore
};


class Tractor{

    void RenderAllParts(bool usecolor) const;

public:
    // Metodi
    void Init(); // inizializza variabili
    void Render() const; // disegna a schermo
    void DoStep(); // computa un passo del motore fisico
    Tractor(){Init();} // costruttore

    Controller controller;

    // STATO DELLA MACCHINA
    float px,py,pz,facing; // posizione e orientamento
    float mozzoA, mozzoP, sterzo; // stato interno
    float vx,vy,vz; // velocita' attuale

    // STATS DELLA MACCHINA
    float velSterzo, velRitornoSterzo, accMax, attrito,
    raggioRuotaA, raggioRuotaP, grip,
    attritoX, attritoY, attritoZ; // attriti

private:
    void DrawHeadlight(float x, float y, float z, int lightN, bool useHeadlight) const;
};
