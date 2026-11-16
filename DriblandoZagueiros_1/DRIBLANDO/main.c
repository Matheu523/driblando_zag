#include "raylib.h"
#include "config.h"
#include "controles.h"
#include "recursos.h"
#include "jogo.h"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(TELA_W * ESCALA_JANELA, TELA_H * ESCALA_JANELA, TITULO);
    SetWindowMinSize(TELA_W, TELA_H);
    SetExitKey(KEY_NULL);                      
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());

    /* tudo é desenhado em 480x270 e depois ampliado sem borrar os pixels */
    RenderTexture2D tela = LoadRenderTexture(TELA_W, TELA_H);
    SetTextureFilter(tela.texture, TEXTURE_FILTER_POINT);

    Recursos_Carregar();
    Controles_Iniciar();
    Jogo_Iniciar();

    while (!WindowShouldClose() && !Jogo_Sair() && !Controles_RoteiroTerminou()) {
        Controles_Atualizar();
        if (Controle_Apertou(CMD_TELA_CHEIA)) ToggleBorderlessWindowed();
        Jogo_Atualizar(Controles_DeltaTempo());

        BeginTextureMode(tela);
        Jogo_Desenhar();
        EndTextureMode();

        float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
        float escala = sw / TELA_W < sh / TELA_H ? sw / TELA_W : sh / TELA_H;
        if (escala >= 1.0f) escala = (float)(int)escala;
        Rectangle origem  = { 0, 0, TELA_W, -TELA_H };
        Rectangle destino = { (sw - TELA_W * escala) / 2, (sh - TELA_H * escala) / 2, TELA_W * escala, TELA_H * escala };

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(tela.texture, origem, destino, (Vector2){ 0, 0 }, 0, WHITE);
        EndDrawing();

        const char *captura = Controles_PedidoCaptura();
        if (captura) TakeScreenshot(TextFormat("%s.png", captura));
    }

    Jogo_Encerrar();
    Recursos_Liberar();
    UnloadRenderTexture(tela);
    CloseWindow();
    return 0;
}
