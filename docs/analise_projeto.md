# 🔬 Análise Completa — IMUNIDADE: A Guerra Celular

> **Trabalho Final de Computação Gráfica · UFPI 2026.1**
> Projeto: Shoot 'em up 3D/2D com OpenGL + freeGLUT em C++17

---

## 📊 Visão Geral do Projeto

| Aspecto | Detalhe |
|---|---|
| **Linguagem** | C++17 |
| **API Gráfica** | OpenGL (Legacy/Fixed-Pipeline) via freeGLUT |
| **Áudio** | SDL2 + SDL2_mixer |
| **Build System** | Makefile (cross-platform Windows/Linux) |
| **Plataformas** | Linux + Windows |
| **Código real** | ~515 linhas em `main.cpp` + ~600 linhas em classes + ~84.000 linhas de mesh gerada |
| **Binário compilado** | ~6.4MB (Linux), ~7.1MB (Windows) |

---

## 🏗️ Arquitetura Real vs. Documentada

### Estrutura Real
```
Trabalho-Final-CG-Jogo/
├── src/
│   ├── main.cpp                    (515 linhas — loop principal, game logic, callbacks)
│   ├── classes/
│   │   ├── renderer.cpp            (151 linhas — texturas, cenário, HUD, iluminação)
│   │   ├── renderer_menu.cpp       (50 linhas — telas de menu)
│   │   ├── renderer_player.cpp     (24.764 linhas — mesh 3D do player gerada)
│   │   ├── renderer_virus1.cpp     (9.224 linhas — mesh vírus tipo 1)
│   │   ├── renderer_virus2.cpp     (10.790 linhas — mesh vírus tipo 2)
│   │   ├── renderer_virus3.cpp     (19.593 linhas — mesh vírus tipo 3)
│   │   ├── renderer_virus4.cpp     (19.359 linhas — mesh vírus tipo 4)
│   │   ├── collision.cpp           (73 linhas — detecção de colisão + spatial hash)
│   │   ├── ai_boids.cpp            (134 linhas — IA de enxame)
│   │   ├── ai_fsm.cpp              (74 linhas — FSM dos bosses)
│   │   ├── particles.cpp           (86 linhas — sistema de partículas)
│   │   └── audio.cpp               (45 linhas — gerenciamento de áudio)
│   └── bibliotecas/                (headers + stb_image.h)
├── assets/textures/                (6 PNGs: menu, instruções, créditos, vitória, derrota, fim)
├── conveter.py                     (55 linhas — conversor OBJ → C++)
├── Makefile
└── README.md
```

> [!IMPORTANT]
> A arquitetura real difere significativamente da descrita no README. O README documenta uma estrutura modular com `entities/`, `scenes/`, `include/`, `models/`, `audio/` — mas **nenhum destes diretórios existe**. A implementação real é mais centralizada.

### Diferenças Arquiteturais

| README descreve | Realidade |
|---|---|
| `entities/player.cpp`, `enemy.cpp`, `boss.cpp`, `projectile.cpp` | Toda lógica de entidades está em `main.cpp` |
| `scenes/scene_01_blood.cpp` ... `scene_05_core.cpp` | Não existem — cenas são gerenciadas em `main.cpp` |
| `include/` (headers de terceiros) | Apenas `stb_image.h` em `src/bibliotecas/` |
| `assets/audio/` e `assets/models/` | Não existem como diretórios |
| `renderer.cpp` como pipeline completo | Dividido em 7 arquivos renderer_* |
| GLM, GLEW, tinyobjloader como dependências | **Não utilizados** — apenas OpenGL legacy e `stb_image` |

---

## 📂 Análise Detalhada por Arquivo

### 1. [main.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/main.cpp) — Núcleo do Jogo (515 linhas)

O arquivo central. Contém **toda a lógica do jogo**:

