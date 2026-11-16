@echo off
REM Driblando Zagueiros - compilacao no Windows
REM Requisito: raylib 5.5 instalado pelo instalador oficial (cria C:\raylib com o gcc incluso).
REM Uso: de dois cliques neste arquivo.
set RAYLIB=C:\raylib
set PATH=%RAYLIB%\w64devkit\bin;%PATH%

gcc -std=c99 -Wall -O2 -Iinclude -I%RAYLIB%\raylib\src ^
  main.c src\adversarios.c src\controles.c src\finalizacao.c src\interface.c src\jogador.c src\jogo.c src\loja.c src\personagens.c src\recorde.c src\recursos.c ^
  -o DriblandoZagueiros.exe -L%RAYLIB%\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -static

if %ERRORLEVEL% NEQ 0 (
  echo.
  echo ERRO na compilacao. Verifique se o raylib esta instalado em %RAYLIB%.
  pause
  exit /b 1
)
echo.
echo Compilado com sucesso: DriblandoZagueiros.exe
pause
