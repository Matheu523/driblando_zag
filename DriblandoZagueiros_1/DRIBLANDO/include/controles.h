/*
 * controles.h - leitura de teclado e gamepad em um só lugar.
 * Também permite rodar roteiros de teste (variável de ambiente DZ_ROTEIRO).
 */
#ifndef CONTROLES_H
#define CONTROLES_H

#include <stdbool.h>

typedef enum {
    CMD_CIMA, CMD_BAIXO, CMD_ESQUERDA, CMD_DIREITA,
    CMD_CONFIRMAR, CMD_VOLTAR, CMD_PAUSA, CMD_TELA_CHEIA,
    CMD_TOTAL
} Comando;

void  Controles_Iniciar(void);
void  Controles_Atualizar(void);
bool  Controle_Segurando(Comando c);
bool  Controle_Apertou(Comando c);
float Controles_DeltaTempo(void);
bool  Controles_RoteiroTerminou(void);
const char *Controles_PedidoCaptura(void);

#endif
