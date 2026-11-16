#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "jogo.h"
#include "config.h"
#include "controles.h"
#include "recursos.h"
#include "interface.h"
#include "jogador.h"
#include "adversarios.h"
#include "finalizacao.h"
#include "loja.h"
#include "recorde.h"
#include "personagens.h"

/* ---- dados da partida ---- */
static struct {
    EstadoJogo estado, estadoAntesPausa;
    int   fase;
    int   pontos;
    float pontosDistancia;      
    int   moedas;
    int   combo, melhorCombo;
    float deslocamento;        
    float distancia, meta;    
    float esperaFinal;
    int   driblesFase, moedasFase, bonusFase;
    int   recorde;
    bool  novoRecorde;
    int   cursorMenu, cursorPausa;
    int   personagem;          
    float tempoTela;
    bool  sair;
} P;

static Jogador     jogador;
static Adversarios adv;
static Finalizacao fin;
static Loja        loja;


static float VelocidadeCampo(void) { return 105.0f + 18.0f * P.fase; }

static void IniciarFase(void)
{
    Jogador_IniciarFase(&jogador);
    Adversarios_Limpar(&adv);
    Interface_LimparAvisos();
    P.distancia = 0;
    P.meta = 300.0f + 100.0f * P.fase;   
#ifdef DZ_TESTE
    P.meta = 40;                            
#endif
    P.combo = 0;
    P.driblesFase = P.moedasFase = P.bonusFase = 0;
    P.esperaFinal = 0;
    P.estado = ESTADO_CORRIDA;
    Tocar(R.somApito);
}

static void NovaPartida(void)
{
    Jogador_Novo(&jogador, P.personagem);
    P.fase = 1;
    P.pontos = 0;
    P.pontosDistancia = 0;
    P.moedas = 0;
    P.melhorCombo = 0;
    P.novoRecorde = false;
    IniciarFase();
}

static void EncerrarPartida(EstadoJogo final)
{
    P.novoRecorde = Recorde_Salvar(P.pontos);
    P.recorde = Recorde_Ler();
    P.estado = final;
    P.tempoTela = 0;
    Tocar(final == ESTADO_VITORIA ? R.somGol : R.somApito);
}

static void PerderVida(const char *motivo)
{
    jogador.vidas--;
    jogador.invencivel = 1.5f;
    P.combo = 0;
    Tocar(R.somFalta);
    Interface_NovoAviso(motivo, (Color){ 255, 90, 90, 255 });
    if (jogador.vidas <= 0) EncerrarPartida(ESTADO_FIM_DE_JOGO);
}

void Jogo_Iniciar(void)
{
    P.estado = ESTADO_MENU;
    P.recorde = Recorde_Ler();
    P.fase = 1;
}

bool Jogo_Sair(void) { return P.sair; }
void Jogo_Encerrar(void) {}

static void AtualizarCorrida(float dt)
{
    if (Controle_Apertou(CMD_PAUSA)) { P.estadoAntesPausa = P.estado; P.cursorPausa = 0; P.estado = ESTADO_PAUSA; return; }

    float rolagem = VelocidadeCampo();
    bool chegou = P.distancia >= P.meta;
    P.deslocamento += rolagem * dt;
    if (!chegou) {
        P.distancia += rolagem * dt / 8.0f;
        P.pontosDistancia += rolagem * dt / 8.0f;
        int inteiros = (int)P.pontosDistancia;
        P.pontos += inteiros;
        P.pontosDistancia -= inteiros;
    }

    Jogador_Atualizar(&jogador, dt);
    EventosCampo ev = Adversarios_Atualizar(&adv, &jogador, P.fase, rolagem, dt, !chegou);

    for (int i = 0; i < ev.dribles; i++) {
        if (P.combo < COMBO_MAX) P.combo++;
        if (P.combo > P.melhorCombo) P.melhorCombo = P.combo;
        int ganho = PONTOS_DRIBLE * P.combo;
        P.pontos += ganho;
        P.driblesFase++;
        char txt[40];
        snprintf(txt, sizeof(txt), "DRIBLE! +%d", ganho);
        Interface_NovoAviso(txt, (Color){ 255, 214, 0, 255 });
        Tocar(R.somDrible);
    }
    for (int i = 0; i < ev.moedas; i++) {
        P.moedas++;
        P.moedasFase++;
        P.pontos += PONTOS_MOEDA;
        Tocar(R.somMoeda);
    }
#ifdef DZ_TESTE
    ev.perdeuBola = false;                  
#endif
    if (ev.perdeuBola) {
        PerderVida("PERDEU A BOLA!");
        if (P.estado != ESTADO_CORRIDA) return;
    }


    if (chegou) {
        P.esperaFinal += dt;
        if (Adversarios_Ativos(&adv) == 0 || P.esperaFinal > 3.0f) {
            Finalizacao_Iniciar(&fin, P.fase);
            P.estado = ESTADO_FINALIZACAO;
            Tocar(R.somApito);
        }
    }
}

