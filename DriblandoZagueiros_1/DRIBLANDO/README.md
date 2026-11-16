# Driblando Zagueiros

Jogo 2D de futebol feito em **C com a biblioteca Raylib 5.5**.
Você é um atacante que conduz a bola pelo campo, dribla os zagueiros e, ao chegar na área, precisa fazer o gol.
O campeonato tem **5 fases**. Vença todas para ser **campeão**. Se perder todas as vidas, é **fim de jogo**.

## Como executar

Dê dois cliques em **`DriblandoZagueiros.exe`**. A pasta `assets/` precisa estar ao lado do executável.
Não é preciso instalar o Raylib para jogar, porque ele já está embutido no `.exe`.

## Controles

| Ação | Teclado | Gamepad |
|---|---|---|
| Mover | Setas ou WASD | Analógico esquerdo / direcional |
| Chutar / confirmar | Espaço ou Enter | A |
| Pausar | P ou Esc | Start |
| Voltar | Backspace | B |
| Tela cheia | F11 | |

## Craques

Antes da partida você escolhe um entre três jogadores fictícios. Cada um começa com 3 níveis de bônus distribuídos de forma diferente, que se somam às melhorias compradas na loja:

| Craque | Estilo | Bônus inicial |
|---|---|---|
| Ziguezague | Driblador | Drible +2, Velocidade +1 |
| Canhão | Artilheiro | Chute +2, Fôlego +1 |
| Foguete | Velocista | Velocidade +2, Chute +1 |

Para mudar nomes ou atributos, edite só a tabela em `src/personagens.c`. Os sprites ficam em `assets/sprites/atacante_1.png` a `atacante_3.png`.

## Regras

- **Objetivo:** atravessar o campo (a barra "GOL" no topo mostra a distância) e marcar o gol na finalização.
- **Drible:** passar raspando por um zagueiro sem encostar nele. Dribles seguidos aumentam o **combo**, que multiplica os pontos (até x10).
- **Perder a bola:** encostar num zagueiro custa 1 vida e zera o combo.
- **Zagueiros:** o **azul** corre reto. O **azul-claro** marca você. O **laranja** mostra um "!" e dá um carrinho em diagonal.
- **Finalização:** posicione-se (cima/baixo) e chute com Espaço quando a mira estiver boa. São 3 chances. Se errar as 3, perde 1 vida e tenta de novo.
- **Moedas:** são coletadas no campo e compram melhorias no **Centro de Treinamento**, entre as fases.

| Melhoria | Efeito | Custo por nível |
|---|---|---|
| Velocidade | corre mais rápido | 4 / 7 / 10 moedas |
| Drible | área de contato menor | 4 / 7 / 10 |
| Chute | chute mais forte e mira mais lenta | 4 / 7 / 10 |
| Fôlego | +1 vida por fase | 4 / 7 / 10 |

**Pontuação:** 1 ponto por metro percorrido, 100 × combo por drible, 25 por moeda, 1000 × fase por gol e 200 por vida restante ao fim da fase.
O **recorde** fica salvo no arquivo `recorde.dat`.

A cada fase os zagueiros ficam mais rápidos e frequentes, novos tipos aparecem (marcador a partir da fase 2, carrinho a partir da fase 3), o goleiro melhora e o campo fica mais longo.

## Como compilar

**Windows:** instale o Raylib 5.5 pelo instalador oficial (raylib.com → *raylib_installer*), que cria `C:\raylib` com o compilador incluso.
Depois, dê dois cliques em **`build.bat`**.

**Com make** (terminal w64devkit ou Linux): `make`

## Organização do código

```
main.c              janela, loop principal e ampliação da tela virtual (480x270)
include/config.h    constantes de regras, pontuação e balanceamento
src/jogo.c          máquina de estados: menu, escolha do craque, corrida, finalização, loja, pausa, fim de jogo e vitória
src/jogador.c       movimento, vidas, hitbox e níveis (bônus do craque + melhorias)
src/personagens.c   tabela dos três craques selecionáveis
src/adversarios.c   zagueiros (3 tipos), moedas, colisões e detecção de dribles
src/finalizacao.c   minigame de chute a gol contra o goleiro
src/loja.c          centro de treinamento (melhorias com moedas)
src/interface.c     campo, HUD, textos, painéis e avisos
src/controles.c     teclado e gamepad num só lugar (e roteiros de teste automático)
src/recursos.c      carregamento de imagens e sons
src/recorde.c       leitura e gravação do recorde em arquivo
tools/gerar_assets.py   script que gerou todos os sprites e sons
```

Principais conceitos aplicados: `struct` e `enum`, vetores de structs (zagueiros e moedas), máquina de estados com `switch`, detecção de colisão entre retângulos, números aleatórios, leitura e escrita de arquivo e separação em módulos (`.h`/`.c`).

## Créditos

- **Programação:** Matheus Silva.
- **Biblioteca:** [Raylib](https://www.raylib.com) 5.5, de Ramon Santamaria e colaboradores, licença zlib/libpng.
- **Sprites e sons:** autoria própria. Foram gerados por código com o script `tools/gerar_assets.py` (desenho pixel a pixel com Pillow e síntese de áudio em WAV) e não usam material de terceiros.
- **Fonte:** fonte padrão embutida no Raylib.
