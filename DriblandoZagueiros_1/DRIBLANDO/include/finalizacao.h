/*
 * finalizacao.h - o chute a gol no fim de cada fase.
 * Uma seta oscila; o jogador se posiciona (cima/baixo) e chuta no momento certo,
 * tentando passar pelo goleiro.
 */
#ifndef FINALIZACAO_H
#define FINALIZACAO_H

#include <stdbool.h>
#include "raylib.h"
#include "jogador.h"

typedef enum { FIN_MIRANDO, FIN_BOLA_VIAJANDO, FIN_RESULTADO } EstadoFinalizacao;
typedef enum { RES_NENHUM, RES_GOL, RES_DEFESA, RES_FORA } ResultadoChute;

typedef struct {
    EstadoFinalizacao estado;
    float atacanteY;
    float angulo, direcaoAngulo;    /* mira em graus */
    Vector2 bola, velBola;
    float goleiroY, goleiroVel;
    float timer;
    int   chancesRestantes;
    ResultadoChute ultimo;
    int   fase;
} Finalizacao;

void           Finalizacao_Iniciar(Finalizacao *f, int fase);
/* devolve RES_GOL quando marca; RES_FORA/RES_DEFESA após um chute perdido; RES_NENHUM em andamento */
ResultadoChute Finalizacao_Atualizar(Finalizacao *f, const Jogador *j, float dt);
void           Finalizacao_Desenhar(const Finalizacao *f, const Jogador *j);

#endif