static void AtualizarFinalizacao(float dt)
{
    if (Controle_Apertou(CMD_PAUSA)) { P.estadoAntesPausa = P.estado; P.cursorPausa = 0; P.estado = ESTADO_PAUSA; return; }
    ResultadoChute r = Finalizacao_Atualizar(&fin, &jogador, dt);
    if (r == RES_GOL) {
        int gol = PONTOS_GOL * P.fase;
        int bonus = jogador.vidas * PONTOS_VIDA_BONUS;
        P.pontos += gol + bonus;
        P.bonusFase = gol + bonus;
        P.estado = ESTADO_FASE_CONCLUIDA;
        P.tempoTela = 0;
    } else if (r != RES_NENHUM && fin.chancesRestantes <= 0) {
        /* errou todas as chances: perde uma vida e tenta de novo */
        PerderVida("SEM GOL... -1 VIDA");
        if (P.estado == ESTADO_FINALIZACAO) fin.chancesRestantes = CHANCES_CHUTE;
    }
}

static void AtualizarMenu(void)
{
    P.deslocamento += 40 * Controles_DeltaTempo();
    if (Controle_Apertou(CMD_CIMA))  { P.cursorMenu = (P.cursorMenu + 2) % 3; Tocar(R.somMenu); }
    if (Controle_Apertou(CMD_BAIXO)) { P.cursorMenu = (P.cursorMenu + 1) % 3; Tocar(R.somMenu); }
    if (Controle_Apertou(CMD_CONFIRMAR)) {
        if (P.cursorMenu == 0) { P.estado = ESTADO_ESCOLHA; P.tempoTela = 0; }
        else if (P.cursorMenu == 1) P.estado = ESTADO_COMO_JOGAR;
        else P.sair = true;
    }
}

static void AtualizarEscolha(void)
{
    if (Controle_Apertou(CMD_ESQUERDA)) { P.personagem = (P.personagem + TOTAL_PERSONAGENS - 1) % TOTAL_PERSONAGENS; Tocar(R.somMenu); }
    if (Controle_Apertou(CMD_DIREITA))  { P.personagem = (P.personagem + 1) % TOTAL_PERSONAGENS; Tocar(R.somMenu); }
    if (Controle_Apertou(CMD_VOLTAR) || Controle_Apertou(CMD_PAUSA)) { P.estado = ESTADO_MENU; return; }
    if (Controle_Apertou(CMD_CONFIRMAR)) {
        NovaPartida();
#ifdef DZ_TESTE
        P.moedas = 30;
        if (getenv("DZ_FASE")) { jogador.nivelChute = 3; P.fase = atoi(getenv("DZ_FASE")); IniciarFase(); }
#endif
    }
}

