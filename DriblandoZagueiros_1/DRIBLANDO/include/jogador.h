/*
 * jogador.h - o atacante: movimento, vidas e melhorias compradas na loja.
 */
#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"

typedef struct {
    Vector2 pos;            /* canto superior esquerdo do sprite 16x24 */
    int   vidas;
    float invencivel;       /* segundos de invencibilidade após perder a bola */
    float anim;
    int   quadro;
    int   personagem;       /* índice em personagens.c (define sprite e bônus) */
    /* níveis COMPRADOS na loja (0..MAX_NIVEL_MELHORIA) */
    int   nivelVelocidade, nivelDrible, nivelChute, nivelFolego;
} Jogador;

void      Jogador_Novo(Jogador *j, int personagem);
/* nível efetivo = bônus do personagem + nível comprado */
int       Jogador_NivelVelocidade(const Jogador *j);
int       Jogador_NivelDrible(const Jogador *j);
int       Jogador_NivelChute(const Jogador *j);
int       Jogador_NivelFolego(const Jogador *j);
void      Jogador_DesenharSprite(int personagem, Vector2 pos, int quadro, float escala);
void      Jogador_IniciarFase(Jogador *j);
void      Jogador_Atualizar(Jogador *j, float dt);
void      Jogador_Desenhar(const Jogador *j);
Rectangle Jogador_Hitbox(const Jogador *j);
int       Jogador_VidasMaximas(const Jogador *j);
float     Jogador_Velocidade(const Jogador *j);

#endif
