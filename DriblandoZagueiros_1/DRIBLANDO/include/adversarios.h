/*
 * adversarios.h - zagueiros que vêm na direção do atacante e moedas no gramado.
 *
 * Tipos de zagueiro:
 *   NORMAL   - corre em linha reta
 *   MARCADOR - acompanha a altura do atacante
 *   CARRINHO - avisa com "!" e dá um carrinho rápido em diagonal
 */
#ifndef ADVERSARIOS_H
#define ADVERSARIOS_H

#include <stdbool.h>
#include "raylib.h"
#include "config.h"
#include "jogador.h"

typedef enum { ZAG_NORMAL, ZAG_MARCADOR, ZAG_CARRINHO } TipoZagueiro;

typedef struct {
    bool ativo;
    TipoZagueiro tipo;
    Vector2 pos;
    float vel;            /* velocidade própria (além da rolagem do campo) */
    float vy;
    float menorDist;      /* menor distância vertical ao atacante na passagem */
    bool  passou;         /* já passou pelo atacante (drible avaliado) */
    bool  aviso;          /* carrinho: mostrando "!" */
    bool  deslizando;     /* carrinho: em movimento rápido */
    float timer;
    float anim;
} Zagueiro;

typedef struct { bool ativo; Vector2 pos; float anim; } Moeda;

typedef struct {
    Zagueiro zagueiros[MAX_ZAGUEIROS];
    Moeda    moedas[MAX_MOEDAS];
    float    tempoProximo;
    float    tempoMoeda;
} Adversarios;

/* Eventos de um quadro, para o módulo jogo somar pontos e tocar sons */
typedef struct {
    int  dribles;         /* dribles realizados neste quadro */
    int  moedas;          /* moedas coletadas neste quadro */
    bool perdeuBola;      /* colidiu com um zagueiro */
} EventosCampo;

void         Adversarios_Limpar(Adversarios *a);
EventosCampo Adversarios_Atualizar(Adversarios *a, Jogador *j, int fase, float rolagem, float dt, bool gerarNovos);
void         Adversarios_Desenhar(const Adversarios *a);
int          Adversarios_Ativos(const Adversarios *a);

#endif
