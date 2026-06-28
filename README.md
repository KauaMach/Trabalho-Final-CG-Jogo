
# 🦠 IMUNIDADE: A Guerra Celular
### Game Design Document — v1.0

> **Trabalho Final — Computação Gráfica (DC/CCN032 · 2026.1)**
> Universidade Federal do Piauí — Prof. Dr. Laurindo de Sousa Britto Neto
> Gênero: Shoot 'em Up (Shmup) 3D · Jogabilidade 2D · OpenGL + freeGLUT

---
| Campo | Detalhe |
|---|---|
| **Título** | Imunidade: A Guerra Celular |
| **Gênero** | Shoot 'em Up · Top-Down · Bullet Hell Tático |
| **Perspectiva** | 3D com Projeção Ortográfica Isométrica |
| **Plataformas** | Linux, macOS, Windows |
| **Motor / API** | OpenGL (Fixed-Function Pipeline) via freeGLUT |
| **Linguagem** | C++17 |
| **Áudio** | SDL2 + SDL2_mixer |
| **Resolução** | 1024 × 768 px (janela redimensionável) |
| **Framerate alvo** | 60 FPS — timer `glutTimerFunc` de 16 ms |
| **Nº de Fases** | 03 Fases: Corrente Sanguínea, Pulmões, Sistema Nervoso |
--

## Sobre o Jogo


**Imunidade: A Guerra Celular** é um Shoot 'em Up (Shmup) com perspectiva 3D isométrica, ambientado no interior do corpo humano. O jogador controla o **NANOCELL-1**, um nanobô médico injetado na corrente sanguínea para combater uma infecção viral mutante chamada **Nexus-7**. O jogo é desenvolvido em C++17 com OpenGL Fixed-Function Pipeline e freeGLUT.

A jogabilidade se passa em 2D dentro de um cenário 3D renderizado em projeção ortográfica. O diferencial criativo é o **sistema de polaridade cromática**, inspirado no clássico Ikaruga: o nanobô alterna entre dois estados (Azul e Vermelho), absorvendo tiros da mesma cor e sofrendo dano dos da cor oposta. Isso cria decisões táticas contínuas e uma curva de aprendizado orgânica.

> **Proposta central:** Um shmup biológico acessível, com mecânica de polaridade como elemento tático central, 3 fases temáticas com ambientação orgânica renderizada em OpenGL, sistema de Boids para IA de enxame e progressão por Saúde do Paciente.

### Destaques do Projeto

| Característica | Descrição |
|---|---|
| **Mecânica central** | Sistema de polaridade cromática (Azul ↔ Vermelho) |
| **IA** | Comportamento de enxame via algoritmo de Boids |
| **Progressão** | Medidor de Saúde do Paciente como gatilho de boss |
| **Gráficos** | OpenGL 3D com jogabilidade 2D, Gouraud shading, texturas e blending |
| **Fases** | 3 ambientes biológicos distintos + 3 bosses temáticos |

---

## 2. História

### 2.1 Contexto

O **Paciente Zero** — um cientista de 34 anos — foi infectado pela cepa **Nexus-7**, um vírus sintético criado pelo grupo biohacker *Entropia Corp*. O vírus é semi-inteligente: aprende padrões, evolui e coordena ataques celulares em enxame. O sistema imunológico entra em colapso.

Em última instância, é iniciado o **Protocolo Ícaro**: injetar o NANOCELL-1 — nanobô experimental de 800 nm — diretamente na corrente sanguínea. O jogador assume seu controle em uma batalha microscópica pela sobrevivência do hospedeiro.

### 2.2 Arco Narrativo por Fase

| Fase | Evento Narrativo |
|---|---|
| **Fase 1 — Corrente Sanguínea** | NANOCELL-1 é injetado. Primeiros vírus Alfa testam as defesas. Aprendizado das mecânicas. |
| **Fase 2 — Pulmões** | Nexus-7 invade os alvéolos. O cenário pulsa com a respiração do paciente. O vírus se torna mais organizado. |
| **Fase 3 — Sistema Nervoso** | Ataque ao cérebro. Inimigos adaptados imitam o jogador. Batalha final contra o Nexus Omega. |