void Jogo_Atualizar(float dt)
{
    if (R.audio) UpdateMusicStream(R.torcida);
    Interface_Avisos(dt);
    P.tempoTela += dt;

    switch (P.estado) {
        case ESTADO_MENU: AtualizarMenu(); break;
        case ESTADO_COMO_JOGAR:
            if (Controle_Apertou(CMD_CONFIRMAR) || Controle_Apertou(CMD_VOLTAR) || Controle_Apertou(CMD_PAUSA)) P.estado = ESTADO_MENU;
            break;
        case ESTADO_ESCOLHA:     P.deslocamento += 40 * dt; AtualizarEscolha(); break;
        case ESTADO_CORRIDA:     AtualizarCorrida(dt); break;
        case ESTADO_FINALIZACAO: AtualizarFinalizacao(dt); break;
        case ESTADO_FASE_CONCLUIDA:
            if (P.tempoTela > 0.8f && Controle_Apertou(CMD_CONFIRMAR)) {
                if (P.fase >= TOTAL_FASES) EncerrarPartida(ESTADO_VITORIA);
                else { Loja_Abrir(&loja); P.estado = ESTADO_LOJA; }
            }
            break;
        case ESTADO_LOJA:
            if (Loja_Atualizar(&loja, &jogador, &P.moedas, dt)) { P.fase++; IniciarFase(); }
            break;
        case ESTADO_PAUSA:
            if (Controle_Apertou(CMD_CIMA) || Controle_Apertou(CMD_BAIXO)) { P.cursorPausa = !P.cursorPausa; Tocar(R.somMenu); }
            if (Controle_Apertou(CMD_PAUSA) || Controle_Apertou(CMD_VOLTAR) ||
                (Controle_Apertou(CMD_CONFIRMAR) && P.cursorPausa == 0)) P.estado = P.estadoAntesPausa;
            else if (Controle_Apertou(CMD_CONFIRMAR)) { P.estado = ESTADO_MENU; P.recorde = Recorde_Ler(); }
            break;
        case ESTADO_FIM_DE_JOGO:
        case ESTADO_VITORIA:
            if (P.tempoTela > 1.0f && Controle_Apertou(CMD_CONFIRMAR)) { P.estado = ESTADO_MENU; P.cursorMenu = 0; }
            break;
    }
}

/* ------------------------------------------------------------------ desenho */
static void DesenharPartida(void)
{
    if (P.estadoAntesPausa == ESTADO_FINALIZACAO && P.estado == ESTADO_PAUSA) Finalizacao_Desenhar(&fin, &jogador);
    else if (P.estado == ESTADO_FINALIZACAO || P.estado == ESTADO_FASE_CONCLUIDA) Finalizacao_Desenhar(&fin, &jogador);
    else {
        Interface_DesenharCampo(P.deslocamento);
        Adversarios_Desenhar(&adv);
        Jogador_Desenhar(&jogador);
        if (P.distancia >= P.meta) TextoSombra("CHEGOU NA ÁREA!", TELA_W / 2, 120, 20, (Color){ 255, 214, 0, 255 });
    }
    Interface_DesenharHUD(P.fase, P.pontos, P.moedas, jogador.vidas, Jogador_VidasMaximas(&jogador),
                          P.combo, P.meta > 0 ? P.distancia / P.meta : 0);
    Interface_DesenharAvisos();
}

static void DesenharMenu(void)
{
    Interface_DesenharCampo(P.deslocamento);
    DrawRectangle(0, 0, TELA_W, TELA_H, (Color){ 0, 30, 10, 120 });
    float bal = sinf((float)GetTime() * 2) * 3;
    TextoSombra("DRIBLANDO", TELA_W / 2, (int)(42 + bal), 40, (Color){ 255, 255, 255, 255 });
    TextoSombra("ZAGUEIROS", TELA_W / 2, (int)(82 + bal), 40, (Color){ 255, 214, 0, 255 });
    const char *itens[3] = { "Jogar", "Como jogar", "Sair" };
    Painel((Rectangle){ TELA_W / 2.0f - 60, 140, 120, 62 });
    for (int i = 0; i < 3; i++) {
        Color c = i == P.cursorMenu ? (Color){ 20, 120, 50, 255 } : DARKGRAY;
        if (i == P.cursorMenu) TextoCentro(">", TELA_W / 2 - 44, 148 + i * 17, 10, c);
        TextoCentro(itens[i], TELA_W / 2, 148 + i * 17, 10, c);
    }
    char txt[48];
    snprintf(txt, sizeof(txt), "RECORDE: %06d", P.recorde);
    TextoSombra(txt, TELA_W / 2, 214, 10, (Color){ 255, 214, 0, 255 });
    TextoSombra("Setas: escolher   Espaço/Enter: confirmar", TELA_W / 2, 250, 10, WHITE);
}

