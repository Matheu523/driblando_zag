/*
 * recorde.h - maior pontuação salva em arquivo (recorde.dat).
 */
#ifndef RECORDE_H
#define RECORDE_H

#include <stdbool.h>

int  Recorde_Ler(void);
bool Recorde_Salvar(int pontos);   /* grava se for maior; devolve true se é novo recorde */

#endif
