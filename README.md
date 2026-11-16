# ⚽ Driblando Zagueiros

> Jogo 2D de futebol em **C** com a biblioteca **[Raylib](https://www.raylib.com) 5.5**: escolha seu craque, drible os zagueiros, marque o gol e vença as 5 fases do campeonato.

![Menu inicial](docs/screenshots/menu.png)

---

## Índice

1. [Sobre o jogo](#sobre-o-jogo)
2. [Como executar](#como-executar)
3. [Controles](#controles)
4. [Como jogar](#como-jogar)
5. [Craques](#craques)
6. [Pontuação e evolução](#pontuação-e-evolução)
7. [Telas do jogo](#telas-do-jogo)
8. [Como compilar](#como-compilar)
9. [Organização do código](#organização-do-código)
10. [Conceitos de programação aplicados](#conceitos-de-programação-aplicados)
11. [Requisitos do trabalho](#requisitos-do-trabalho)
12. [Créditos](#créditos)

---

## Sobre o jogo

Você é um atacante que conduz a bola pelo campo enquanto os zagueiros adversários tentam desarmá-lo.
Desvie, faça dribles para somar combos, colete moedas e, ao chegar na área, enfrente o goleiro na finalização.
Entre as fases, use as moedas no **Centro de Treinamento** para evoluir seu jogador.

| | |
|---|---|
| **Objetivo** | Vencer as 5 fases e ser **CAMPEÃO** |
| **Derrota** | Perder todas as vidas (**FIM DE JOGO**) |
| **Gênero** | Ação / corrida lateral (*side-scroller*) |
| **Linguagem** | C (padrão C99) |
| **Biblioteca** | Raylib 5.5 |
| **Plataforma** | Windows (também compila no Linux) |

---

## Como executar

1. Extraia a pasta do projeto.
2. Dê dois cliques em **`DriblandoZagueiros.exe`**.

> A pasta `assets/` precisa estar ao lado do `.exe`. **Não é preciso instalar o Raylib para jogar**, porque ele já está embutido no executável.

---

## Controles

| Ação | Teclado | Gamepad |
|---|---|---|
| Mover o jogador | Setas ou `W` `A` `S` `D` | Analógico esquerdo / direcional |
| Chutar / confirmar | `Espaço` ou `Enter` | `A` |
| Pausar | `P` ou `Esc` | `Start` |
| Voltar | `Backspace` | `B` |
| Tela cheia | `F11` | |

---

## Como jogar

### 1. Corrida

![Corrida](docs/screenshots/corrida.png)

- O campo rola sozinho. Você movimenta o atacante para desviar dos zagueiros.
- A barra **GOL**, no canto superior direito, mostra quanto falta até a área.
- **Drible:** passar *raspando* por um zagueiro sem encostar nele. Dribles seguidos aumentam o **combo** (até x10).
- **Perder a bola:** encostar num zagueiro custa **1 vida** e zera o combo.

| Zagueiro | Cor | Comportamento | Aparece a partir da |
|---|---|---|---|
| Normal | Azul | Corre em linha reta | Fase 1 |
| Marcador | Azul-claro | Acompanha a sua altura no campo | Fase 2 |
| Carrinho | Laranja | Mostra um **"!"** e dá um carrinho em diagonal | Fase 3 |

### 2. Finalização

![Finalização](docs/screenshots/finalizacao.png)

- Ao chegar na área, posicione-se com **Cima/Baixo** e chute com **Espaço** quando a mira (pontilhado amarelo) estiver boa.
- O goleiro se movimenta e reage à bola.
- São **3 chances**. Se errar as 3, você perde 1 vida e tenta de novo.

### 3. Centro de Treinamento

![Centro de Treinamento](docs/screenshots/loja.png)

Depois de cada gol, gaste as moedas coletadas em melhorias permanentes:

| Melhoria | Efeito | Custo por nível |
|---|---|---|
| Velocidade | Corre mais rápido pelo campo | 4 / 7 / 10 moedas |
| Drible | Área de contato menor: mais fácil escapar | 4 / 7 / 10 moedas |
| Chute | Chute mais forte e mira mais lenta (mais precisa) | 4 / 7 / 10 moedas |
| Fôlego | +1 vida em cada fase | 4 / 7 / 10 moedas |

Os quadrados **azuis** são o bônus do craque escolhido e os **verdes** são as melhorias compradas.

---

## Craques

![Escolha do craque](docs/screenshots/escolha.png)

Antes da partida você escolhe um entre três jogadores **fictícios**.
Todos têm 3 níveis de bônus, mas distribuídos de formas diferentes:

| Craque | Estilo | Velocidade | Drible | Chute | Fôlego |
|---|---|:---:|:---:|:---:|:---:|
| **Ziguezague** | Driblador | +1 | +2 | – | – |
| **Canhão** | Artilheiro | – | – | +2 | +1 |
| **Foguete** | Velocista | +2 | – | +1 | – |

Para mudar nomes ou equilibrar os atributos, edite apenas a tabela em `src/personagens.c`.

---

## Pontuação e evolução

| Evento | Pontos |
|---|---|
| Distância percorrida | 1 ponto por metro |
| Drible | 100 × combo atual |
| Moeda coletada | 25 |
| Gol | 1000 × número da fase |
| Vida restante ao fim da fase | 200 por vida |

**Dificuldade progressiva:** a cada fase os zagueiros ficam mais rápidos e aparecem com mais frequência, novos tipos entram em campo, o goleiro melhora e o campo fica mais longo.

**Recorde:** a maior pontuação fica salva no arquivo `recorde.dat` e aparece no menu inicial.

---

## Telas do jogo

| Tela | Descrição |
|---|---|
| Menu inicial | Jogar, Como jogar, Sair e o recorde atual |
| Como jogar | Resumo das regras e controles |
| Escolha do craque | Seleção entre os 3 personagens |
| Corrida | Fase principal, com HUD de fase, pontos, moedas, vidas, combo e distância |
| Finalização | Chute a gol contra o goleiro |
| Fase concluída | Resumo da fase: dribles, moedas e bônus |
| Centro de Treinamento | Compra de melhorias |
| Pausa | Continuar ou voltar ao menu |
| Fim de jogo | Exibida ao perder todas as vidas |
| Campeão | Exibida ao vencer as 5 fases |

![Campeão](docs/screenshots/campeao.png)

---

## Como compilar

### Windows (recomendado)

1. Instale o **Raylib 5.5** pelo instalador oficial ([raylib.com](https://www.raylib.com) → *raylib_installer*).
   Ele cria a pasta `C:\raylib` com o compilador (`gcc`, via w64devkit) já incluso.
2. Dê dois cliques em **`build.bat`**.
3. O arquivo `DriblandoZagueiros.exe` será gerado na pasta do projeto.

### Com `make` (terminal w64devkit ou Linux)

```bash
make            # compila o jogo
make clean      # remove o executável
```

> No Linux é preciso ter o Raylib 5.5 instalado no sistema.

### Regerar os sprites e sons (opcional)

```bash
pip install pillow
python tools/gerar_assets.py
```

---

## Organização do código

```
DRIBLANDO/
├── main.c                  janela, loop principal e ampliação da tela virtual (480x270)
├── include/                cabeçalhos (.h), um para cada módulo
│   └── config.h            constantes de regras, pontuação e balanceamento
├── src/
│   ├── jogo.c              máquina de estados (menu, escolha, corrida, finalização, loja, pausa, fim)
│   ├── jogador.c           movimento, vidas, hitbox e níveis do atacante
│   ├── personagens.c       tabela dos três craques selecionáveis
│   ├── adversarios.c       zagueiros (3 tipos), moedas, colisões e detecção de dribles
│   ├── finalizacao.c       minigame de chute a gol contra o goleiro
│   ├── loja.c              Centro de Treinamento (melhorias com moedas)
│   ├── interface.c         campo, HUD, textos, painéis e avisos flutuantes
│   ├── controles.c         teclado e gamepad num só lugar (+ roteiros de teste)
│   ├── recursos.c          carregamento de imagens e sons
│   └── recorde.c           leitura e gravação do recorde em arquivo
├── assets/
│   ├── sprites/            atacantes, zagueiro, goleiro, bola, moeda, gol, arquibancada
│   └── sons/               chute, drible, moeda, falta, apito, gol, menu, torcida
├── tools/gerar_assets.py   script que gerou todos os sprites e sons
├── docs/screenshots/       imagens usadas neste README
├── build.bat               compilação com um clique (Windows)
├── Makefile                compilação com make
└── DriblandoZagueiros.exe  jogo pronto para executar
```

### Fluxo de estados

```
MENU ──► COMO JOGAR
  │
  └──► ESCOLHA DO CRAQUE ──► CORRIDA ──► FINALIZAÇÃO ──► FASE CONCLUÍDA ──► CENTRO DE TREINAMENTO ─┐
                               ▲   │          │                │                                    │
                               │   └─ PAUSA ──┘                └──► (5ª fase) CAMPEÃO               │
                               └───────────────────────────────────────── próxima fase ◄────────────┘

       Sem vidas (na corrida ou na finalização) ──► FIM DE JOGO ──► MENU
```

---

## Conceitos de programação aplicados

| Conceito | Onde aparece |
|---|---|
| `struct` e `enum` | `Jogador`, `Zagueiro`, `Personagem`, `EstadoJogo`, `TipoZagueiro` |
| Vetores de structs | zagueiros e moedas em campo (`adversarios.c`), tabela de craques (`personagens.c`) |
| Máquina de estados com `switch` | fluxo de telas em `jogo.c` |
| Detecção de colisão | `CheckCollisionRecs` entre o atacante, os zagueiros, as moedas, o goleiro e a bola |
| Números aleatórios | surgimento e tipo dos zagueiros, posição das moedas |
| Trigonometria | direção do chute (`cosf`/`sinf`) na finalização |
| Leitura e escrita de arquivos | recorde em `recorde.dat` (`fopen`, `fscanf`, `fprintf`) |
| Modularização | separação em `.h` e `.c`, constantes centralizadas em `config.h` |
| Ponteiros | módulos recebem `Jogador *`, `Adversarios *`, `Finalizacao *` |
| Game loop com delta time | movimento independente da taxa de quadros |

---

## Requisitos do trabalho

| Requisito | Como foi atendido |
|---|---|
| Biblioteca Raylib | Raylib 5.5 para janela, gráficos, entrada e áudio |
| Linguagem C ou C++ | C99 |
| Jogo funcional com objetivo claro | Vencer 5 fases, com pontuação e recorde |
| Telas de início e fim | Menu inicial, Fim de Jogo e Campeão |
| Condição de vitória e derrota | Vitória ao vencer a 5ª fase; derrota ao ficar sem vidas |
| Controle por teclado, mouse ou gamepad | Teclado e gamepad |
| Assets próprios ou com licença livre, com créditos | Sprites e sons próprios (ver [Créditos](#créditos)) |

---

## Créditos

- **Desenvolvimento:** Matheus Silva
- **Biblioteca:** [Raylib](https://www.raylib.com) 5.5, de Ramon Santamaria e colaboradores, sob licença zlib/libpng.
- **Sprites e sons:** autoria própria. Foram gerados por código com o script `tools/gerar_assets.py` (pixel art desenhada com a biblioteca Pillow e áudio sintetizado em WAV). Nenhum material de terceiros foi utilizado.
- **Fonte:** fonte padrão embutida no Raylib.
- **Personagens:** Ziguezague, Canhão e Foguete são personagens fictícios, sem relação com jogadores reais.
