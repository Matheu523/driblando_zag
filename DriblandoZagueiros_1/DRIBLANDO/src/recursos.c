/*
 * recursos.c - carrega texturas e sons. Se o áudio não estiver disponível,
 * o jogo continua funcionando em silêncio.
 */
#include "recursos.h"

Recursos R;

void Recursos_Carregar(void)
{
    for (int i = 0; i < 3; i++) R.atacantes[i] = LoadTexture(TextFormat("assets/sprites/atacante_%d.png", i + 1));
    R.zagueiro     = LoadTexture("assets/sprites/zagueiro.png");
    R.goleiro      = LoadTexture("assets/sprites/goleiro.png");
    R.bola         = LoadTexture("assets/sprites/bola.png");
    R.moeda        = LoadTexture("assets/sprites/moeda.png");
    R.gol          = LoadTexture("assets/sprites/gol.png");
    R.arquibancada = LoadTexture("assets/sprites/arquibancada.png");

    InitAudioDevice();
    R.audio = IsAudioDeviceReady();
    if (!R.audio) return;
    R.somChute  = LoadSound("assets/sons/chute.wav");
    R.somMoeda  = LoadSound("assets/sons/moeda.wav");
    R.somDrible = LoadSound("assets/sons/drible.wav");
    R.somFalta  = LoadSound("assets/sons/falta.wav");
    R.somApito  = LoadSound("assets/sons/apito.wav");
    R.somGol    = LoadSound("assets/sons/gol.wav");
    R.somMenu   = LoadSound("assets/sons/menu.wav");
    R.torcida   = LoadMusicStream("assets/sons/torcida.wav");
    R.torcida.looping = true;
    SetMusicVolume(R.torcida, 0.35f);
    PlayMusicStream(R.torcida);
}

void Tocar(Sound s)
{
    if (R.audio && s.frameCount > 0) PlaySound(s);
}

void Recursos_Liberar(void)
{
    for (int i = 0; i < 3; i++) { UnloadTexture(R.atacantes[i]); }
    UnloadTexture(R.zagueiro); UnloadTexture(R.goleiro);
    UnloadTexture(R.bola); UnloadTexture(R.moeda); UnloadTexture(R.gol); UnloadTexture(R.arquibancada);
    if (R.audio) {
        UnloadSound(R.somChute); UnloadSound(R.somMoeda); UnloadSound(R.somDrible); UnloadSound(R.somFalta);
        UnloadSound(R.somApito); UnloadSound(R.somGol); UnloadSound(R.somMenu);
        UnloadMusicStream(R.torcida);
        CloseAudioDevice();
    }
}
