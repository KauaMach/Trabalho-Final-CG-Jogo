# Imunidade — Documento de Design de Jogo (GDD v2.0)
## Documento 1: Visão Geral e Pilares

---

### 1. Ficha Técnica

| Categoria | Detalhe |
| :--- | :--- |
| **Título** | Imunidade: A Guerra Celular (Reboot) |
| **Gênero** | Top-Down Shooter / Bullet Hell Tático |
| **Plataforma** | PC (Linux, macOS, Windows) |
| **Tecnologia** | C++17, OpenGL (Pipeline Fixo), FreeGLUT |
| **Público-Alvo** | Jogadores hardcore, fãs de roguelikes e bullet hells (ex: Enter the Gungeon, Ikaruga) |
| **Perspectiva** | Top-Down 3D (Câmera Isométrica com `gluLookAt`) |
| **Classificação** | Livre (Violência fantasiosa microscópica) |

---

### 2. Visão Geral do Jogo

**Imunidade** é um Top-Down Shooter frenético que se passa em escala microscópica, no interior do corpo do "Paciente Zero". O jogador assume o controle do **NANOCELL-1**, um glóbulo branco cibernético experimental injetado para combater uma infecção viral alienígena de rápida mutação.

Ao contrário de shooters convencionais, a sobrevivência não depende apenas de reflexos rápidos, mas da manipulação da **Polaridade**. O NANOCELL-1 possui um escudo eletromagnético que pode alternar instantaneamente entre a frequência Azul (Fria) e Vermelha (Quente). Projéteis inimigos da mesma cor do escudo são absorvidos, recarregando a energia do jogador, enquanto projéteis da cor oposta causam dano letal.

O jogo é dividido em 5 Fases (Órgãos do corpo), culminando em batalhas épicas contra Chefões gigantes com membros articulados (mecânica herdeira do P1-CG: Braço Robótico).

---

### 3. Pilares de Design (Core Pillars)

1. **Dualidade de Polaridade (Ikaruga Style)**
   O núcleo do gameplay. O jogador deve estar constantemente lendo a tela e trocando de cor para transformar padrões de tiro impossíveis em fontes de recarga de energia. O jogo deve punir quem tenta apenas desviar e recompensar quem "mergulha" no perigo com a cor certa.

2. **Excelência Audiovisual e Cinética (C&C)**
   Todo impacto, tiro e absorção deve ter feedback extremo. Uso extensivo de *Screen Shake*, partículas com Blending perfeito (fade-out), flashes de tela invertendo cores no impacto e trilha sonora pulsante. O jogo deve "parecer" moderno e polido, mesmo rodando sobre um pipeline de renderização antigo.

3. **Arquitetura Baseada em Técnicas Fundamentais (P1, P2, P3)**
   O design do jogo foi concebido de trás para frente: para cada mecânica necessária, uma técnica de CG da disciplina foi resgatada para implementá-la. Animações hierárquicas, rasterização de Bresenham, Flood Fill e loaders de OBJ não são apenas requisitos técnicos, mas o cerne da jogabilidade.

---

### 4. Referências e Inspirações

* **Ikaruga (2001):** Principal inspiração mecânica. A troca de polaridades (preto/branco) inspirou diretamente a mecânica Azul/Vermelho de *Imunidade*.
* **Geometry Wars (2003):** Referência para a estética visual neon e brilhante. O uso de materiais emissivos (Gouraud Shading intensificado) e alto contraste contra um fundo escuro vêm daqui.
* **Enter the Gungeon (2016):** Referência para os controles precisos de Top-Down Shooter e a mecânica de evasão (Dash).
* **Osmos (2009):** Referência para a física de flutuação e movimentação viscosa no interior das veias.

---