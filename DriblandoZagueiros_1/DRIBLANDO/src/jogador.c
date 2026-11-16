/*
 * jogador.c - movimento livre do atacante dentro da sua metade do campo.
 */
#include "jogador.h"
#include "config.h"
#include "controles.h"
#include "recursos.h"
#include "personagens.h"

void Jogador_Novo(Jogador *j, int personagem)
{
    *j = (Jogador){ 0 };
    j->personagem = personagem;
    Jogador_IniciarFase(j);
}

int Jogador_NivelVelocidade(const Jogador *j) { return Personagem_Obter(j->personagem)->velocidade + j->nivelVelocidade; }
int Jogador_NivelDrible(const Jogador *j)     { return Personagem_Obter(j->personagem)->drible + j->nivelDrible; }
int Jogador_NivelChute(const Jogador *j)      { return Personagem_Obter(j->personagem)->chute + j->nivelChute; }
int Jogador_NivelFolego(const Jogador *j)     { return Personagem_Obter(j->personagem)->folego + j->nivelFolego; }

int Jogador_VidasMaximas(const Jogador *j) { return VIDAS_BASE + Jogador_NivelFolego(j); }

/* Velocidade em pixels por segundo: cada nível de melhoria soma 18 */
float Jogador_Velocidade(const Jogador *j) { return 110.0f + 18.0f * Jogador_NivelVelocidade(j); }

void Jogador_IniciarFase(Jogador *j)
{
    j->pos = (Vector2){ 60, (CAMPO_TOPO + CAMPO_BASE) / 2.0f - 12 };
    j->vidas = Jogador_VidasMaximas(j);
    j->invencivel = 1.0f;
}

void Jogador_Atualizar(Jogador *j, float dt)
{
    float v = Jogador_Velocidade(j) * dt;
    if (Controle_Segurando(CMD_CIMA))     j->pos.y -= v;
    if (Controle_Segurando(CMD_BAIXO))    j->pos.y += v;
    if (Controle_Segurando(CMD_ESQUERDA)) j->pos.x -= v;
    if (Controle_Segurando(CMD_DIREITA))  j->pos.x += v;

    /* mantém o atacante dentro do gramado */
    if (j->pos.y < CAMPO_TOPO - 8) j->pos.y = CAMPO_TOPO - 8;
    if (j->pos.y > CAMPO_BASE - 24) j->pos.y = CAMPO_BASE - 24;
    if (j->pos.x < JOGADOR_X_MIN) j->pos.x = JOGADOR_X_MIN;
    if (j->pos.x > JOGADOR_X_MAX) j->pos.x = JOGADOR_X_MAX;

    if (j->invencivel > 0) j->invencivel -= dt;

    /* animação de corrida contínua (o campo sempre está rolando) */
    j->anim += dt * 10.0f;
    j->quadro = 1 + ((int)j->anim % 2);
}

Rectangle Jogador_Hitbox(const Jogador *j)
{
    /* área dos pés; a melhoria de Drible deixa a área menor (mais fácil escapar) */
    float encolhe = (float)Jogador_NivelDrible(j);
    return (Rectangle){ j->pos.x + 3 + encolhe / 2, j->pos.y + 12 + encolhe, 10 - encolhe, 11 - encolhe };
}

void Jogador_Desenhar(const Jogador *j)
{
    /* pisca enquanto está invencível */
    if (j->invencivel > 0 && ((int)(j->invencivel * 12) % 2 == 0)) return;
    Jogador_DesenharSprite(j->personagem, j->pos, j->quadro, 1);
    /* bola nos pés, quicando um pouco */
    float quique = (j->quadro == 1) ? 0 : 1;
    DrawTexture(R.bola, (int)j->pos.x + 13, (int)(j->pos.y + 16 + quique), WHITE);
}

void Jogador_DesenharSprite(int personagem, Vector2 pos, int quadro, float escala)
{
    Rectangle src = { (float)(quadro * 16), 0, 16, 24 };
    Rectangle dst = { pos.x, pos.y, 16 * escala, 24 * escala };
    DrawTexturePro(R.atacantes[personagem % TOTAL_PERSONAGENS], src, dst, (Vector2){ 0, 0 }, 0, WHITE);
}