---
## 3. Mecânicas Principais

### 3.1 Sistema de Polaridade Cromática

Mecânica central do jogo. O NANOCELL-1 opera em dois estados de polaridade que alteram comportamento de disparo, absorção e vulnerabilidade:

* **Alternância de Polaridade:** O jogador pode alternar a cor do NANOCELL-1 instantaneamente entre Azul (Frio) e Vermelha (Quente).
* **Ataque:** Disparos azuis causam dano em inimigos vermelhos, e vice-versa.
* **Defesa:** Se o jogador estiver Azul, ele é imune a projéteis azuis (eles são absorvidos carregando o especial SURGE), mas recebe dano de projéteis vermelhos.
* **Movimentação Plana em Mundo 3D:** A navegação ocorre nos eixos X e Z, mantendo o eixo Y fixo (ou com leves variações senoidais para simular flutuação).

### 3.2 Ataque SURGE

Ao absorver projéteis, a barra de SURGE vai de 0% a 100%. Quando cheia, o jogador ativa um super-ataque dependente da polaridade ativa:

- **SURGE Azul — Blizzard:** feixe contínuo de alta potência que atravessa a tela, causando dano extremo durante 2 segundos.
- **SURGE Vermelho — Inferno:** explosão radial massiva que destrói projéteis e causa dano em área contínuo, expandindo durante 2 segundos.


### 3.3 Medidor de Saúde do Paciente (HSP)

Substitui o conceito de "vidas" por um medidor coletivo de 0% a 100% que representa o estado do hospedeiro:

| Ação | Efeito no HSP |
|---|---|
| Eliminar inimigo comum | +2% |
| Eliminar inimigo elite | +5% |
| Boss derrotado | +15% |
| Inimigo passa da tela | -3% |
| NANOCELL-1 recebe dano | -2% por hit |
| HSP atingir 90% | Boss da fase é invocado imediatamente |
| HSP atingir 0% | Game Over — reinicia a fase atual |

### 3.4 Dash de Evasão e Barrel Roll

O NANOCELL-1 pode executar um recuo evasivo tático (*Back-Dash*) acompanhado de um *Barrel Roll* completo em torno do próprio eixo (tecla `E`). Cooldown de 2 segundos. Durante a manobra (0.8s), a nave desliza para trás automaticamente escapando de ataques frontais e garante uma breve janela de invulnerabilidade (*i-frames*) — fundamental para sobreviver a padrões densos de vírus.


### 3.5 Absorção Celular (Vida e Surge)

Para manter o foco tático na mecânica de polaridade e evitar poluição visual com itens na tela, os clássicos *Power-ups* foram substituídos por um sistema orgânico de absorção:

* **Recuperação Dupla:** Absorver projéteis ou chocar-se contra inimigos da mesma cor da sua polaridade tem dois efeitos simultâneos:
  1. Recarrega uma porcentagem da barra de ataque especial **SURGE**.
  2. Recupera uma fração da **Vida** do NANOCELL-1.
* **Taxas Variáveis:** A quantidade de vida recuperada depende do tipo de ataque absorvido (lasers pesados ou inimigos maiores curam uma taxa maior).
* **Risco vs Recompensa:** O **dano** sofrido ao colidir com a cor oposta é **sempre rigorosamente superior** à quantidade de vida ganha ao absorver a cor correta. Isso garante que o jogador não abuse da mecânica de cura e seja forçado a pilotar e alternar as cores com precisão cirúrgica.

### 3.6 Screen Wrapping (Efeito Pac-Man / Asteroids)

Para manter a dinâmica ágil e compensar a câmera fixa no eixo X, o jogo utiliza uma mecânica de teletransporte nas bordas laterais (*Screen Wrapping*). Se o NANOCELL-1 ultrapassar o limite máximo de visão da câmera pela direita, ele reaparecerá imediatamente na borda esquerda, e vice-versa. Isso cria novas estratégias evasivas e garante que a nave nunca "suma" do campo de visão.

