#include <math.h>
#include "adversarios.h"
#include "recursos.h"

#define ALTURA_MIN (CAMPO_TOPO - 6)
#define ALTURA_MAX (CAMPO_BASE - 24)

void Adversarios_Limpar(Adversarios *a)
{
    *a = (Adversarios){ 0 };
    a->tempoProximo = 1.0f;
    a->tempoMoeda = 2.0f;
}

int Adversarios_Ativos(const Adversarios *a)
{
    int n = 0;
    for (int i = 0; i < MAX_ZAGUEIROS; i++) n += a->zagueiros[i].ativo;
    return n;
}

static float Aleatorio(float min, float max)
{
    return min + (max - min) * (float)GetRandomValue(0, 1000) / 1000.0f;
}

static TipoZagueiro SortearTipo(int fase)
{
    int r = GetRandomValue(1, 100);
    if (fase >= 3 && r <= 25) return ZAG_CARRINHO;
    if (fase >= 2 && r <= 55) return ZAG_MARCADOR;
    return ZAG_NORMAL;
}

static void CriarZagueiro(Adversarios *a, int fase)
{
    for (int i = 0; i < MAX_ZAGUEIROS; i++) {
        Zagueiro *z = &a->zagueiros[i];
        if (z->ativo) continue;
        *z = (Zagueiro){ 0 };
        z->ativo = true;
        z->tipo = SortearTipo(fase);
        z->pos = (Vector2){ TELA_W + 4, Aleatorio(ALTURA_MIN, ALTURA_MAX) };
        z->vel = Aleatorio(15, 45) + 10.0f * fase;
        z->menorDist = 9999;
        z->anim = Aleatorio(0, 3);
        return;
    }
}

static void CriarMoeda(Adversarios *a)
{
    for (int i = 0; i < MAX_MOEDAS; i++) {
        if (a->moedas[i].ativo) continue;
        a->moedas[i] = (Moeda){ true, { TELA_W + 4, Aleatorio(CAMPO_TOPO + 4, CAMPO_BASE - 14) }, 0 };
        return;
    }
}

static Rectangle HitboxZagueiro(const Zagueiro *z)
{
    return (Rectangle){ z->pos.x + 3, z->pos.y + 12, 10, 11 };
}