static void DesenharComoJogar(void)
{
    Interface_DesenharCampo(P.deslocamento);
    Painel((Rectangle){ 30, 16, TELA_W - 60, TELA_H - 32 });
    TextoCentro("COMO JOGAR", TELA_W / 2, 24, 20, (Color){ 30, 90, 50, 255 });
    const char *linhas[] = {
        "Escolha seu craque e conduza a bola até a área adversária.",
        "Setas/WASD (ou analógico): mover    P/Esc: pausa",
        "",
        "DRIBLE: passe RASPANDO por um zagueiro sem encostar.",
        "Dribles seguidos aumentam o COMBO e multiplicam os pontos.",
        "Encostou no zagueiro? Perde a bola (-1 vida).",
        "Zagueiro azul-claro marca você; laranja dá carrinho após o \"!\".",
        "",
        "Na área: posicione-se e aperte Espaço para chutar. 3 chances!",
        "Moedas compram melhorias entre as fases.",
        "Vença as 5 fases para ser CAMPEÃO. Sem vidas: fim de jogo.",
    };
    for (int i = 0; i < 11; i++) Texto(linhas[i], 46, 54 + i * 15, 10, DARKGRAY);
    TextoCentro("Espaço/Enter: voltar", TELA_W / 2, 230, 10, (Color){ 20, 120, 50, 255 });
}

static void DesenharEscolha(void)
{
    Interface_DesenharCampo(P.deslocamento);
    DrawRectangle(0, 0, TELA_W, TELA_H, (Color){ 0, 30, 10, 140 });
    TextoSombra("ESCOLHA SEU CRAQUE", TELA_W / 2, 12, 20, WHITE);
    const char *nomesAtr[4] = { "Velocidade", "Drible", "Chute", "Fôlego" };
    for (int i = 0; i < TOTAL_PERSONAGENS; i++) {
        const Personagem *p = Personagem_Obter(i);
        bool sel = i == P.personagem;
        Rectangle card = { 18.0f + i * 150, sel ? 38.0f : 44.0f, 144, 196 };
        Painel(card);
        if (sel) DrawRectangleLinesEx((Rectangle){ card.x - 2, card.y - 2, card.width + 4, card.height + 4 }, 2, (Color){ 255, 214, 0, 255 });
        int cx = (int)(card.x + card.width / 2);
        /* sprite ampliado 3x, correndo quando selecionado */
        int quadro = sel ? 1 + ((int)(P.tempoTela * 8) % 2) : 0;
        Jogador_DesenharSprite(i, (Vector2){ (float)cx - 24, card.y + 8 }, quadro, 3);
        TextoCentro(p->nome, cx, (int)card.y + 86, 20, sel ? (Color){ 20, 120, 50, 255 } : DARKGRAY);
        TextoCentro(p->estilo, cx, (int)card.y + 106, 10, (Color){ 180, 130, 0, 255 });
        int bonus[4] = { p->velocidade, p->drible, p->chute, p->folego };
        for (int a = 0; a < 4; a++) {
            int y = (int)card.y + 124 + a * 13;
            Texto(nomesAtr[a], (int)card.x + 10, y, 10, DARKGRAY);
            for (int k = 0; k < 3; k++)
                DrawRectangle((int)card.x + 90 + k * 14, y + 1, 10, 9, k < bonus[a] ? (Color){ 60, 120, 220, 255 } : LIGHTGRAY);
        }
        TextoCentro(p->descricao, cx, (int)card.y + 178, 10, GRAY);
    }
    TextoSombra("Esquerda/Direita: escolher   Espaço: jogar   Backspace: voltar", TELA_W / 2, 252, 10, WHITE);
}

static void DesenharFaseConcluida(void)
{
    DrawRectangle(0, 0, TELA_W, TELA_H, (Color){ 0, 0, 0, 110 });
    Painel((Rectangle){ 100, 40, TELA_W - 200, 190 });
    char txt[64];
    snprintf(txt, sizeof(txt), "FASE %d CONCLUÍDA!", P.fase);
    TextoCentro(txt, TELA_W / 2, 52, 20, (Color){ 30, 120, 50, 255 });
    int y = 88;
    snprintf(txt, sizeof(txt), "Dribles: %d", P.driblesFase);              TextoCentro(txt, TELA_W / 2, y, 10, DARKGRAY); y += 16;
    snprintf(txt, sizeof(txt), "Moedas coletadas: %d", P.moedasFase);     TextoCentro(txt, TELA_W / 2, y, 10, DARKGRAY); y += 16;
    snprintf(txt, sizeof(txt), "Gol + vidas restantes: +%d", P.bonusFase); TextoCentro(txt, TELA_W / 2, y, 10, DARKGRAY); y += 16;
    snprintf(txt, sizeof(txt), "PONTOS: %06d", P.pontos);                 TextoCentro(txt, TELA_W / 2, y + 6, 20, (Color){ 200, 140, 0, 255 });
    if (P.tempoTela > 0.8f)
        TextoCentro(P.fase >= TOTAL_FASES ? "Espaço: final do campeonato!" : "Espaço: ir para o treinamento",
                    TELA_W / 2, 204, 10, (Color){ 20, 120, 50, 255 });
}