---

## 4. Controles

| Tecla | Ação |
|---|---|
| `W` `A` `S` `D` | Movimentação 2D do NANOCELL-1 |
| `SPACE` | Disparo contínuo na polaridade ativa |
| `TAB` / `C` | Alternar polaridade (Azul ↔ Vermelho) |
| `Q` | Ativar SURGE (apenas quando barra estiver cheia) |
| `E` | Back-Dash evasivo com Barrel Roll (cooldown 2 s) |
| `R` | Fazer um Barrel Roll sem recuo no ar (Manobra Estilosa) |
| `1` a `4` | Alternar modos de Câmera (2.5D Dir, 2.5D Esq, Top-Down, 1ª Pessoa) |
| `ESC` | Pausar jogo / Abrir menu de pausa |
| `F` | Alternar fullscreen / janela |
| `ENTER` | Confirmar seleção nos menus |

> **Nota de implementação:** Movimentação via `glutKeyboardFunc` e `glutSpecialFunc`. Estado das teclas mantido em array booleano `keyState[256]` para movimento suave frame a frame.
---
## 5. Fases e Progressão

### 5.1 Fase 1 — Corrente Sanguínea

| Campo | Detalhe |
|---|---|
| **Objetivo** | Introduzir o sistema de polaridade e controles básicos |
| **Ambiente** | Scroll vertical lento. Hemácias 3D passam como obstáculos decorativos. Plasma simulado via blending OpenGL com objetos semi-transparentes. |
| **Inimigos** | Vírus Alfa (enxame Boids azul) e Bactéria Coco (tanque vermelho lento) |
| **Mecânica especial** |  |
| **Boss — Leukocyte Corrupto** | 3 estados: patrulha senoidal → ataque em espiral → fúria com projéteis densos em todas as direções |
| **Gatilho do Boss** | HSP atinge 90% ou 120 segundos de fase decorridos |


### 5.2 Fase 2 — Pulmões

| Campo | Detalhe |
|---|---|
| **Objetivo** | Apresentar mecânica de timing e abertura de janelas |
| **Ambiente** | Alvéolos 3D expandem e contraem em ciclo sinusoidal de 4 segundos, simulando a respiração. Passagens abrem e fecham. |
| **Inimigos** | Vírus Gama (Alterna entre Azul e Vermelho a cada 3s) e Esporo Fúngico (kamikaze azul de alta velocidade) |
| **Mecânica especial** | Alvéolo aberto = janela de vulnerabilidade do boss. Fechado = escudo natural. |
| **Boss — Pneumococo Gigante** | Ataca apenas na inspiração. Na expiração recua — janela de ataque seguro para o jogador. |
| **Gatilho do Boss** | HSP atinge 90% ou 120 segundos de fase |

### 5.3 Fase 3 — Sistema Nervoso Central

| Campo | Detalhe |
|---|---|
| **Objetivo** | Desafio máximo: inimigos imitadores e controles alterados por pulsos |
| **Ambiente** | Axônios e sinapses como cenário 3D. Pulsos elétricos percorrem o fundo. Flashes visuais temporários simulam interferência. |
| **Inimigos** | Príon Mimético (copia posição do jogador com 1,5 s de delay) e Vírus Delta (espelha e atira simultaneamente) |
| **Mecânica especial** | Pulso sináptico a cada 20 s: inverte eixos de movimento por 3 segundos |
| **Boss — Nexus Omega** | 3 formas: escudo dual → convoca enxame → núcleo exposto com bullet-hell final |
| **Gatilho do Boss** | HSP atinge 90% ou 120 segundos de fase |

### 5.4 Fluxo de Progressão Geral

```
Menu Principal
    └── Fase 1 ──► Resultado ──► Fase 2 ──► Resultado ──► Fase 3 ──► Resultado Final
              │                        │                        │
           (morte)                  (morte)                  (morte)
              └── Game Over ◄────────┴────────────────────────┘
```
### Sistema de Ranking por Fase