EventosCampo Adversarios_Atualizar(Adversarios *a, Jogador *j, int fase, float rolagem, float dt, bool gerarNovos)
{
    EventosCampo ev = { 0 };

    /* ---- surgimento: intervalo diminui a cada fase ---- */
    if (gerarNovos) {
        a->tempoProximo -= dt;
        if (a->tempoProximo <= 0) {
            CriarZagueiro(a, fase);
            float base = 1.25f - 0.15f * fase;
            if (base < 0.45f) base = 0.45f;
            a->tempoProximo = Aleatorio(base * 0.7f, base * 1.3f);
        }
        a->tempoMoeda -= dt;
        if (a->tempoMoeda <= 0) { CriarMoeda(a); a->tempoMoeda = Aleatorio(2.0f, 3.5f); }
    }

    Rectangle corpo = Jogador_Hitbox(j);

    /* ---- zagueiros ---- */
    for (int i = 0; i < MAX_ZAGUEIROS; i++) {
        Zagueiro *z = &a->zagueiros[i];
        if (!z->ativo) continue;
        z->anim += dt * 9.0f;
        float vx = rolagem + z->vel;

        switch (z->tipo) {
            case ZAG_NORMAL: break;
            case ZAG_MARCADOR: {
                if (z->pos.x > j->pos.x) {
                    float alvo = j->pos.y - z->pos.y;
                    float passo = (30.0f + 8.0f * fase) * dt;
                    z->pos.y += fabsf(alvo) < passo ? alvo : (alvo > 0 ? passo : -passo);
                }
                break;
            }
            case ZAG_CARRINHO:
                if (!z->aviso && !z->deslizando && z->pos.x - j->pos.x < 130 && z->pos.x > j->pos.x) {
                    z->aviso = true;            
                    z->timer = 0.45f;
                } else if (z->aviso) {
                    vx *= 0.4f;                 
                    z->timer -= dt;
                    if (z->timer <= 0) {
                        z->aviso = false;
                        z->deslizando = true;
                        z->vel += 150;
                        z->vy = (j->pos.y > z->pos.y ? 1.0f : -1.0f) * 70.0f;
                    }
                }
                if (z->deslizando) z->pos.y += z->vy * dt;
                break;
        }
        z->pos.x -= vx * dt;
        if (z->pos.y < ALTURA_MIN) z->pos.y = ALTURA_MIN;
        if (z->pos.y > ALTURA_MAX) z->pos.y = ALTURA_MAX;

        if (fabsf(z->pos.x - j->pos.x) < 18) {
            float d = fabsf(z->pos.y - j->pos.y);
            if (d < z->menorDist) z->menorDist = d;
        }

        if (j->invencivel <= 0 && CheckCollisionRecs(corpo, HitboxZagueiro(z))) {
            ev.perdeuBola = true;
            z->ativo = false;
            continue;
        }

        if (!z->passou && z->pos.x + 14 < j->pos.x) {
            z->passou = true;
            if (z->menorDist < 30) ev.dribles++;
        }
        if (z->pos.x < -24) z->ativo = false;
    }

    /* ---- moedas ---- */
    Rectangle corpoInteiro = { j->pos.x + 2, j->pos.y + 4, 14, 20 };
    for (int i = 0; i < MAX_MOEDAS; i++) {
        Moeda *m = &a->moedas[i];
        if (!m->ativo) continue;
        m->anim += dt * 8.0f;
        m->pos.x -= rolagem * dt;
        if (CheckCollisionRecs(corpoInteiro, (Rectangle){ m->pos.x, m->pos.y, 8, 8 })) {
            m->ativo = false;
            ev.moedas++;
        } else if (m->pos.x < -10) m->ativo = false;
    }
    return ev;
}

void Adversarios_Desenhar(const Adversarios *a)
{
    for (int i = 0; i < MAX_MOEDAS; i++) {
        const Moeda *m = &a->moedas[i];
        if (!m->ativo) continue;
        Rectangle src = { (float)(((int)m->anim % 4) * 8), 0, 8, 8 };
        DrawTextureRec(R.moeda, src, m->pos, WHITE);
    }
    for (int faixa = 0; faixa < 2; faixa++) {
        for (int i = 0; i < MAX_ZAGUEIROS; i++) {
            const Zagueiro *z = &a->zagueiros[i];
            if (!z->ativo) continue;
            Color cor = WHITE;
            if (z->tipo == ZAG_MARCADOR) cor = (Color){ 190, 200, 255, 255 };
            if (z->tipo == ZAG_CARRINHO) cor = (Color){ 255, 190, 150, 255 };
            if (faixa == 0) {
                if (z->deslizando) {
                    Rectangle src = { 0, 0, 16, 24 };
                    Rectangle dst = { z->pos.x + 8, z->pos.y + 18, 16, 24 };
                    DrawTexturePro(R.zagueiro, src, dst, (Vector2){ 8, 12 }, z->vy > 0 ? -70.0f : -110.0f, cor);
                } else {
                    Rectangle src = { (float)((1 + (int)z->anim % 2) * 16), 0, 16, 24 };
                    DrawTextureRec(R.zagueiro, src, z->pos, cor);
                }
            } else if (z->aviso) {
                DrawRectangle((int)z->pos.x + 5, (int)z->pos.y - 12, 7, 10, (Color){ 230, 40, 40, 255 });
                DrawText("!", (int)z->pos.x + 7, (int)z->pos.y - 11, 10, WHITE);
            }
        }
    }
}
