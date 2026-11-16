/*
 * interface.h - textos, painéis, campo de fundo, HUD e telas fixas.
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#include "raylib.h"

void Texto(const char *t, int x, int y, int tam, Color c);
void TextoCentro(const char *t, int cx, int y, int tam, Color c);
void TextoSombra(const char *t, int cx, int y, int tam, Color c);
void Painel(Rectangle r);
void Interface_DesenharCampo(float deslocamento);
void Interface_DesenharHUD(int fase, int pontos, int moedas, int vidas, int vidasMax,
                           int combo, float progresso);
void Interface_Avisos(float dt);
void Interface_NovoAviso(const char *t, Color c);
void Interface_DesenharAvisos(void);
void Interface_LimparAvisos(void);

#endif