Ao final de cada fase é atribuído um ranking de **S** a **D** baseado em:
- Total de kills realizados
- Saúde do Paciente final (HSP)
- Dano total recebido pelo nanobô
- Maior combo de kills consecutivos
- Tempo total da fase
- Fases são desbloqueadas sequencialmente

---

## 6. Inimigos

| Nome | Polaridade / Tipo | Comportamento |
|---|---|---|
| **Vírus Alfa** | 🔵 Azul — Enxame | Ataca em grupo via Boids. Mergulha em formação V ao se aproximar do jogador. |
| **Bactéria Coco** | 🔴 Vermelho — Tanque | Alta vida, movimento linear, tiro duplo. Serve como obstáculo de absorção. |
| **Vírus Gama** | Vermelho/azul 🔴 🔵 - Camuflagem   | Alterna entre Azul e Vermelho a cada 3 segundos, forçando o jogador a adaptar sua polaridade. |
| **Esporo Fúngico** | 🔵 Azul — Kamikaze | Detecta posição do jogador e mergulha em linha reta. Explode em área ao contato. |
| **Príon Mimético** | Oposta — Espelho | Buffer circular de posições do jogador reproduzido com delay de 1,5 s. |
| **Vírus Delta** |Vermelho/azul 🔴 🔵 - Hacker | Copia movimento e dispara projéteis da cor oposta. Delay reduz com o tempo. |
| **Microplaqueta Hive** | Dual — Elite | Metade azul, metade vermelha. Exige troca de polaridade durante o combate. |

### 6.1 Tabela de Balanceamento de HP e Resistência

| Inimigo (Fase)          | Tipo / Comportamento    | HP (Vida)  | Tiros Certos p/ Matar | Tiros Errados p/ Matar |
|-------------------------|-------------------------|:----------:|:---------------------:|:----------------------:|
| **Vírus Alfa** (1) | Enxame fraco (Azul)     |    15.0    |      1 a 2 tiros      |        15 tiros        |
| **Bactéria Coco** (F1)  | Tanque Lento (Vermelho) |    40.0    |        4 tiros        |        40 tiros        |
| **Leukocyte Corrupto** (F1)  | BOSS (Fase 1)           |   400.0    |       40 tiros        |       400 tiros        |
| **Esporo Fúngico** (F2) | Kamikaze muito rápido   |    10.0    |   1 tiro (One-hit)    |        10 tiros        |
| **Vírus Gama** (F2) | Troca de cor a cada 3s  |    20.0    |        2 tiros        |        20 tiros        |
| **Pneumococo Gigante**  | BOSS (Fase 2)           |   600.0    |       60 tiros        |       600 tiros        |
| **Príon Mimético** (F3) | Copia sua posição       |    30.0    |        3 tiros        |        30 tiros        |
| **Vírus Delta** (3)| Atira e reflete danos   |    45.0    |      4 a 5 tiros      |        45 tiros        |
| **Microplaqueta Hive**  | Elite (Fase 2 e 3)      |    80.0    |        8 tiros        |        80 tiros        |
| **Nexus Omega** (F3)   | BOSS FINAL (Fase 3)     |   1200.0   | 120 tiros (Use SURGE!)| Impossível sem a cor certa |

**Análise do Design:**
- Note que o **Esporo Fúngico** tem vida quase nula (10 de HP), pois como ele é um "Kamikaze" que se joga contra você muito rápido, o jogador precisa conseguir matá-lo em um milissegundo de reação.
- A **Bactéria Coco** tem 40 de HP justamente porque o README a define como um "Tanque" de resistência, obrigando o jogador a ficar atirando nela por alguns segundos.
- O **Nexus Omega (Final Boss)** tem tanta vida que obrigará o jogador a usar o medidor de **SURGE (Especial)** várias vezes durante a luta para conseguir vencer.

### 6.2 Tabela de Danos (Nanocell e Paciente)