**Estrutura de estados (enum `TelaEstado`):**
| ID | Estado | Descrição |
|---|---|---|
| 0 | `TELA_MENU` | Menu principal |
| 1–5 | `FASE_1_SANGUE` ... `FASE_5_NUCLEO_VIRAL` | 5 fases de gameplay |
| 6 | `TELA_VITORIA` | Tela de vitória |
| 7 | `TELA_DERROTA` | Tela de derrota |
| 8 | `TELA_CREDITOS` | Créditos |
| 9 | `TELA_FIM` | Tela final |
| 10 | `TELA_INSTRUCOES` | Instruções |

**Variáveis globais de estado:**
- `playerX`, `playerY` — posição do jogador
- `playerPolaridade` — AZUL ou VERMELHA
- `barraSurge` — medidor do super-ataque (0–100%)
- `hsp` — saúde do paciente (inicia em 100)
- `score`, `combo`, `dnaColetado` — pontuação

**Callbacks GLUT registrados:**
- `DisplayLoop()` — renderização (menus via textura OU cena 3D com câmera, materiais, modelos, partículas, HUD)
- `TimerPhysics()` — atualização a ~60fps (16ms): Boids, contenção de fronteiras, colisões, progressão de fase, SURGE
- `KeyboardDown()` — WASD (passo 0.25), Space (ataque), Q (SURGE), ESC (sair)
- `SpecialKeys()` — Shift para trocar polaridade
- `MouseClick()` — 4 botões de menu com coordenadas hardcoded em pixels

**Mecânicas implementadas:**
- ✅ 5 fases com progressão automática (quando todos os vírus morrem)
- ✅ Troca de polaridade (Azul ↔ Vermelho)
- ✅ SURGE: carrega a 0.1/tick, gasta a 0.8/tick quando ativo, ativa fuga dos Boids
- ✅ HSP (Saúde do Paciente) com check de derrota
- ✅ Spawn de 20 boids iniciais por fase

> [!CAUTION]
> **Bug crítico:** O ataque (Space → matar vírus) está dentro de `#ifdef _WIN32` usando `GetAsyncKeyState(VK_SPACE)`. **No Linux, o Space dispara partículas e som, mas NÃO mata vírus.** O jogo é efetivamente injogável no Linux.

### 2. [renderer.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/renderer.cpp) — Renderização (151 linhas)

**Funções principais:**
- `InicializarGL()` — Clear color preto, depth test, `GL_LIGHTING`, `GL_LIGHT0`, `GL_NORMALIZE`, `GL_COLOR_MATERIAL` (front+back), ambient global 0.3
- `ConfigurarCamera()` — Perspectiva 45° FOV, near=0.1, far=100, câmera em (0,0,15) olhando para a origem
- `AtualizarIluminacaoDinamica()` — `GL_LIGHT0` com difusa/especular branca full, posição (0,5,4)
- `CarregarTextura()` — Usa `stb_image`, filtragem bilinear (`GL_LINEAR`), suporta RGB e RGBA
- `RenderizarTextoHUD()` — Ortho 800×600, desabilita iluminação, `glutBitmapCharacter`
- `DesenharEsferaGouraud()` — Esfera via `GL_QUAD_STRIP` com normais para Gouraud shading

> [!WARNING]
> - `AtualizarIluminacaoDinamica()` recebe `polaridade` e posição do jogador como parâmetros, mas **ignora ambos completamente**. A iluminação nunca muda com a polaridade.
> - O HUD usa projeção ortho 800×600 mas a janela é 1024×768 — coordenadas desalinhadas.
> - Material especular é configurado duas vezes consecutivas com os mesmos valores (redundância).

### 3. [renderer_menu.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/renderer_menu.cpp) — Menus (50 linhas)

- `InicializarTexturaEstado()` — Mapeia textura para 1 de 6 slots de estado
- `RenderizarTelaEstado()` — Quad texturizado em tela cheia (ortho 1024×768)
- Limpo e bem implementado. Desabilita iluminação, habilita `GL_TEXTURE_2D`, restaura estado ao sair.

### 4. [collision.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/collision.cpp) — Colisão (73 linhas)

