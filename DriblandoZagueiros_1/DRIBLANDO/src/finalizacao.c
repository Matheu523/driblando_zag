/*
 * finalizacao.c - minigame de chute a gol que encerra cada fase.
 */
#include <math.h>
#include <stdio.h>
#include "finalizacao.h"
#include "config.h"
#include "controles.h"
#include "recursos.h"
#include "interface.h"

#define GOL_X        446      /* posição do sprite da trave */
#define GOL_Y        87
#define LINHA_GOL    450.0f
#define BOCA_TOPO    93.0f    /* área útil entre as traves */
#define BOCA_BASE    176.0f
#define ATACANTE_X   210.0f
#define GOLEIRO_X    426.0f

void Finalizacao_Iniciar(Finalizacao *f, int fase)
{
    *f = (Finalizacao){ 0 };
    f->fase = fase;
    f->atacanteY = 135;
    f->direcaoAngulo = 1;
    f->goleiroY = 135;
    f->goleiroVel = 40.0f + 16.0f * fase;
    f->chancesRestantes = CHANCES_CHUTE;
    f->estado = FIN_MIRANDO;
}

static void MoverGoleiro(Finalizacao *f, float dt)
{
    if (f->estado == FIN_BOLA_VIAJANDO) {
        /* reage à bola depois de um pequeno atraso, com reflexo limitado */
        if (f->timer < 0.25f) { f->timer += dt; return; }
        float reflexo = (30.0f + 10.0f * f->fase) * dt;
        float alvo = f->bola.y - f->goleiroY;
        f->goleiroY += fabsf(alvo) < reflexo ? alvo : (alvo > 0 ? reflexo : -reflexo);
    } else {
        /* vai e volta embaixo das traves */
        f->goleiroY += f->goleiroVel * dt;
        if (f->goleiroY < BOCA_TOPO + 8) { f->goleiroY = BOCA_TOPO + 8; f->goleiroVel = fabsf(f->goleiroVel); }
        if (f->goleiroY > BOCA_BASE - 8) { f->goleiroY = BOCA_BASE - 8; f->goleiroVel = -fabsf(f->goleiroVel); }
    }
}

ResultadoChute Finalizacao_Atualizar(Finalizacao *f, const Jogador *j, float dt)
{
    MoverGoleiro(f, dt);

    if (f->estado == FIN_MIRANDO) {
        /* posicionamento */
        if (Controle_Segurando(CMD_CIMA))  f->atacanteY -= 90 * dt;
        if (Controle_Segurando(CMD_BAIXO)) f->atacanteY += 90 * dt;
        if (f->atacanteY < 70) f->atacanteY = 70;
        if (f->atacanteY > 200) f->atacanteY = 200;
        /* mira oscilando; a melhoria de Chute deixa a mira mais lenta (mais precisa) */
        float velMira = 50.0f - 7.0f * Jogador_NivelChute(j);
        f->angulo += f->direcaoAngulo * velMira * dt;
        if (f->angulo > 20)  { f->angulo = 20;  f->direcaoAngulo = -1; }
        if (f->angulo < -20) { f->angulo = -20; f->direcaoAngulo = 1; }

        if (Controle_Apertou(CMD_CONFIRMAR)) {
            float forca = 280.0f + 55.0f * Jogador_NivelChute(j);
            f->timer = 0;                       /* conta o atraso de reação do goleiro */
            float rad = f->angulo * DEG2RAD;
            f->bola = (Vector2){ ATACANTE_X + 14, f->atacanteY };
            f->velBola = (Vector2){ cosf(rad) * forca, sinf(rad) * forca };
            f->estado = FIN_BOLA_VIAJANDO;
            Tocar(R.somChute);
        }
        return RES_NENHUM;
    }

    if (f->estado == FIN_BOLA_VIAJANDO) {
        f->bola.x += f->velBola.x * dt;
        f->bola.y += f->velBola.y * dt;
        Rectangle bola = { f->bola.x - 4, f->bola.y - 4, 8, 8 };
        Rectangle goleiro = { GOLEIRO_X + 2, f->goleiroY - 12, 12, 24 };
        ResultadoChute r = RES_NENHUM;
        if (CheckCollisionRecs(bola, goleiro)) r = RES_DEFESA;
        else if (f->bola.x >= LINHA_GOL) r = (f->bola.y > BOCA_TOPO && f->bola.y < BOCA_BASE) ? RES_GOL : RES_FORA;
        else if (f->bola.y < CAMPO_TOPO || f->bola.y > CAMPO_BASE) r = RES_FORA;
        if (r != RES_NENHUM) {
            f->ultimo = r;
            f->estado = FIN_RESULTADO;
            f->timer = r == RES_GOL ? 2.2f : 1.2f;
            if (r == RES_GOL) Tocar(R.somGol); else Tocar(R.somFalta);
        }
        return RES_NENHUM;
    }

    /* FIN_RESULTADO: mostra a mensagem por um tempo e então informa o jogo */
    f->timer -= dt;
    if (f->timer > 0) return RES_NENHUM;
    if (f->ultimo == RES_GOL) return RES_GOL;
    f->chancesRestantes--;
    f->estado = FIN_MIRANDO;
    return f->ultimo;
}

void Finalizacao_Desenhar(const Finalizacao *f, const Jogador *j)
{
    Interface_DesenharCampo(0);
    /* grande área */
    DrawRectangleLines(370, 60, 120, 150, (Color){ 255, 255, 255, 200 });
    DrawRectangleLines(420, 95, 70, 80, (Color){ 255, 255, 255, 200 });
    DrawCircle(395, 135, 2, WHITE);

    DrawTexture(R.gol, GOL_X, GOL_Y, WHITE);
    Rectangle src = { 16, 0, 16, 24 };
    DrawTextureRec(R.goleiro, src, (Vector2){ GOLEIRO_X, f->goleiroY - 12 }, WHITE);

    Jogador_DesenharSprite(j->personagem, (Vector2){ ATACANTE_X - 8, f->atacanteY - 20 }, 0, 1);

    if (f->estado == FIN_MIRANDO) {
        DrawTexture(R.bola, (int)ATACANTE_X + 6, (int)f->atacanteY - 4, WHITE);
        /* seta de mira tracejada */
        float rad = f->angulo * DEG2RAD;
        for (int i = 2; i < 9; i++) {
            float d = i * 9.0f;
            DrawCircle((int)(ATACANTE_X + 10 + cosf(rad) * d), (int)(f->atacanteY + sinf(rad) * d), 1.5f, (Color){ 255, 235, 59, 255 });
        }
    } else if (f->estado == FIN_BOLA_VIAJANDO || f->ultimo != RES_DEFESA) {
        DrawTexture(R.bola, (int)f->bola.x - 4, (int)f->bola.y - 4, WHITE);
    }

    char txt[48];
    snprintf(txt, sizeof(txt), "Chances: %d", f->chancesRestantes);
    Texto(txt, 12, 42, 10, WHITE);
    if (f->estado == FIN_MIRANDO)
        TextoSombra("Cima/Baixo: posicionar   Espaço: chutar", TELA_W / 2, 240, 10, WHITE);

    if (f->estado == FIN_RESULTADO) {
        const char *msg = f->ultimo == RES_GOL ? "GOOOOL!" : (f->ultimo == RES_DEFESA ? "DEFENDEU!" : "PRA FORA!");
        Color c = f->ultimo == RES_GOL ? (Color){ 255, 214, 0, 255 } : (Color){ 255, 90, 90, 255 };
        TextoSombra(msg, TELA_W / 2, 110, 40, c);
    }
}