| Inimigo / Entidade | Tipo de Ataque | Dano na Nanocell (Vida 100%) | Impacto no Paciente (HSP 100%) |
|:---|:---|:---:|:---:|
| **Qualquer Inimigo** | **Fuga** (Passar direto pelo jogador) | `0%` | `-3.0%` (Punição de Fuga) |
| **Vírus Alfa** (F1) | **Colisão Física** (Kamikaze/Mergulho) | `-10.0%` | `0%` |
| **Bactéria Coco** (F1) | **Colisão Física** (Trombar no Tanque) | `-10.0%` | `0%` |
| **Bactéria Coco** (F1) | **Projétil** (Tiro Duplo Inimigo) | `-5.0%` por acerto | `0%` |
| **Esporo Fúngico** (F2) | **Colisão Explosiva** (Área) | `-20.0%` | `-5.0%` (Infecção grave) |
| **Vírus Gama** (F2) | **Colisão Física** | `-10.0%` | `0%` |
| **Microplaqueta Hive** (F2) | **Colisão Física** | `-15.0%` | `0%` |
| **Príon Mimético** (F3) | **Colisão Física / Confusão** | `-15.0%` | `0%` |
| **Vírus Delta** (F3) | **Projétil Refletido** | `-8.0%` por acerto | `0%` |
| **Bosses (Qualquer Fase)** | **Colisão Física Direta** | `-30.0%` (Esmagamento) | `-10.0%` |
| **Bosses (Qualquer Fase)** | **Projéteis / Habilidades Especiais**| `-10.0%` a `-20.0%` | `0%` |

*(Nota: Destruir qualquer inimigo sempre curará o paciente em `+2.0%` de HSP como recompensa).*

---

## 7. Inteligência Artificial

### Algoritmo de Boids (Craig Reynolds, 1986)  — Enxame dos Vírus

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

### 7.2 Máquina de Estados dos Bosses (FSM)

```
PATRULHA   (vida > 70%)  → Movimento senoidal. Ataques básicos e previsíveis.
AGRESSIVO  (30–70%)      → Padrões complexos de projéteis. Velocidade aumentada.
FÚRIA      (vida < 30%)  → Enrage. Convoca minions. Padrões bullet-hell.
ADAPTAÇÃO  (Nexus-7)     → Memoriza polaridade dominante do jogador e contra-ataca
                           com projéteis da cor oposta em maior frequência.
```

### 7.3 Príon Mimético — Buffer Circular

O Príon Mimético usa uma fila circular de **90 posições (x, y)** do jogador (1,5 s × 60 FPS):

1. A cada frame, a posição atual do NANOCELL-1 é inserida na cabeça da fila.
2. O Príon se move para a posição `[t - 1,5 s]` lida da cauda da fila.
3. Dispara projéteis da cor oposta ao estado atual do jogador com predição simples.
4. O delay diminui de **1,5 s → 0,8 s** conforme a vida do Príon cai abaixo de 50%.
---

## 8. Sistema de Pontuação

| Evento | Pontos / Multiplicador |
|---|---|
| Kill — inimigo comum | 100 pts |
| Kill — inimigo elite | 300 pts |
| Boss derrotado | 2.000 pts |
| Absorção de projétil (carga SURGE) | 10 pts por projétil |
| Fase concluída sem morrer | Bônus +1.000 pts |
| Combo ×5 (5 kills sem tomar dano) | Multiplicador ×2 |
| Combo ×10 + absorção ativa | Multiplicador ×3 |
| Kill durante SURGE ativo | Multiplicador ×5 |
| Boss derrotado sem tomar dano | Multiplicador ×10 — Perfect Clear |

### 8.1 Ranking de Fase

| Rank | Critério |
|---|---|
| **S** | Pontuação ≥ 90% do máximo + zero mortes + HSP ≥ 90% |
| **A** | Pontuação ≥ 70% ou HSP ≥ 75% |
| **B** | Pontuação ≥ 50% ou fase concluída com até 1 morte |
| **C** | Fase concluída com até 2 mortes |
| **D** | Fase concluída com 3+ mortes ou via continue |


---

## 9. HUD e Interface

