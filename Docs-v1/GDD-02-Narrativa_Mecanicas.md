# Imunidade — Documento de Design de Jogo (GDD v2.0)
## Documento 2: Narrativa, Mecânicas e Controles

---

### 1. Narrativa e Ambientação

#### A História do Paciente Zero
No ano de 2042, a corporação de bioengenharia "Entropia Corp" tentou criar um vírus projetado para devorar células cancerígenas. O experimento falhou catastroficamente. O **Príon Ômega**, apelidado de "A Praga", mutou, infectando o principal pesquisador: o Paciente Zero. 

A Praga não apenas destrói tecidos, ela assimila o DNA humano, construindo estruturas cristalinas bioluminescentes nas artérias. Medicamentos convencionais foram ignorados pelo parasita. A última esperança de conter uma pandemia de extinção global é o **Projeto NANOCELL-1**: um linfócito cibernético injetado diretamente na corrente sanguínea do Paciente Zero, capaz de utilizar o próprio metabolismo do vírus como arma.

O objetivo do jogador é descer pelo interior do corpo humano, rastrear o Núcleo Viral alojado no cérebro e destruí-lo antes que a Praga tome controle do sistema nervoso central e force o hospedeiro a quebrar sua quarentena.

#### Ambientação Visual (Bio-Neon)
O interior do corpo humano não é realista, mas sim uma representação "cibernética-neon". Parede das veias pulsam com efeitos de rasterização `Bresenham` rítmicos, glóbulos vermelhos flutuam ao fundo, e os vírus irradiam luz verde tóxica (Shading Gouraud emissivo).

---

### 2. Mecânicas de Jogo

#### 2.1 Movimentação Viscosa
O cenário representa fluidos orgânicos. A movimentação da nave não para instantaneamente. Há inércia calculada pelas matrizes de translação OpenGL, exigindo do jogador controle fino. A movimentação utiliza um padrão *Turn-then-Move* fluído.

#### 2.2 Ataque e Rasterização de Disparo
Ao invés de atirar lentos projéteis circulares, o jogador utiliza **Raios Laser Constantes**, desenhados diretamente no framebuffer usando o algoritmo de `Bresenham` ou geometria retangular esticada com matriz de escala.

#### 2.3 Mecânica de Polaridade (Core Gameplay)
A nave possui dois estados (Azul Frio e Vermelho Quente).
* **Escudo Ativado (Mesma Cor):** Se o jogador colidir com um projétil ou inimigo da MESMA cor da sua polaridade, ele absorve a ameaça. Isso não causa dano, recarrega a barra especial (Surge) e aumenta o combo.
* **Vulnerabilidade (Cor Oposta):** Colidir com um inimigo da cor OPOSTA causa dano letal à integridade da nave (HSP).
* **Dano Crítico:** Atirar com o laser de cor OPOSTA ao inimigo causa o dobro de dano, destruindo armaduras rapidamente.

#### 2.4 Evasão (Dash Cinético)
Pressionar espaço resulta em um impulso rápido na direção atual (Translação acelerada com rastro de partículas). O jogador ganha 0.4 segundos de invulnerabilidade (i-frames). Ideal para passar por lasers contínuos dos Chefões.

#### 2.5 Modo SURGE e Espalhamento Viral (Flood Fill)
Ao completar a barra de absorção, o jogador ativa o SURGE. O NANOCELL-1 dispara um ataque em área. 
Ao atingir os vírus inimigos, utilizamos o algoritmo **Flood Fill**: os inimigos que morrerem infectam os inimigos adjacentes próximos (em cadeia), causando uma explosão massiva na tela.

---

### 3. Controles e Mapeamento de Input

O jogo suporta teclado padrão com polling implementado nativamente via callbacks GLUT `glutKeyboardFunc` e `glutKeyboardUpFunc` para suporte cross-platform.

| Ação | Tecla | Efeito no Código |
| :--- | :--- | :--- |
| Mover Cima | `W` ou `Seta Cima` | Incrementa Transl Y `(0, 1, 0)` |
| Mover Baixo | `S` ou `Seta Baixo` | Decrementa Transl Y `(0, -1, 0)` |
| Mover Esquerda | `A` ou `Seta Esq` | Decrementa Transl X `(-1, 0, 0)` |
| Mover Direita | `D` ou `Seta Dir` | Incrementa Transl X `(1, 0, 0)` |
| Atirar Laser | `Mouse Esq` ou `Z` | Raycast Bresenham |
| Dash (Esquiva) | `Mouse Dir` ou `X` | Aceleração vetorial + invulnerabilidade |
| Alternar Polaridade | `Shift` ou `Espaço` | Alterna variável de estado `isBlue = !isBlue` e cor da luz Difusa |
| Ativar SURGE | `Q` ou `E` | Limpa barra de Surge, ativa Flood Fill chain-reaction |
| Pausar/Menu | `ESC` | Congela variável de tempo global |

---

### 4. Glossário

* **HSP (Host Survival Probability):** A "vida" do jogador. Se chegar a 0%, o Paciente Zero morre.
* **Polaridade:** O estado de energia eletromagnética do personagem (Azul ou Vermelho).
* **Surge:** O ataque supremo carregado ao absorver tiros com a polaridade correta.
* **Príon:** Proteínas anômalas, a classificação biológica dos inimigos no jogo.
* **i-frames:** Ponto de invulnerabilidade do Dash, onde não há checagem de colisão (AABB/Sphere desativado).
