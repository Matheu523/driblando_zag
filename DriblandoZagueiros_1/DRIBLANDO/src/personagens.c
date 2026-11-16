/*
 * personagens.c - tabela de personagens jogáveis (fictícios).
 * Para mudar nomes ou equilibrar atributos, edite só esta tabela.
 * Todos têm 3 pontos de bônus distribuídos de formas diferentes.
 */
#include "personagens.h"

static const Personagem PERSONAGENS[TOTAL_PERSONAGENS] = {
    /* nome          estilo         descrição                               vel drb cht fol */
    { "Ziguezague", "Driblador",   "Liso: difícil de desarmar.",            1,  2,  0,  0 },
    { "Canhão",     "Artilheiro",  "Chute forte e mira precisa.",           0,  0,  2,  1 },
    { "Foguete",    "Velocista",   "Arranque explosivo.",                   2,  0,  1,  0 },
};

const Personagem *Personagem_Obter(int indice)
{
    if (indice < 0 || indice >= TOTAL_PERSONAGENS) indice = 0;
    return &PERSONAGENS[indice];
}