O HUD é desenhado via `glOrtho` em coordenadas de tela (overlay 2D sobre a cena 3D), sem fontes externas — texto via `glutBitmapCharacter`.

```
┌─────────────────────────────────────────────────────────┐
│  NANOCELL-1                 FASE 1 — Corrente Sanguínea │
│  VIDA   ███████░░  78%      HSP   ██████░░░  62%        │
│                                                         │
│  ● POLARIDADE: AZUL🔵        SURGE ████░░░░  47%        │
│                                                         │
│  SCORE: 48.200   COMBO: x3                              │
└─────────────────────────────────────────────────────────┘
```

| Elemento | Localização e Função |
|---|---|
| **Vida do NANOCELL-1** | Superior esquerdo — barra horizontal segmentada em 5 divisões |
| **Barra de SURGE** | Superior esquerdo — abaixo da vida. Cor muda conforme polaridade ativa |
| **Indicador de Polaridade** | Centro superior — círculo pulsante com cor e nome do estado ativo |
| **Medidor HSP** | Superior direito — barra vertical de 0% a 100% com valor numérico |
| **Pontuação** | Superior direito — atualizada em tempo real |
| **Combo counter** | Centro da tela — aparece quando combo ≥ 2, some após 2 s sem kill |
| **Timer de fase** | Inferior direito — contagem até spawn forçado do boss |
| **Nome da fase** | Inferior esquerdo — tag permanente durante o jogo |
| **Mensagem de evento** | Centro inferior — alertas como `BOSS EM APROXIMAÇÃO` por 2 s |

### 9.1 Telas do Jogo

- **Menu Principal:** título animado, opções Jogar / Controles/configs / Créditos / Sair
- **Pausa:** overlay semi-transparente — Continuar / Reiniciar Fase / Sair
- **Game Over:** animação de dissolução, pontuação final, botão de reinício
- **Resultado:** rank, pontuação, HSP final, fragmentos coletados, próxima fase
---
## 10. Recursos OpenGL Utilizados

| Recurso | Aplicação no Jogo |
|---|---|
| `glOrtho` — Projeção Ortográfica | Visão isométrica da arena. Escala uniforme sem perspectiva. |
| `GL_LIGHTING` — Iluminação | Ponto de luz central. Inimigos e nave com `GL_DIFFUSE` e `GL_SPECULAR`. |
| Gouraud Shading | Interpolação suave de cor por vértice nas membranas celulares e corpos dos inimigos. |
| `glBindTexture` — Texturização | Sprites de inimigos, projéteis e fundo orgânico como texturas PNG via `stb_image.h`. |
| `GL_BLEND` — Alpha Blending | Plasma, aura de polaridade e partículas com `GL_SRC_ALPHA + GL_ONE_MINUS_SRC_ALPHA`. |
| `glNewList` — Display Lists | Geometrias fixas (hemácias, alvéolos) pré-compiladas para reuso eficiente. |
| `GL_DEPTH_TEST` — Depth Buffer | Fundo 3D em z < 0, jogabilidade em z = 0, HUD em z > 0. |
| `GL_FOG` | Névoa volumétrica para simular profundidade do plasma e ambiente orgânico. |
| `glutBitmapCharacter` | Renderização de texto do HUD sem dependência externa de fontes. |
| `glScissor` / `glViewport` | Limitação de área de renderização para barras de HUD e tela de pausa. |

---

---

## 11. Arquitetura do Projeto

### 11.1 Estrutura de Arquivos

A arquitetura foi planejada de forma modular e orientada a objetos para separar a lógica de negócio das chamadas de baixo nível do OpenGL, facilitando a manutenção e a adequação aos requisitos de multiplataforma.

