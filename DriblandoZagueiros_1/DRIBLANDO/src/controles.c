#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "controles.h"

static bool atual[CMD_TOTAL], anterior[CMD_TOTAL];

typedef struct { char cmd[8]; int acao; int quadros; char arg[48]; } Passo;
static Passo *roteiro = NULL;
static int totalPassos = 0, passo = 0, quadroPasso = 0;
static bool usandoRoteiro = false, roteiroFim = false;
static char captura[48];
static bool capturaPendente = false;

static int NomeComando(const char *s)
{
    const char *nomes[CMD_TOTAL] = { "cima", "baixo", "esquerda", "direita", "confirmar", "voltar", "pausa", "telacheia" };
    for (int i = 0; i < CMD_TOTAL; i++) if (strcmp(s, nomes[i]) == 0) return i;
    return -1;
}

void Controles_Iniciar(void)
{
    const char *caminho = getenv("DZ_ROTEIRO");
    if (!caminho) return;
    char *texto = LoadFileText(caminho);
    if (!texto) return;
    int cap = 512;
    roteiro = malloc(sizeof(Passo) * cap);
    for (char *linha = strtok(texto, "\r\n"); linha; linha = strtok(NULL, "\r\n")) {
        Passo p = { 0 };
        char a[32] = "", b[48] = "";
        int n = sscanf(linha, "%7s %31s %47s", p.cmd, a, b);
        if (n < 1 || p.cmd[0] == '#') continue;
        p.acao = -1; p.quadros = 1;
        if (!strcmp(p.cmd, "wait") && n >= 2) p.quadros = atoi(a);
        else if (!strcmp(p.cmd, "press") && n >= 2) { p.acao = NomeComando(a); p.quadros = 2; }
        else if (!strcmp(p.cmd, "hold") && n >= 3) { p.acao = NomeComando(a); p.quadros = atoi(b); }
        else if (!strcmp(p.cmd, "shot") && n >= 2) snprintf(p.arg, sizeof(p.arg), "%s", a);
        if (totalPassos == cap) { cap *= 2; roteiro = realloc(roteiro, sizeof(Passo) * cap); }
        roteiro[totalPassos++] = p;
    }
    UnloadFileText(texto);
    usandoRoteiro = true;
}

static void PassoRoteiro(bool *seg)
{
    capturaPendente = false;
    if (passo >= totalPassos) { roteiroFim = true; return; }
    Passo *p = &roteiro[passo];
    if (!strcmp(p->cmd, "press") && quadroPasso == 0 && p->acao >= 0) seg[p->acao] = true;
    else if (!strcmp(p->cmd, "hold") && p->acao >= 0) seg[p->acao] = true;
    else if (!strcmp(p->cmd, "shot")) { snprintf(captura, sizeof(captura), "%s", p->arg); capturaPendente = true; }
    else if (!strcmp(p->cmd, "quit")) roteiroFim = true;
    if (++quadroPasso >= p->quadros) { passo++; quadroPasso = 0; }
}

void Controles_Atualizar(void)
{
    memcpy(anterior, atual, sizeof(atual));
    memset(atual, 0, sizeof(atual));
    if (usandoRoteiro) { PassoRoteiro(atual); return; }

    float eixoX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float eixoY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    atual[CMD_CIMA]     = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)    || eixoY < -0.5f;
    atual[CMD_BAIXO]    = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)  || eixoY > 0.5f;
    atual[CMD_ESQUERDA] = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)  || eixoX < -0.5f;
    atual[CMD_DIREITA]  = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || eixoX > 0.5f;
    atual[CMD_CONFIRMAR]= IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_ENTER) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    atual[CMD_VOLTAR]   = IsKeyDown(KEY_BACKSPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
    atual[CMD_PAUSA]    = IsKeyDown(KEY_P) || IsKeyDown(KEY_ESCAPE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
    atual[CMD_TELA_CHEIA] = IsKeyDown(KEY_F11);
}

bool Controle_Segurando(Comando c) { return atual[c]; }
bool Controle_Apertou(Comando c)   { return atual[c] && !anterior[c]; }
bool Controles_RoteiroTerminou(void) { return roteiroFim; }
const char *Controles_PedidoCaptura(void) { return capturaPendente ? captura : NULL; }

float Controles_DeltaTempo(void)
{
    if (usandoRoteiro) return 1.0f / 60.0f; 
    float dt = GetFrameTime();
    return dt > 0.05f ? 0.05f : dt;
}
