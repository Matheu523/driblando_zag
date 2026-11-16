/*
 * jogo.h - estados do jogo e fluxo entre telas.
 *
 *  MENU -> COMO_JOGAR
 *  MENU -> ESCOLHA (personagem) -> CORRIDA -> FINALIZACAO -> FASE_CONCLUIDA -> LOJA -> CORRIDA ...
 *                                                    -> (5ª fase) VITORIA
 *  CORRIDA/FINALIZACAO sem vidas -> FIM_DE_JOGO
 *  CORRIDA/FINALIZACAO <-> PAUSA
 */
#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>

typedef enum {
    ESTADO_MENU, ESTADO_COMO_JOGAR, ESTADO_ESCOLHA, ESTADO_CORRIDA, ESTADO_FINALIZACAO,
    ESTADO_FASE_CONCLUIDA, ESTADO_LOJA, ESTADO_PAUSA, ESTADO_FIM_DE_JOGO, ESTADO_VITORIA
} EstadoJogo;

void Jogo_Iniciar(void);
void Jogo_Atualizar(float dt);
void Jogo_Desenhar(void);
bool Jogo_Sair(void);
void Jogo_Encerrar(void);

#endif
