/*
 * personagens.h - os três craques que podem ser escolhidos antes da partida.
 * Cada um começa com níveis diferentes de atributo (somados às melhorias da loja).
 */
#ifndef PERSONAGENS_H
#define PERSONAGENS_H

#define TOTAL_PERSONAGENS 3

typedef struct {
    const char *nome;
    const char *estilo;
    const char *descricao;
    int velocidade, drible, chute, folego;   /* níveis iniciais (bônus) */
} Personagem;

const Personagem *Personagem_Obter(int indice);

#endif