static void DesenharFim(bool vitoria)
{
    Interface_DesenharCampo(P.deslocamento);
    DrawRectangle(0, 0, TELA_W, TELA_H, vitoria ? (Color){ 40, 30, 0, 150 } : (Color){ 30, 0, 0, 160 });
    if (vitoria) {
        /* confete simples */
        for (int i = 0; i < 60; i++) {
            float x = fmodf(i * 37.0f + P.tempoTela * (20 + i % 7 * 6), TELA_W);
            float y = fmodf(i * 53.0f + P.tempoTela * (40 + i % 5 * 15), TELA_H);
            Color cs[4] = { RED, GOLD, WHITE, GREEN };
            DrawRectangle((int)x, (int)y, 3, 3, cs[i % 4]);
        }
    }
    TextoSombra(vitoria ? "CAMPEÃO!" : "FIM DE JOGO", TELA_W / 2, 50, 40, vitoria ? (Color){ 255, 214, 0, 255 } : (Color){ 255, 90, 90, 255 });
    char txt[64];
    if (!vitoria) {
        snprintf(txt, sizeof(txt), "Você chegou até a fase %d de %d", P.fase, TOTAL_FASES);
        TextoSombra(txt, TELA_W / 2, 100, 10, WHITE);
    } else TextoSombra("Você venceu as 5 fases do campeonato!", TELA_W / 2, 100, 10, WHITE);
    snprintf(txt, sizeof(txt), "PONTOS: %06d", P.pontos);
    TextoSombra(txt, TELA_W / 2, 124, 20, WHITE);
    snprintf(txt, sizeof(txt), "Craque: %s   |   Melhor combo: x%d", Personagem_Obter(P.personagem)->nome, P.melhorCombo);
    TextoSombra(txt, TELA_W / 2, 152, 10, WHITE);
    if (P.novoRecorde) TextoSombra("NOVO RECORDE!", TELA_W / 2, 172, 20, (Color){ 255, 214, 0, 255 });
    else {
        snprintf(txt, sizeof(txt), "Recorde: %06d", P.recorde);
        TextoSombra(txt, TELA_W / 2, 176, 10, (Color){ 255, 214, 0, 255 });
    }
    if (P.tempoTela > 1.0f) TextoSombra("Espaço/Enter: menu inicial", TELA_W / 2, 230, 10, WHITE);
}

void Jogo_Desenhar(void)
{
    ClearBackground(BLACK);
    switch (P.estado) {
        case ESTADO_MENU:        DesenharMenu(); break;
        case ESTADO_COMO_JOGAR:  DesenharComoJogar(); break;
        case ESTADO_ESCOLHA:     DesenharEscolha(); break;
        case ESTADO_CORRIDA:
        case ESTADO_FINALIZACAO: DesenharPartida(); break;
        case ESTADO_FASE_CONCLUIDA: DesenharPartida(); DesenharFaseConcluida(); break;
        case ESTADO_LOJA:
            Interface_DesenharCampo(P.deslocamento);
            Loja_Desenhar(&loja, &jogador, P.moedas, P.fase + 1);
            break;
        case ESTADO_PAUSA:
            DesenharPartida();
            DrawRectangle(0, 0, TELA_W, TELA_H, (Color){ 0, 0, 0, 140 });
            TextoSombra("PAUSA", TELA_W / 2, 80, 30, WHITE);
            Painel((Rectangle){ TELA_W / 2.0f - 60, 120, 120, 44 });
            TextoCentro(P.cursorPausa == 0 ? "> Continuar" : "Continuar", TELA_W / 2, 128, 10, P.cursorPausa == 0 ? (Color){ 20, 120, 50, 255 } : DARKGRAY);
            TextoCentro(P.cursorPausa == 1 ? "> Menu inicial" : "Menu inicial", TELA_W / 2, 145, 10, P.cursorPausa == 1 ? (Color){ 20, 120, 50, 255 } : DARKGRAY);
            break;
        case ESTADO_FIM_DE_JOGO: DesenharFim(false); break;
        case ESTADO_VITORIA:     DesenharFim(true); break;
    }
}