**Implementado:**
- **`SpatialHashGrid`** — Grid espacial via `unordered_map` com hash polinomial (`ix * 73856093 ^ iy * 19349663`). Suporta insert, clear e query 9-vizinhos.
- **`CollisionEngine`** (métodos estáticos):
  - `ChecarAABB()` — overlap retangular
  - `ChecarEsferaParaEsfera()` — distância² ≤ somaRaios²
  - `ChecarMistas()` — esfera vs. AABB (closest point on box)
- **`ColisaoObjeto`** — struct com tipo (0=Player, 1=Enemy, 2=Projectile, 3=PowerUp)

> [!WARNING]
> O `SpatialHashGrid` está **totalmente implementado mas nunca é usado** em `main.cpp`. A colisão no jogo é feita por força bruta O(n). Apenas `ChecarEsferaParaEsfera()` é chamado na prática. `ChecarAABB()` e `ChecarMistas()` nunca são invocados.

### 5. [ai_boids.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/ai_boids.cpp) — IA de Enxame (134 linhas)

**Classe `BoidsEngine` — implementação correta de Reynolds (1987):**

| Regra | Peso | Raio | Implementação |
|---|---|---|---|
| **Separação** | 1.5 | 2.5 | Repulsão inverso-quadrática |
| **Alinhamento** | 1.0 | 2.5 | Média de velocidade dos vizinhos |
| **Coesão** | 1.0 | 2.5 | Atração para centroide do grupo |
| **Perseguição** | 0.8 | — | Seek steering toward player |
| **Fuga SURGE** | 2.5 | 6.0 | Flee com panic radius e 1.5× boost |

**Funções auxiliares:**
- `AplicarForcaProcura()` — Seek: velocidade desejada → steering force → limitação
- `AplicarForcaFuga()` — Flee: inverso do seek, com raio de pânico e boost

**Qualidade:** Implementação correta com normalização, limitação de forças e clamping de velocidade. Complexidade O(n²) — aceitável para 20 boids.

### 6. [ai_fsm.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/ai_fsm.cpp) — FSM dos Bosses (74 linhas)

**Classe `BossFSM` — 4 estados baseados em vida:**

| Estado | Vida | Velocidade | Freq. Tiro | Padrão |
|---|---|---|---|---|
| `PATRULHA` | > 75% | 1.0× | 1.5s | Linear |
| `AGRESSIVO` | 40–75% | 1.4× | 0.9s | Leque triplo |
| `FURIA` | 15–40% | 1.9× | 0.4s | Radial 360° |
| `ADAPTACAO` | < 15% | 2.2× | 0.2s | Homing preditivo |

> [!WARNING]
> A FSM está **totalmente implementada mas nunca é usada** em `main.cpp`. Não existe nenhuma entidade Boss no jogo atual. É código preparado mas não integrado.

### 7. [particles.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/particles.cpp) — Partículas (86 linhas)

**Design:**
- Pool estático de 1000 partículas (sem alocação heap)
- `EmitirExplosao()` — até 60 partículas por explosão, distribuição radial trigonométrica, velocidades aleatórias (0.05–0.20)
- `AtualizarERenderizarParticulas()` — passo único: atualiza posições (Euler), decrementa vida, desativa mortas, renderiza como `GL_POINTS` com fade-out via alpha

> [!WARNING]
> Usa `glColor4f` com canal alpha para fade-out, mas **nunca habilita `GL_BLEND`** (`glEnable(GL_BLEND)` e `glBlendFunc` não são chamados). As partículas aparecem 100% opacas e desaparecem instantaneamente em vez de fazer fade suave.

### 8. [audio.cpp](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/src/classes/audio.cpp) — Áudio (45 linhas)

**Classe `AudioManager`:**
- `InicializarAudio()` — Inicia SDL audio, abre mixer (44100Hz, stereo, buffer 2048), carrega música `.ogg` + 2 SFX `.wav`, inicia loop de música
- `TocarLaser()` / `TocarSurge()` — Efeitos em canal disponível
- `LimparAudio()` — Libera recursos corretamente

