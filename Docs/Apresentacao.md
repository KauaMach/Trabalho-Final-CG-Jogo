# Roteiro de Apresentação — Imunidade: A Guerra Celular

Este documento consolida e mapeia **todos os pontos técnicos e conceituais exigidos**, estruturando a sua apresentação e o relatório técnico da disciplina de Computação Gráfica para que você tenha todas as respostas na ponta da língua e se destaque perante os questionamentos do professor.

---

## 1. Resumo e Pitch do Projeto
**O que é o projeto?**
"Imunidade: A Guerra Celular" é um jogo de tiro espacial (*Shoot 'em Up / Bullet-Hell*) com perspectiva isométrica 3D, ambientado no interior do corpo humano. O jogador controla o *NANOCELL-1*, um nanobô médico que deve erradicar vírus sintéticos e superbactérias.

**Diferencial Mecânico:** A jogabilidade é baseada em *Polaridade Cromática* (inspirada no clássico Ikaruga):
- **Polaridade Azul:** Absorve tiros azuis (curando e gerando carga pro especial), mas toma dano fatal de elementos vermelhos. Causam dano bônus em inimigos vermelhos.
- **Polaridade Vermelha:** A mesma regra invertida (absorve o vermelho, sofre com o azul).
- A troca de polaridade é o pilar que exige pensamento estratégico e agilidade visual do jogador, fugindo da repetição comum no gênero.

---

## 2. Materiais e Tecnologias
Se o professor perguntar o porquê de cada tecnologia:
- **OpenGL (Fixed-Function 3.3+) & freeGLUT:** O motor gráfico base. Usado para desenhar na tela, capturar eventos de teclado/mouse e gerenciar matrizes de visualização e projeção. O uso intencional da Pipeline Fixa serve para aplicar diretamente os conceitos matemáticos ensinados em sala.
- **GLEW:** Fundamental para acessar extensões modernas de hardware da placa de vídeo não inclusas nos headers padrão.
- **SDL2 & SDL2_mixer:** A solução de áudio. Permite o *multithreading* acústico. Tocamos a música (streaming) assincronamente em um canal paralelo, enquanto alocamos dúzias de "chunks" sonoros instantâneos (lasers e colisões) sem gargalar o laço principal de 60 FPS.
- **tinyobjloader:** Um parser em C++ robusto para mapear arquivos modelados em 3D (`.obj`) em vetores brutos compreensíveis pela engine do jogo (vértices e texturas).
- **stb_image.h:** Decodificação veloz e direta de `.png` e `.jpg`, repassando arrays de pixels instantaneamente para a memória de vídeo (VRAM).

---

## 3. Arquitetura de Software e Padrões (Pontos Extras)
Apresentar uma arquitetura limpa em C++ é sinônimo de maturidade:
- **Máquina de Estados Finita (FSM):** O laço da aplicação é blindado por um enumerador `GameState` (`STATE_MENU`, `STATE_PLAYING`, `STATE_PAUSE`, etc.). Isso extingue o risco de entradas (cliques e tiros) "vazarem" entre contextos diferentes (exemplo: atirar dentro do menu foi proibido graças a esta máquina).
- **Herança e Polimorfismo:** A classe abstrata `Inimigo` define métodos virtuais. Assim, enfileiramos os comportamentos mais exóticos (Chefões, Vírus Delta, Kamikazes) em uma simples e única lista `std::vector<Inimigo*>`, executando colisões e atualizações dinamicamente (*Dynamic Dispatch*).

---

## 4. Métodos & Funcionalidades (O Coração da Apresentação)

### A. Projeção Híbrida 3D/2D
**Possível pergunta:** *"Se o jogo aparenta ser 2D, por que usaram estruturas 3D?"*
**Resposta:** Para gerar profundidade e sombreamento reais! Restringimos a movimentação espacial livre apenas aos eixos X e Z para a navegação, e gerenciamos a "câmera" com maestria. O segredo está em empilhar matrizes (`glPushMatrix`) para desenhar o cenário paralax, depois a cena 3D (inimigos) e, por último e totalmente sobreposto em 2D, os painéis da HUD de vida.

### B. Otimização de Renderização (Display Lists)
**Possível pergunta:** *"Como garantiram desempenho ao desenhar inimigos de milhares de polígonos usando o clássico `glBegin/glEnd`?"*
**Resposta:** Nós *não* usamos `glBegin` todo quadro para malhas complexas. Nós compilamos os vértices de cada modelo na GPU na tela de carregamento (via **Display Lists** / `glGenLists`). Durante o jogo, apenas invocamos `glCallList()`, que repassa o fardo para a placa de vídeo.

### C. Cálculos Algébricos em Tempo Real (Gouraud e Produto Vetorial)
**Possível pergunta:** *"Onde está a computação gráfica matemática pura de vocês?"*
**Resposta:** Quando carregamos um modelo que não possui Vetores Normais para cálculo de iluminação. Nós aplicamos a Álgebra Linear aplicando o **Produto Vetorial (Cross Product)** nos três vértices da face geométrica para calcular a normal ortogonal dinamicamente e a enviamos ao motor de sombreamento Phong / Gouraud Shading (`GL_LIGHT0`, `GL_SMOOTH`).

### D. Sistema Eficiente de Colisão e Particionamento Básico
Usamos **Bounding Spheres** para evitar checar triângulo por triângulo. Elevamos as posições X/Z ao quadrado e subtraímos da soma dos raios para evitar a lentidão da raiz quadrada (`sqrt()`) dentro do laço O(n²), cruzando as hitboxes apenas quando o cálculo escalar exige.

### E. Inteligência Artificial: Boids e Fila Temporal
- **Vírus Alfa:** Programado em torno da lógica dos **Boids**, ele possui uma diretriz de flocular e caçar em revoada, imitando bando de pássaros que se adaptam se o jogador entrar no meio.
- **Esporos e Pneumococos:** Usam máquinas de estados locais (Patrulha -> Puxão Gravitacional -> Fúria).
- **Príon Mimético:** O inimigo mais letal. Usa um Fila Circular de dados (*Circular Deque Buffer*) para memorizar posições X/Z que você pisou no passado e trilhar o mesmo caminho de forma atrasada e implacável.

### F. Gestão de Memória: Animações de Cenário
Não criamos um único fundo gigantesco. A "Animação de Fundo" (Backgrounds) é controlada carregando até 240 quadros leves que intercalam em função da variável temporal base (ex: `gameTimer`), calculando seu índice exato a 60 FPS, dando a ilusão de voar sobre tecidos biológicos.

---

## 5. Casamento com a Ementa da Disciplina (Como convencer o professor)

1. **Transformações Geométricas e Hierarquia:** Assim como no exemplo do "Robô" ministrado em sala, rotacionamos (`glRotatef`) a entidade sobre seu próprio eixo local de forma desvinculada de sua translação global pelo mapa (`glTranslatef`), encapsulando os estados para não torcer outros elementos na tela.
2. **Projeções e Visibilidade:** Lidamos duramente com o temido Z-Buffer (`GL_DEPTH_TEST`). Calibramos recortes exatos da câmera para que a tela não sofresse *Z-fighting* (intermitência visual) em objetos distantes.
3. **Modelagem:** Em vez de *hardcodar* vértices brutos no C++, nós criamos uma ponte programável com o software *Blender*, puxando os modelos exportados e conectando-os no OpenGL.
4. **Texturização e Blending (Unidade 3):** Exploramos o canal Alpha (transparência matemática `GL_BLEND` com `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`). Essa matemática funde o pixel de fundo com o da frente, construindo "escudos" de plasma visualmente translúcidos e vívidos, algo muito distante de blocos retangulares estáticos.
5. **Matrizes (Push/Pop):** Controlamos a separação das HUDs, onde a interface com o slider de menu e a barra de sangue nunca afundam na profundidade porque a matriz ortográfica sobrepõe as definições da perspectiva isométrica 3D do restante do jogo.

---

## 6. Desafios e Soluções (Fechamento Prático)
- **Desafio:** Conflito de Matriz ModelView. A HUD (Barra de Vida, Tiros) sumia ou rodava junta do player na tela.
- **Solução:** Aplicamos corretamente o reset de matriz (`glLoadIdentity`) e particionamos em blocos as renderizações UI x Cenário. 
- **Desafio:** Entradas de teclado indesejadas (tiros, sons da nave) explodindo na tela do Menu Inicial e Seleção.
- **Solução:** Controle de Estado. Redirecionamos os fluxos de hardware amarrando os laços do teclado rigidamente nas validações `if(currentState == STATE_PLAYING)`.
- **Trabalhos Futuros:** Expandir para a pipeline moderna de Shaders programáveis em GLSL e adicionar iluminação fisicamente baseada (*PBR*).

---
> **DICA DE OURO PARA APRESENTAR:** Seja enérgico! Mostre confiança apontando os trechos em que vocês misturaram Inteligência Artificial (Boids e Buffer) com Física e Matemática de Cores. O professor não avalia apenas se a tela liga, mas se vocês entenderam **o que acontece na matemática por trás de cada linha do OpenGL.**