```text
NTF-CG-Jogo/
├── src/
│   ├── core/         # Motores base: Renderer (abstração OpenGL), Sistema de Janela, Input, Física
│   ├── entities/     # Lógica das entidades: Player (NANOCELL-1), Inimigos, Bosses, Projéteis
│   ├── scenes/       # Gerenciamento de Estados/Cenas: Menu, Fases (Corrente Sanguínea, etc)
│   ├── math/         # Abstrações matemáticas p/ transformações 2D/3D (Translação, Rotação, Escala)
│   └── ai/           # Lógica de Inteligência Artificial: Algoritmo de Boids e FSM dos Bosses
├── assets/
│   ├── textures/     # PNG: sprites, texturas orgânicas e elementos de HUD
│   ├── audio/        # OGG: trilhas sonoras · WAV: efeitos especiais (SFX)
│   └── models/       # OBJ: modelos 3D carregados via tinyobjloader
├── include/          # Headers de terceiros (header-only como stb_image e tinyobjloader)
├── Docs/             # Documentação de análise e referências teóricas da disciplina
└── README.md
```


### 11.2 Loop de Jogo

```
glutTimerFunc(16ms)
    └── update()
          ├── input processing (keyState[])
          ├── player.update()
          ├── enemies.update() → boids.update()
          ├── projectiles.update()
          ├── collision.check()
          ├── hsp.update()
          └── glutPostRedisplay()

glutDisplayFunc → render()
    ├── glClear()
    ├── draw3DScene()   ← fundo, inimigos, projéteis, player
    ├── drawParticles()
    ├── drawHUD()       ← overlay glOrtho
    └── glutSwapBuffers()
```

**Estados do jogo:** `MENU → PLAYING → PAUSED → GAMEOVER → RESULT`


### 11.3 Detecção de Colisão

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
## 12. Dependências e Compilação

### 12.1 Dependências

| Biblioteca | Uso |
|---|---|
| **OpenGL 3.3+** | Pipeline gráfico principal |
| **freeGLUT** | Janela, contexto OpenGL, input e timer |
| **GLEW** | Carregamento de extensões OpenGL |
| **SDL2** | Inicialização de áudio cross-platform |
| **SDL2_mixer** | Reprodução de OGG (música) e WAV (SFX) |
| **stb_image.h** | Carga de PNG/JPG para texturas (header-only) |
| **GLM** | Vetores e matrizes para Boids e transformações |

### 12.2 Instalação das Dependências

**Fedora / RHEL:**
```bash
sudo dnf install freeglut-devel SDL2-devel SDL2_mixer-devel glm-devel glew-devel
```

**Ubuntu / Debian:**
```bash
sudo apt install freeglut3-dev libsdl2-dev libsdl2-mixer-dev libglm-dev libglew-dev
```

**macOS (Homebrew):**
```bash
brew install freeglut sdl2 sdl2_mixer glm glew
```

### 12.3 Compilação

```bash
# Linux / macOS
make

# Cross-compile para Windows (MinGW)
make win

# Limpar build
make clean
```


### Makefile (resumo)

```makefile
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LIBS     = -lGL -lGLU -lglut -lGLEW -lSDL2_mixer
TARGET   = imunidade
```

### 12.4 Flags de Compilação

| Plataforma | Flags de Linker |
|---|---|
| **Linux / macOS** | `-lGL -lGLU -lfreeglut -lSDL2 -lSDL2_mixer -lGLEW` |
| **Windows (MinGW)** | `-lopengl32 -lglu32 -lfreeglut -lSDL2 -lSDL2_mixer -lglew32` |
| **Comuns** | `-std=c++17 -O2 -Wall` |


---

## 13. Referências e Inspirações

### 13.1 Jogos de Referência

| Jogo | Elemento Adotado |
|---|---|
| **Space Invaders** (Taito, 1978) | Estrutura base de shmup: oleadas, progressão por ondas, pontuação por kill |
| **Ikaruga** (Treasure, 2001) | Sistema de polaridade cromática (absorção vs. dano), mecânica de SURGE |
| **Galaga** (Namco, 1981) | Padrões de ataque em enxame, dive-bombing, formações de inimigos |
| **Xenon II Megablast** (Bitmap Bros, 1989) | Scroll com obstáculos orgânicos, power-ups e boss de fase |
| **Deluxe Galaga** (Amiga, 1994) | Sistema de pontuação com multiplicadores e combo |

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