**Implementação limpa e correta** — null checks, cleanup no destrutor, gerenciamento adequado de recursos.

### 9. [conveter.py](file:///home/kz/Documentos/UFPI/CG/Trabalho-Final-CG-Jogo/conveter.py) — Conversor OBJ→C++ (55 linhas)

Converte modelos `.obj` em funções C++ com `glBegin(GL_TRIANGLES)` e milhares de `glVertex3f` hardcoded.

> [!CAUTION]
> **O conversor NÃO gera normais** (`glNormal3f`). Os 5 modelos 3D (player + 4 vírus) são renderizados **sem normais por vértice**, o que significa que a iluminação Gouraud configurada no renderer produz resultados imprevisíveis/incorretos nessas meshes. Sem normais, `GL_LIGHTING` não pode calcular shading adequadamente.

---

## ✅ Recursos OpenGL Efetivamente Utilizados

| Recurso | Status | Onde | Funciona? |
|---|---|---|---|
| **Texturas 2D** (`GL_TEXTURE_2D`) | ✅ Implementado | `renderer.cpp`, menus | ✅ Sim |
| **Iluminação** (`GL_LIGHTING`, `GL_LIGHT0`) | ✅ Implementado | `renderer.cpp` | ⚠️ Parcial — não muda com polaridade |
| **`GL_COLOR_MATERIAL`** | ✅ Implementado | `renderer.cpp` | ✅ Sim |
| **`GL_NORMALIZE`** | ✅ Implementado | `renderer.cpp` | ⚠️ Ineficaz sem normais nos modelos |
| **Gouraud Shading** | ✅ Implementado | `DesenharEsferaGouraud()` | ✅ Na esfera, ❌ nos OBJs |
| **Depth Buffer** (`GL_DEPTH_TEST`) | ✅ Implementado | `renderer.cpp` | ✅ Sim |
| **Projeção Perspectiva** | ✅ Implementado | `gluPerspective(45°)` | ✅ Sim |
| **Projeção Ortográfica** | ✅ Implementado | `gluOrtho2D` (HUD + menus) | ⚠️ Mismatch 800×600 vs 1024×768 |
| **Materiais** | ✅ Implementado | Ambient, diffuse, specular, shininess | ✅ Sim |
| **`GL_QUAD_STRIP`** | ✅ Implementado | Esfera Gouraud | ✅ Sim |
| **`GL_TRIANGLES`** | ✅ Implementado | Meshes 3D | ✅ Sim |
| **`GL_POINTS`** | ✅ Implementado | Partículas | ✅ Sim |
| **Blending** (`GL_BLEND`) | ⚠️ Parcial | Menus sim, partículas **não** | ⚠️ |
| **Spatial Hashing** | ✅ Codificado | `collision.cpp` | ❌ Nunca chamado |
| **Display Lists / VBOs** | ❌ Ausente | — | — |

---

## 🐛 Bugs Críticos Encontrados

### 1. ❌ Ataque não funciona no Linux
```cpp
#ifdef _WIN32
    if (GetAsyncKeyState(VK_SPACE)) { /* mata vírus */ }
#endif
```
O Space dispara partículas e som, mas a lógica de dano está dentro de `#ifdef _WIN32`. **O jogo é injogável no Linux.**

### 2. ❌ Modelos 3D sem normais
O `conveter.py` **não gera `glNormal3f`**. Os 5 modelos (player + 4 vírus) não têm normais, então `GL_LIGHTING` não calcula shading corretamente neles.

### 3. ❌ Partículas sem transparência
`particles.cpp` usa `glColor4f` com alpha mas **nunca chama `glEnable(GL_BLEND)`**. As partículas ficam opacas e desaparecem instantaneamente.

### 4. ❌ Iluminação dinâmica ignora polaridade
`AtualizarIluminacaoDinamica()` recebe `polaridade` e posição do jogador como parâmetros, mas **não os usa**. A luz é sempre branca em (0,5,4).

