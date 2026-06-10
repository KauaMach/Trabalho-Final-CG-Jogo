# 🦠 IMUNIDADE: A Guerra Celular

> **Trabalho Final — Computação Gráfica (DC/CCN032 · 2026.1)**  
> Universidade Federal do Piauí — Prof. Dr. Laurindo de Sousa Britto Neto  
> Gênero: Shoot 'em Up (Shmup) 3D · Jogabilidade 2D · OpenGL + freeGLUT

---

## 📖 Sumário

1. [Sobre o Jogo](#sobre-o-jogo)
2. [História](#história)
3. [Mecânicas Principais](#mecânicas-principais)
4. [Controles](#controles)
5. [Fases e Progressão](#fases-e-progressão)
6. [Inimigos](#inimigos)
7. [Inteligência Artificial](#inteligência-artificial)
8. [Sistema de Pontuação](#sistema-de-pontuação)
9. [HUD e Interface](#hud-e-interface)
10. [Recursos OpenGL Utilizados](#recursos-opengl-utilizados)
11. [Arquitetura do Projeto](#arquitetura-do-projeto)
12. [Dependências e Compilação](#dependências-e-compilação)
13. [Referências e Inspirações](#referências-e-inspirações)

---

## Sobre o Jogo

**Imunidade: A Guerra Celular** é um *shoot 'em up* com scroll vertical/horizontal que se passa **dentro do corpo humano**. Em vez do espaço sideral clássico dos jogos que o inspiraram — Space Invaders, Galaga, Xenon II e Ikaruga — o campo de batalha são os sistemas biológicos de um paciente infectado por um vírus mutante.

O jogador controla o **NANOCELL-1**, um nanobô médico injetado como último recurso. O objetivo é eliminar os agentes patogênicos, manter a **Saúde do Paciente** acima de zero e, ao final, destruir o núcleo do vírus NEXUS-7.

### Destaques do Projeto

| Característica | Descrição |
|---|---|
| **Mecânica central** | Sistema de polaridade cromática (Azul ↔ Vermelho) |
| **IA** | Comportamento de enxame via algoritmo de Boids |
| **Progressão** | Medidor de Saúde do Paciente como gatilho de boss |
| **Gráficos** | OpenGL 3D com jogabilidade 2D, Gouraud shading, texturas e blending |
| **Fases** | 5 ambientes biológicos distintos + 5 bosses temáticos |

---

## História

O **Paciente Zero** — um cientista de 34 anos — foi infectado por uma cepa sintética criada pelo grupo biohacker **Entropia Corp**: o vírus **NEXUS-7**, uma entidade semi-consciente capaz de aprender, evoluir e coordenar ataques em enxame.

Com o sistema imunológico em colapso, o único recurso restante é o **Protocolo Ícaro**: injetar um nanobô experimental de 800nm diretamente na corrente sanguínea. O jogador assume o controle desse nanobô numa batalha microscópica pela sobrevivência do hospedeiro.

### Arco Narrativo

```
Fase 1 — Corrente Sanguínea   → Primeiro contato. NANOCELL-1 aprende o ambiente.
Fase 2 — Barreira Pulmonar    → O vírus invade os alvéolos. O cenário respira.
Fase 3 — Nódulo Linfático     → Aliados e traidores. Linfócitos corrompidos enganam.
Fase 4 — Sistema Nervoso      → Sinapses invertem controles. Inimigos imitam o jogador.
Fase 5 — Núcleo Viral         → Arena final. Destruir o NEXUS-7 Omega e curar o paciente.
```

---

## Mecânicas Principais

### 1. Sistema de Polaridade Cromática

A mecânica central do jogo. O NANOCELL-1 opera em duas polaridades que alteram ataque, defesa e vulnerabilidade simultaneamente.

```
POLARIDADE AZUL (Criocinética)          POLARIDADE VERMELHA (Térmica)
────────────────────────────────        ─────────────────────────────────
✔  Absorve projéteis AZUIS             ✔  Absorve projéteis VERMELHOS
✔  Disparo: raio contínuo frio         ✔  Disparo: pulso explosivo em área
✔  Aplica slow em inimigos atingidos   ✔  Dano em área ao impacto
✘  Vulnerável a projéteis VERMELHOS    ✘  Vulnerável a projéteis AZUIS
```

**Regra crítica:** projéteis da cor correta são **absorvidos** (sem dano) e carregam o medidor SURGE. Projéteis da cor errada causam dano total.

### 2. Ataque SURGE

Carregado absorvendo projéteis inimigos da mesma polaridade. Ao atingir 100%, libera um super-ataque devastador:

- **Blizzard SURGE** (Azul): congela toda a tela por 3 segundos com dano contínuo.
- **Inferno SURGE** (Vermelho): explosão radial que destrói todos os projéteis ativos na tela.

### 3. Medidor de Saúde do Paciente (HSP)

Funciona como medidor de progressão da fase, não apenas como penalidade:

```
+2%  → matar inimigo comum
+5%  → matar inimigo elite
+15% → derrotar boss
-3%  → inimigo atravessa a tela sem ser eliminado
-1%  → NANOCELL-1 recebe qualquer dano
```

> **HSP = 100%** → Boss da fase é liberado imediatamente.  
> **HSP = 0%**   → Game Over. A fase reinicia do início.

### 4. Power-ups Orgânicos

| Power-up | Efeito |
|---|---|
| 💊 Anticorpo IgG | +30% dano por 10 segundos |
| 🧬 Fragmento de RNA | +1 bomba de área |
| ⚡ Mitocôndria | Velocidade +50% por 8 segundos |
| 🛡️ Linfócito T | Escudo temporário (3 hits) |
| 🔄 Proteína Dual | +20% carregamento do SURGE |
| ✨ Célula-Tronco | Restaura 20% da vida do nanobô |

### 5. Dash de Evasão

Pressionando `E`, o NANOCELL-1 executa um dash rápido na direção do movimento atual. Cooldown de 2 segundos. Durante o dash, há 12 frames de invencibilidade (*i-frames*).

---

## Controles

| Tecla | Ação |
|---|---|
| `W A S D` | Movimentação 2D do NANOCELL-1 |
| `SPACE` | Disparo contínuo (polaridade ativa) |
| `SHIFT` | Alternar polaridade (Azul ↔ Vermelho) |
| `Q` | Ativar SURGE (quando 100% carregado) |
| `E` | Dash de evasão (cooldown 2s) |
| `ESC` | Pausar / Menu principal |
| `F` | Tela cheia (*fullscreen*) |
| `R` | Reiniciar fase (após Game Over) |

---

## Fases e Progressão

### Fase 1 — Corrente Sanguínea
**Cenário:** Scroll horizontal com hemácias 3D flutuando como obstáculos decorativos. Plasma sanguíneo simulado por blending OpenGL.  
**Mecânica especial:** Correntes de plasma aumentam temporariamente a velocidade do nanobô.  
**Boss:** *Leukocyte Corrupto* — leucócito infectado com 3 padrões de ataque.  
**Objetivo narrativo:** Tutorial implícito. O jogador aprende o sistema de polaridade.

### Fase 2 — Barreira Pulmonar
**Cenário:** Alvéolos que se expandem e contraem em ciclo de 4 segundos. Inimigos sincronizam aparições com o ritmo da "respiração".  
**Mecânica especial:** Alvéolo expandido = janela de vulnerabilidade do boss.  
**Boss:** *Pneumococo Gigante* — ataca exclusivamente no ciclo de expiração.

### Fase 3 — Nódulo Linfático
**Cenário:** Interior de gânglio linfático com fluxo de linfa como corrente de fundo.  
**Mecânica especial:** Linfócitos T aliados circulam na tela e podem ser coletados como escudo. Linfócitos corrompidos imitam a aparência dos aliados — a distinção é feita pela cor.  
**Boss:** *Nexus-7 Beta* — aprende a polaridade dominante usada pelo jogador e aumenta projéteis da cor oposta.

### Fase 4 — Sistema Nervoso Central
**Cenário:** Axônios e sinapses. Pulsos elétricos atravessam a tela periodicamente.  
**Mecânica especial:** Descarga sináptica **inverte os controles** do jogador por 3 segundos. Vírus Delta copia posição e disparo do nanobô com 2s de delay.  
**Boss:** *Ganglion Prime* — ao entrar em *enrage*, inverte a polaridade do jogador forçosamente.

### Fase 5 — Núcleo Viral (Boss Rush)
**Cenário:** Interior do próprio NEXUS-7. Arena circular fechada, sem scroll.  
**Mecânica especial:** Sem entrada de novos inimigos comuns — toda a atenção vai ao boss.  
**Boss Final:** *NEXUS-7 Omega* com 3 formas sequenciais:

```
Forma 1 — Escudo Polar      : Destruir escudos da cor correta com projéteis polarizados.
Forma 2 — Enxame Definitivo : Controla 40 vírus simultâneos via Boids. Cobre toda a arena.
Forma 3 — Núcleo Exposto    : HP baixo, bullet-hell intenso. SURGE duplo para finalizar.
```

### Sistema de Ranking por Fase

Ao final de cada fase é atribuído um ranking de **S** a **D** baseado em:
- Total de kills realizados
- Saúde do Paciente final (HSP)
- Dano total recebido pelo nanobô
- Maior combo de kills consecutivos
- Tempo total da fase

Ranking S em todas as fases desbloqueia a skin **NEXUS-ZERO** para o NANOCELL-1.

---

## Inimigos

| Nome | Polaridade | Padrão | Comportamento |
|---|---|---|---|
| Vírus Alfa | Azul | Enxame | Boids clássico. Mergulha em formação V. |
| Bactéria Coco | Vermelho | Linear | Alta vida, tiro duplo, sem IA complexa. |
| Esporo Fúngico | Azul | Kamikaze | Detecta posição do jogador e vai direto. Explode ao contato. |
| Vírus Gama | Dual | Camuflagem | Invisível no plasma. Aparece ao atirar. Troca de cor ao ser atingido. |
| Príon Mimético | Oposta | Shadowplay | Armazena 120 posições do jogador e se move com 2s de atraso. |
| Vírus Delta | Vermelho | Espelho+Ataque | Copia movimento e dispara simultaneamente. Aprende padrão de evasão. |
| Microplaqueta Hive | Ambos | Boids Avançado | Metade do enxame é azul, metade vermelha. Exige troca de polaridade constante. |

---

## Inteligência Artificial

### Algoritmo de Boids (Craig Reynolds, 1986)

Os vírus menores operam via 3 regras base + 2 extensões combativas:

```
R1 — Separação   : Mantém distância mínima dos vizinhos (raio ~30px).
                   Vetor repulsão normalizado evita sobreposição.

R2 — Alinhamento : Tende a alinhar velocidade com a média dos vizinhos (raio ~80px).
                   Produz movimento fluido e orgânico.

R3 — Coesão      : Atraído pelo centroide do grupo local (raio ~120px).
                   Mantém o enxame unido durante navegação.

R4 — Cerco       : Quando detecta NANOCELL-1 no raio de 200px, adiciona vetor de
                   perseguição. Divide o grupo em dois subgrupos para flanquear.

R5 — Fuga SURGE  : Se o SURGE do jogador está carregado, o enxame aumenta separação
                   e dispersa preventivamente para reduzir dano em área.
```

**Otimização:** Spatial hashing em grade 32×32 reduz busca de vizinhos de O(n²) para ~O(n).

### Máquina de Estados dos Bosses

```
PATRULHA   (vida > 70%)  → Movimento senoidal. Ataques básicos e previsíveis.
AGRESSIVO  (30–70%)      → Padrões complexos de projéteis. Velocidade aumentada.
FÚRIA      (vida < 30%)  → Enrage. Convoca minions. Padrões bullet-hell.
ADAPTAÇÃO  (Nexus-7)     → Memoriza polaridade dominante do jogador e contra-ataca
                           com projéteis da cor oposta em maior frequência.
```

### IA dos Príons Miméticos

Utiliza **buffer circular** de posições:
1. Armazena as últimas 120 posições (x, y) do NANOCELL-1 (equivalente a 2s a 60fps).
2. Move-se exatamente para a posição `[t - 2s]` a cada frame.
3. Dispara projéteis com cor oposta mirando a posição atual do jogador (predição simples).
4. O delay diminui progressivamente conforme a vida do Príon cai: 2s → 1.5s → 1s → 0.5s.

---

## Sistema de Pontuação

### Multiplicadores de Combo

| Multiplicador | Condição |
|---|---|
| ×1 (base) | Qualquer kill sem combo ativo |
| ×2 | 5 kills consecutivos sem receber dano |
| ×3 | 10 kills + troca de polaridade com absorção ativa |
| ×5 | Kill realizado durante SURGE ativo |
| ×10 | Boss derrotado sem tomar nenhum dano na fase inteira (*Perfect Clear*) |

### Moeda: Fragmentos de DNA

Dropados por inimigos ao morrer. Acumulados entre fases e gastos na **Árvore de Upgrades**:

```
Caminho Ataque     → Dano base, cadência de disparo, tamanho dos projéteis
Caminho Absorção   → SURGE carrega mais rápido, raio de absorção maior
Caminho Mobilidade → Velocidade base, dash mais rápido, cooldown reduzido
```

---

## HUD e Interface

```
┌─────────────────────────────────────────────────────────┐
│  NANOCELL-1 ❤️❤️❤️          FASE 3 — NÓDULO LINFÁTICO  │
│  VIDA   ███████░░  78%      HSP   ██████░░░  62%        │
│                                                          │
│  ● POLARIDADE: AZUL         SURGE ████░░░░  47%         │
│                                                          │
│  SCORE: 48.200   COMBO: x3   DNA: ◈ 120                 │
└─────────────────────────────────────────────────────────┘
```

- **Vida do nanobô:** barra superior esquerda (3 corações máximo).
- **HSP:** barra superior direita — verde acima de 50%, amarela entre 25–50%, vermelha abaixo de 25%.
- **Indicador de polaridade:** círculo colorido com brilho pulsante (azul ou vermelho).
- **Medidor SURGE:** barra lateral — pisca ao atingir 100%.
- **Score / Combo / DNA:** rodapé inferior central.

---

## Recursos OpenGL Utilizados

| Recurso | Aplicação no Jogo |
|---|---|
| **Texturas 2D** | Sprites orgânicos dos inimigos, plasma sanguíneo, fundo de fase |
| **Gouraud Shading** | Interpolação suave de cor em membranas celulares e corpo dos bosses |
| **Blending** (`GL_SRC_ALPHA`) | Transparência de plasma, efeitos de SURGE, partículas de explosão |
| **Depth Buffer** | Objetos 3D de fundo em Z negativo; gameplay 2D em Z = 0 |
| **Iluminação por ponto de luz** | Luz ambiente + ponto de luz dinâmico na cor da polaridade ativa |
| **Sistema de partículas** | CPU-side, 512 partículas simultâneas para trails e explosões |
| **Display Lists / VBOs** | Geometria estática de fundo pré-compilada para performance |
| **Viewport e Projeção** | Perspectiva 3D para fundo; ortográfica 2D para gameplay e HUD |

---

## Arquitetura do Projeto

```
imunidade/
├── src/
│   ├── main.cpp              # Loop principal GLUT, callbacks de input e timer
│   ├── renderer.cpp          # Pipeline OpenGL: texturas, shaders, iluminação
│   ├── renderer.h
│   ├── ai_boids.cpp          # Implementação Boids (5 regras) + spatial hashing
│   ├── ai_boids.h
│   ├── ai_fsm.cpp            # Máquina de estados dos bosses
│   ├── ai_fsm.h
│   ├── collision.cpp         # Broadphase (grid) + Narrowphase (AABB / círculo)
│   ├── collision.h
│   ├── audio.cpp             # Integração SDL_Mixer: música + SFX
│   ├── audio.h
│   ├── hud.cpp               # Renderização do HUD (modo ortográfico)
│   ├── hud.h
│   ├── entities/
│   │   ├── player.cpp        # NANOCELL-1: movimento, polaridade, SURGE, dash
│   │   ├── enemy.cpp         # Base de inimigos + especializações
│   │   ├── boss.cpp          # Lógica específica dos 5 bosses
│   │   └── projectile.cpp    # Projéteis com tag de polaridade
│   └── scenes/
│       ├── scene_base.cpp    # Interface base de cena
│       ├── scene_01_blood.cpp
│       ├── scene_02_lung.cpp
│       ├── scene_03_lymph.cpp
│       ├── scene_04_nerve.cpp
│       └── scene_05_core.cpp
├── assets/
│   ├── textures/             # PNG: sprites, fundos, HUD
│   ├── audio/                # OGG: músicas · WAV: SFX
│   └── models/               # OBJ: geometria 3D dos bosses (tinyobjloader)
├── include/                  # Headers de terceiros (GLM, stb_image, etc.)
├── Makefile
└── README.md
```

### Detecção de Colisão

**Fase Larga (Broadphase):** grade espacial 32×32 células. Apenas entidades na mesma célula ou vizinhas são testadas — reduz O(n²) para ~O(n).

**Fase Precisa (Narrowphase):**
- Projéteis: AABB (retângulos alinhados ao eixo).
- Entidades biológicas: sphere-sphere (raio configurável por tipo).
- Bosses: múltiplos hitboxes por segmento corporal.

**Lógica de Polaridade na Colisão:**
```
projetil.cor == jogador.polaridade  →  absorção: +SURGE, sem dano
projetil.cor != jogador.polaridade  →  dano normal + feedback visual
```

---

## Dependências e Compilação

### Bibliotecas Necessárias

| Biblioteca | Uso | Instalação (Ubuntu/Debian) |
|---|---|---|
| freeGLUT | Janela e callbacks OpenGL | `apt install freeglut3-dev` |
| GLEW | Extensões OpenGL | `apt install libglew-dev` |
| SDL2_mixer | Áudio (música + SFX) | `apt install libsdl2-mixer-dev` |
| GLM | Matemática vetorial (Boids) | `apt install libglm-dev` |
| stb_image | Carregamento de texturas PNG | Incluso em `include/` (header-only) |
| tinyobjloader | Carregamento de modelos OBJ | Incluso em `include/` (header-only) |

### Compilação

```bash
# Instalar dependências (Ubuntu/Debian)
sudo apt update
sudo apt install freeglut3-dev libglew-dev libsdl2-mixer-dev libglm-dev

# Clonar e compilar
git clone https://github.com/usuario/imunidade-guerra-celular.git
cd imunidade-guerra-celular
make

# Executar
./imunidade
```

### Makefile (resumo)

```makefile
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LIBS     = -lGL -lGLU -lglut -lGLEW -lSDL2_mixer
TARGET   = imunidade
```

---

## Referências e Inspirações

### Jogos

| Jogo | Influência |
|---|---|
| **Space Invaders** (Taito, 1978) | Estrutura base de shmup, formações de inimigos |
| **Galaxian** (Namco, 1979) | Inimigos com ataques individuais em mergulho |
| **Galaga** (Namco, 1981) | Padrões de entrada em formação, power-ups de nave |
| **Xenon II: Megablast** (Bitmap Brothers, 1989) | Scroll vertical, variedade de inimigos biológicos |
| **Deluxe Galaga** (Edgar M. Vigdal, 1994) | Sistema de multiplicadores e bonus stages |
| **Ikaruga** (Treasure, 2001) | **Sistema de polaridade cromática — inspiração direta** |

### Algoritmos e Artigos

- Reynolds, C. W. (1987). *Flocks, herds and schools: A distributed behavioral model.* ACM SIGGRAPH.
- Foley, J. et al. (1990). *Computer Graphics: Principles and Practice.* Addison-Wesley.
- Akenine-Möller, T. et al. (2018). *Real-Time Rendering* (4ª ed.). CRC Press.

### Bibliotecas e Ferramentas

- [freeGLUT](http://freeglut.sourceforge.net/) — Implementação livre do GLUT
- [GLEW](http://glew.sourceforge.net/) — OpenGL Extension Wrangler
- [GLM](https://glm.g-truc.net/) — OpenGL Mathematics
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/) — Áudio multiplataforma
- [stb_image](https://github.com/nothings/stb) — Carregamento de imagens (header-only)
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) — Parser de arquivos OBJ

---

## Equipe

| Membro | Função |
|---|---|
| [Seu Nome] | Desenvolvimento completo |

**Disciplina:** Computação Gráfica (DC/CCN032)  
**Período:** 2026.1  
**Instituição:** Universidade Federal do Piauí (UFPI)  
**Professor:** Prof. Dr. Laurindo de Sousa Britto Neto

---

*"No universo microscópico, até a menor célula pode mudar o destino de um paciente."*
