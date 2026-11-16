/*
 * interface.c - funções de desenho reutilizadas por todas as telas.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "interface.h"
#include "config.h"
#include "recursos.h"

void Texto(const char *t, int x, int y, int tam, Color c)
{
    DrawTextEx(GetFontDefault(), t, (Vector2){ (float)x, (float)y }, (float)tam, tam / 10.0f, c);
}

void TextoCentro(const char *t, int cx, int y, int tam, Color c)
{
    Vector2 m = MeasureTextEx(GetFontDefault(), t, (float)tam, tam / 10.0f);
    Texto(t, cx - (int)(m.x / 2), y, tam, c);
}

void TextoSombra(const char *t, int cx, int y, int tam, Color c)
{
    int s = tam >= 20 ? 2 : 1;
    TextoCentro(t, cx + s, y + s, tam, (Color){ 0, 0, 0, 170 });
    TextoCentro(t, cx, y, tam, c);
}

void Painel(Rectangle r)
{
    DrawRectangleRec((Rectangle){ r.x + 3, r.y + 3, r.width, r.height }, (Color){ 0, 0, 0, 90 });
    DrawRectangleRec(r, (Color){ 250, 250, 242, 255 });
    DrawRectangleLinesEx(r, 2, (Color){ 30, 90, 50, 255 });
}

void Interface_DesenharCampo(float deslocamento)
{
    /* gramado listrado que rola para a esquerda */
    int faixa = 32;
    int off = (int)fmodf(deslocamento, (float)(faixa * 2));
    for (int x = -off; x < TELA_W; x += faixa) {
        int idx = (int)floorf((x + deslocamento) / faixa);
        Color c = (idx % 2 == 0) ? (Color){ 76, 160, 64, 255 } : (Color){ 68, 148, 58, 255 };
        DrawRectangle(x, CAMPO_TOPO - 6, faixa, CAMPO_BASE - CAMPO_TOPO + 12, c);
    }
    /* linhas laterais e marcações que passam (sensação de velocidade) */
    DrawRectangle(0, CAMPO_TOPO - 6, TELA_W, 2, (Color){ 240, 240, 240, 220 });
    DrawRectangle(0, CAMPO_BASE + 4, TELA_W, 2, (Color){ 240, 240, 240, 220 });
    int marca = (int)fmodf(deslocamento, 400.0f);
    if (deslocamento > 0) {
        DrawRectangle(TELA_W - marca, CAMPO_TOPO - 6, 2, CAMPO_BASE - CAMPO_TOPO + 12, (Color){ 240, 240, 240, 160 });
        DrawCircleLines(TELA_W - marca, (CAMPO_TOPO + CAMPO_BASE) / 2, 30, (Color){ 240, 240, 240, 160 });
    }

    /* arquibancadas com efeito de paralaxe */
    int w = R.arquibancada.width;
    int offA = w > 0 ? (int)fmodf(deslocamento * 0.4f, (float)w) : 0;
    for (int x = -offA; x < TELA_W; x += w) {
        DrawTexture(R.arquibancada, x, 0, WHITE);
        DrawTexture(R.arquibancada, x, CAMPO_BASE + 6, WHITE);
    }
    DrawRectangle(0, CAMPO_TOPO - 8, TELA_W, 2, (Color){ 40, 40, 48, 255 });
}

void Interface_DesenharHUD(int fase, int pontos, int moedas, int vidas, int vidasMax, int combo, float progresso)
{
    char txt[64];
    DrawRectangle(0, 0, TELA_W, 16, (Color){ 15, 20, 30, 210 });
    snprintf(txt, sizeof(txt), "FASE %d/%d", fase, TOTAL_FASES);
    Texto(txt, 6, 3, 10, (Color){ 140, 230, 140, 255 });
    snprintf(txt, sizeof(txt), "PONTOS %06d", pontos);
    Texto(txt, 80, 3, 10, WHITE);
    DrawTextureRec(R.moeda, (Rectangle){ 0, 0, 8, 8 }, (Vector2){ 190, 4 }, WHITE);
    snprintf(txt, sizeof(txt), "%d", moedas);
    Texto(txt, 201, 3, 10, (Color){ 255, 214, 0, 255 });
    for (int i = 0; i < vidasMax; i++)
        DrawTexture(R.bola, 236 + i * 10, 4, i < vidas ? WHITE : (Color){ 80, 80, 80, 160 });
    if (combo > 1) {
        snprintf(txt, sizeof(txt), "COMBO x%d", combo);
        Texto(txt, 300, 3, 10, (Color){ 255, 160, 40, 255 });
    }
    /* barra de distância até o gol */
    Texto("GOL", TELA_W - 26, 3, 10, WHITE);
    DrawRectangle(370, 5, 80, 6, (Color){ 60, 60, 70, 255 });
    DrawRectangle(370, 5, (int)(80 * (progresso > 1 ? 1 : progresso)), 6, (Color){ 140, 230, 140, 255 });
}

/* ---- avisos flutuantes ("Drible! +200") ---- */
#define MAX_AVISOS 6
static struct { char t[40]; Color c; float tempo; } avisos[MAX_AVISOS];

void Interface_NovoAviso(const char *t, Color c)
{
    for (int i = MAX_AVISOS - 1; i > 0; i--) avisos[i] = avisos[i - 1];
    snprintf(avisos[0].t, sizeof(avisos[0].t), "%s", t);
    avisos[0].c = c;
    avisos[0].tempo = 1.2f;
}

void Interface_Avisos(float dt)
{
    for (int i = 0; i < MAX_AVISOS; i++) if (avisos[i].tempo > 0) avisos[i].tempo -= dt;
}

void Interface_LimparAvisos(void) { memset(avisos, 0, sizeof(avisos)); }

void Interface_DesenharAvisos(void)
{
    for (int i = 0; i < MAX_AVISOS; i++) {
        if (avisos[i].tempo <= 0) continue;
        float a = avisos[i].tempo > 0.3f ? 1.0f : avisos[i].tempo / 0.3f;
        int y = 48 + i * 14 - (int)((1.2f - avisos[i].tempo) * 10);
        TextoSombra(avisos[i].t, TELA_W / 2 + 60, y, 10, Fade(avisos[i].c, a));
    }
}