### 5. ⚠️ Botões de menu hardcoded
Coordenadas de pixel dos botões são fixas para 1024×768. Redimensionar a janela quebra os cliques.

### 6. ⚠️ HUD com resolução errada
Ortho do HUD é 800×600 mas a janela é 1024×768 → texto desalinhado.

### 7. ⚠️ Combo nunca usado
A variável `combo` é declarada mas **nunca é modificada ou exibida** em nenhum lugar do código.

### 8. ⚠️ Textura array inicialização incompleta
O construtor inicializa `texturasID[4]` com loop `for(i<4)`, mas o header declara `unsigned int texturasID[6]` — 2 slots ficam sem inicializar.

---

## ✅ Mecânicas de Jogo — Status Real

| Mecânica | Status | Funciona? |
|---|---|---|
| Polaridade Cromática (Azul ↔ Vermelho) | ✅ Implementada | ✅ Sim |
| Troca de polaridade (Shift) | ✅ Implementada | ✅ Sim |
| SURGE (carregar + ativar) | ✅ Implementada | ✅ Sim |
| Fuga dos Boids quando SURGE ativo | ✅ Implementada | ✅ Sim |
| HSP (Saúde do Paciente) | ✅ Implementada | ✅ Sim |
| 5 Fases com progressão | ✅ Implementada | ✅ Sim |
| Movimentação WASD | ✅ Implementada | ✅ Sim |
| Áudio (música + SFX) | ✅ Implementada | ✅ Sim |
| Ataque (matar vírus) | ✅ Implementada | ❌ Só no Windows |
| Bosses | ❌ FSM codificada | ❌ Nunca integrada |
| Combo/Multiplicadores | ❌ Declarada | ❌ Nunca usada |
| Power-ups | ❌ Não encontrada | ❌ Não implementada |
| Dash de Evasão | ❌ Não encontrada | ❌ Não implementada |
| Árvore de Upgrades | ❌ Não encontrada | ❌ Não implementada |
| Absorção de projéteis | ❌ Não encontrada | ❌ Não implementada |
| Ranking S–D | ❌ Não encontrada | ❌ Não implementada |
| Príon Mimético (buffer circular) | ❌ Não encontrada | ❌ Não implementada |

---

## 🔍 Pontos Fortes

1. **Conceito temático excelente** — A ambientação biológica é original e criativa.
2. **Algoritmo de Boids sólido** — Implementação fiel de Reynolds com 5 regras e pesos configuráveis.
3. **Modelos 3D reais** — Pipeline de conversão OBJ→C++ funcional.
4. **Áudio bem implementado** — SDL2_mixer com cleanup correto.
5. **Documentação README excepcional** — Extremamente detalhada e profissional.
6. **Cross-platform no Makefile** — Detecção automática Windows/Linux.
7. **Máquina de estados de jogo** — 11 estados bem definidos com transições claras.
8. **Assets visuais** — 6 texturas para telas de menu/estado de alta qualidade.

## ⚠️ Pontos Críticos

### Categoria A — Bugs que impedem o jogo
| # | Problema | Impacto |
|---|---|---|
| 1 | Ataque só funciona no Windows | Jogo injogável no Linux |
| 2 | Modelos 3D sem normais | Shading incorreto em todos os modelos |
| 3 | Partículas sem blending | Feedback visual quebrado |

### Categoria B — Código morto / Não integrado
| # | Problema | Observação |
|---|---|---|
| 4 | `BossFSM` nunca usada | 74 linhas de código sem efeito |
| 5 | `SpatialHashGrid` nunca usado | 73 linhas de código sem efeito |
| 6 | `ChecarAABB()` e `ChecarMistas()` nunca chamados | Apenas sphere-sphere é usado |
| 7 | Iluminação dinâmica não é dinâmica | Parâmetros ignorados |
| 8 | `combo` e `Vector3` declarados mas nunca usados | Variáveis mortas |

