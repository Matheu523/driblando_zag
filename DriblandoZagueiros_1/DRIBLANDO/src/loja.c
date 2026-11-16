/*
 * loja.c - melhorias permanentes compradas com moedas entre as fases.
 */
#include <stdio.h>
#include "loja.h"
#include "config.h"
#include "controles.h"
#include "recursos.h"
#include "interface.h"
#include "personagens.h"

#define ITENS 4

static const char *NOMES[ITENS] = { "Velocidade", "Drible", "Chute", "Fôlego" };
static const char *DESCRICOES[ITENS] = {
    "Corre mais rápido pelo campo.",
    "Área de contato menor: mais fácil escapar.",
    "Chute mais forte e mira mais precisa.",
    "+1 vida em cada fase.",
};

int Loja_Custo(int nivelAtual) { return 4 + nivelAtual * 3; }   /* 4, 7, 10 moedas */

static int *Nivel(Jogador *j, int item)
{
    switch (item) {
        case 0: return &j->nivelVelocidade;
        case 1: return &j->nivelDrible;
        case 2: return &j->nivelChute;
        default: return &j->nivelFolego;
    }
}

void Loja_Abrir(Loja *l) { *l = (Loja){ 0 }; }

bool Loja_Atualizar(Loja *l, Jogador *j, int *moedas, float dt)
{
    if (l->aviso > 0) l->aviso -= dt;
    if (Controle_Apertou(CMD_CIMA))  { l->cursor = (l->cursor + ITENS) % (ITENS + 1); Tocar(R.somMenu); }
    if (Controle_Apertou(CMD_BAIXO)) { l->cursor = (l->cursor + 1) % (ITENS + 1); Tocar(R.somMenu); }
    if (!Controle_Apertou(CMD_CONFIRMAR)) return false;
    l->sucesso = false;

    if (l->cursor == ITENS) return true;           /* "Próxima fase" */
    int *nivel = Nivel(j, l->cursor);
    if (*nivel >= MAX_NIVEL_MELHORIA) {
        snprintf(l->mensagem, sizeof(l->mensagem), "%s já está no nível máximo!", NOMES[l->cursor]);
    } else if (*moedas < Loja_Custo(*nivel)) {
        snprintf(l->mensagem, sizeof(l->mensagem), "Moedas insuficientes.");
    } else {
        *moedas -= Loja_Custo(*nivel);
        (*nivel)++;
        snprintf(l->mensagem, sizeof(l->mensagem), "%s melhorado para o nível %d!", NOMES[l->cursor], *nivel);
        l->sucesso = true;
        Tocar(R.somMoeda);
    }
    l->aviso = 2.0f;
    return false;
}

void Loja_Desenhar(const Loja *l, const Jogador *j, int moedas, int proximaFase)
{
    Jogador copia = *j;
    DrawRectangle(0, 0, TELA_W, TELA_H, (Color){ 10, 30, 20, 220 });
    Painel((Rectangle){ 40, 20, TELA_W - 80, TELA_H - 40 });
    TextoCentro("CENTRO DE TREINAMENTO", TELA_W / 2, 30, 20, (Color){ 30, 90, 50, 255 });
    char txt[96];
    snprintf(txt, sizeof(txt), "Moedas: %d", moedas);
    TextoCentro(txt, TELA_W / 2, 54, 10, (Color){ 180, 130, 0, 255 });

    for (int i = 0; i < ITENS; i++) {
        int y = 74 + i * 32;
        int nivel = *Nivel(&copia, i);
        bool sel = l->cursor == i;
        if (sel) DrawRectangle(52, y - 3, TELA_W - 104, 30, (Color){ 60, 140, 80, 60 });
        Texto(NOMES[i], 62, y, 10, sel ? (Color){ 20, 110, 50, 255 } : DARKGRAY);
        Texto(DESCRICOES[i], 62, y + 13, 10, GRAY);
        /* azul = bônus do personagem, verde = comprado, cinza = disponível */
        const Personagem *p = Personagem_Obter(j->personagem);
        int bonus[ITENS] = { p->velocidade, p->drible, p->chute, p->folego };
        for (int k = 0; k < bonus[i] + MAX_NIVEL_MELHORIA; k++) {
            Color c = k < bonus[i] ? (Color){ 60, 120, 220, 255 } : (k < bonus[i] + nivel ? (Color){ 40, 170, 80, 255 } : LIGHTGRAY);
            DrawRectangle(262 + k * 14, y + 1, 10, 10, c);
        }
        if (nivel < MAX_NIVEL_MELHORIA) snprintf(txt, sizeof(txt), "%d moedas", Loja_Custo(nivel));
        else snprintf(txt, sizeof(txt), "MÁXIMO");
        Texto(txt, 360, y + 1, 10, nivel < MAX_NIVEL_MELHORIA && moedas >= Loja_Custo(nivel) ? (Color){ 180, 130, 0, 255 } : GRAY);
    }
    bool sel = l->cursor == ITENS;
    snprintf(txt, sizeof(txt), "Jogar a fase %d  >", proximaFase);
    DrawRectangle(TELA_W / 2 - 70, 204, 140, 18, sel ? (Color){ 40, 170, 80, 255 } : GRAY);
    TextoCentro(txt, TELA_W / 2, 209, 10, WHITE);
    TextoCentro("Azul: bônus do craque   Verde: comprado", TELA_W / 2, 239, 10, GRAY);
    if (l->aviso > 0) TextoCentro(l->mensagem, TELA_W / 2, 230, 10, l->sucesso ? (Color){ 20, 120, 50, 255 } : (Color){ 160, 40, 40, 255 });
}
