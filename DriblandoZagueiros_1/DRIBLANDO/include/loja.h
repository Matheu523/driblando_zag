/*
 * loja.h - tela de melhorias entre as fases (paga com moedas).
 */
#ifndef LOJA_H
#define LOJA_H

#include <stdbool.h>
#include "jogador.h"

typedef struct { int cursor; float aviso; bool sucesso; char mensagem[64]; } Loja;

void Loja_Abrir(Loja *l);
/* devolve true quando o jogador escolhe "Próxima fase" */
bool Loja_Atualizar(Loja *l, Jogador *j, int *moedas, float dt);
void Loja_Desenhar(const Loja *l, const Jogador *j, int moedas, int proximaFase);
int  Loja_Custo(int nivelAtual);

#endif