### Categoria C — Divergências com a documentação
| # | README diz | Realidade |
|---|---|---|
| 9 | 6 power-ups orgânicos | Não implementados |
| 10 | Dash de evasão com i-frames | Não implementado |
| 11 | Absorção de projéteis da mesma cor | Não implementado |
| 12 | Ranking S–D por fase | Não implementado |
| 13 | Árvore de upgrades com DNA | Não implementada |
| 14 | GLM, GLEW, tinyobjloader | Não utilizados |
| 15 | Estrutura modular (entities/, scenes/) | Código centralizado em main.cpp |

---

## 📈 Sugestões de Melhoria (por prioridade)

### 🔴 Prioridade Crítica
1. **Corrigir ataque no Linux** — Substituir `GetAsyncKeyState` por flag no callback `KeyboardDown`/`KeyboardUp` do GLUT (já existente para WASD).
2. **Gerar normais no conversor** — Adicionar cálculo de face normals no `conveter.py` e regenerar as meshes.
3. **Habilitar blending nas partículas** — Adicionar `glEnable(GL_BLEND)` + `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` em `particles.cpp`.

### 🟡 Prioridade Alta
4. **Integrar a FSM dos bosses** — Criar entidade Boss em `main.cpp` que usa `BossFSM`.
5. **Usar o Spatial Hash** — Substituir colisão bruta pelo `SpatialHashGrid` já implementado.
6. **Implementar iluminação dinâmica** — Usar os parâmetros de polaridade em `AtualizarIluminacaoDinamica()`.
7. **Corrigir resolução do HUD** — Unificar ortho para 1024×768.

### 🟢 Prioridade Média
8. **Usar Display Lists** — Envolver meshes 3D em `glNewList/glEndList`.
9. **Ativar `-O2`** — Mudar otimização no Makefile.
10. **Modularizar `main.cpp`** — Extrair entidades para classes separadas.
11. **Implementar mecânicas faltantes** — Power-ups, dash, absorção, combos.

---

## 🎯 Avaliação Final

| Critério | Nota | Comentário |
|---|---|---|
| **Conceito e Design** | ⭐⭐⭐⭐⭐ | Temática excelente, bem fundamentada |
| **Documentação** | ⭐⭐⭐⭐⭐ | README excepcional (mas diverge do código) |
| **Uso de OpenGL** | ⭐⭐⭐ | Texturas, iluminação, projeções OK; normais e blending com problemas |
| **IA (Boids)** | ⭐⭐⭐⭐⭐ | Implementação fiel e funcional |
| **IA (FSM)** | ⭐⭐⭐ | Bem codificada, mas não integrada |
| **Mecânicas de Jogo** | ⭐⭐⭐ | Polaridade e SURGE funcionam; várias mecânicas ausentes |
| **Completude** | ⭐⭐⭐ | ~40% das features do README estão implementadas |
| **Qualidade de Código** | ⭐⭐⭐ | Funcional mas monolítico, com código morto |
| **Portabilidade** | ⭐⭐ | Bug crítico impede gameplay no Linux |
| **Áudio** | ⭐⭐⭐⭐ | Implementação limpa com SDL2_mixer |

### Resumo
O projeto tem um **conceito brilhante** e uma **documentação exemplar**, com implementações de qualidade em áreas específicas (Boids, áudio, sistema de estados). No entanto, há uma **lacuna significativa entre o que é documentado e o que está implementado** — cerca de 40% das mecânicas descritas existem no código. Os 3 bugs críticos (ataque no Linux, normais ausentes, blending) precisam de correção imediata para o jogo funcionar como esperado.

> [!NOTE]
> O projeto demonstra domínio dos conceitos de Computação Gráfica exigidos pela disciplina: texturas, iluminação, shading, blending, projeções (perspectiva e ortográfica), transformações geométricas, e modelos 3D. O sistema de IA com Boids é um diferencial notável. As correções prioritárias (ataque no Linux, normais, blending) são relativamente simples e teriam grande impacto na qualidade final.
