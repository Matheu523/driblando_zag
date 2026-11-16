/*
 * recursos.h - carregamento de imagens e sons (pasta assets/).
 */
#ifndef RECURSOS_H
#define RECURSOS_H

#include "raylib.h"

typedef struct {
    Texture2D atacantes[3], zagueiro, goleiro, bola, moeda, gol, arquibancada;
    Sound somChute, somMoeda, somDrible, somFalta, somApito, somGol, somMenu;
    Music torcida;
    bool audio;
} Recursos;

extern Recursos R;

void Recursos_Carregar(void);
void Recursos_Liberar(void);
void Tocar(Sound s);

#endif
