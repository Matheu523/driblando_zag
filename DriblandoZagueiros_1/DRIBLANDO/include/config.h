/*
 * config.h - constantes do jogo (tamanho da tela, campo, regras e balanceamento).
 * Ajustar a dificuldade é só mudar os valores aqui.
 */
#ifndef CONFIG_H
#define CONFIG_H

#define TITULO          "Driblando Zagueiros"

/* Tela virtual (pixel art) ampliada para a janela */
#define TELA_W          480
#define TELA_H          270
#define ESCALA_JANELA   2

/* Limites do gramado (entre as arquibancadas) */
#define CAMPO_TOPO      36
#define CAMPO_BASE      258
#define JOGADOR_X_MIN   16
#define JOGADOR_X_MAX   260

/* Regras */
#define TOTAL_FASES     5      /* vencer as 5 fases = campeão */
#define VIDAS_BASE      3
#define CHANCES_CHUTE   3      /* chutes por tentativa de finalização */
#define MAX_ZAGUEIROS   24
#define MAX_MOEDAS      8
#define MAX_NIVEL_MELHORIA 3

/* Pontuação */
#define PONTOS_DRIBLE   100    /* multiplicado pelo combo */
#define PONTOS_MOEDA    25
#define PONTOS_GOL      1000   /* multiplicado pela fase */
#define PONTOS_VIDA_BONUS 200  /* por vida restante ao fim da fase */
#define COMBO_MAX       10

#endif
