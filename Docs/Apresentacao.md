# Roteiro de Apresentação — Imunidade: A Guerra Celular

Este documento consolida e mapeia **exatamente todos os pontos exigidos pelo Prof. Dr. Laurindo**, estruturando a sua apresentação e o relatório técnico da SBC para que não restem brechas para questionamentos.

---

## 1. Resumo
**Breve descrição do jogo:**
"Imunidade: A Guerra Celular" é um Shoot 'em Up (Shmup) com perspectiva isométrica 3D, ambientado no interior do corpo humano. O jogador controla o *NANOCELL-1*, um nanobô médico encarregado de erradicar o vírus sintético *Nexus-7*. O jogo se destaca pela mecânica de polaridade cromática (inspirada em Ikaruga), onde o estado da nave (Azul/Vermelho) determina quais projéteis ela absorve e quais causam dano.

---

## 2. Introdução
* **Contexto e Tema:** Um jogo biológico/tecnológico simulando a batalha celular de um organismo em colapso.
* **Regras:**
  1. Tiros da mesma cor que o jogador são absorvidos, gerando pontos de *SURGE* (Especial) e recuperando vida.
  2. Colidir com objetos da cor oposta causa dano severo e reduz a barra de "Saúde do Paciente" (HSP).
  3. Deixar inimigos passarem pela tela deduz o HSP, exigindo agressividade.
* **Motivação e Objetivos:** Romper o padrão de jogos estáticos através de um título frenético (*Bullet-Hell*) usando tecnologias clássicas do OpenGL (Fixed-Function Pipeline). O objetivo acadêmico é integrar IA avançada e detecção espacial à pipeline gráfica.

---

## 3. Materiais
**Bibliotecas utilizadas e suas finalidades:**
- **OpenGL (Fixed-Function 3.3+) & freeGLUT:** Motor de renderização primário, loop de eventos, criação de janelas e projeções (Perspectivas e Ortográficas).
- **GLEW:** Gerenciamento eficiente e carregamento de extensões avançadas do OpenGL.
- **SDL2 & SDL2_mixer:** Implementação unificada e assíncrona de Áudio (música OGG em streaming contínuo e efeitos de som WAV simultâneos).
- **tinyobjloader (Header-only):** Parser eficiente e robusto em C++ para importar malhas 3D (`.obj`) de inimigos e chefões de forma nativa.
- **stb_image.h (Header-only):** Biblioteca para carregamento instantâneo das texturas `.png` e `.jpg` direto para os buffers da GPU.

---

## 4. Métodos & Funcionalidades (O Coração da Apresentação)

*(Dica: Mostre o código abaixo para o professor e explique as lógicas!)*

### A. Movimentação e Visão
O jogo roda num mundo nativamente 3D mas com "Jogabilidade 2D" via projeção ortográfica. Limitamos o movimento aos eixos X e Z para criar o efeito *Top-Down*.
```cpp
// Projeção principal do 3D fixada como Isometrica/Ortográfica:
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-500.0, 500.0, -375.0, 375.0, -2000.0, 2000.0);
```

### B. Detecção de Colisões (Broadphase e Narrowphase)
Fizemos um salto além do tradicional laço $O(n^2)$. Implementamos uma arquitetura de particionamento espacial. Apenas entidades dentro do mesmo setor (Célula de Grid) testam colisões exatas usando Esferas Bounding (Narrowphase).
```cpp
// Exemplo Conceitual (Broadphase + Narrowphase)
float distSq = (inimigo.x - player.x) * (inimigo.x - player.x) + 
               (inimigo.z - player.z) * (inimigo.z - player.z);

float raioSoma = inimigo.raioColisao + player.raioColisao;

if (distSq <= (raioSoma * raioSoma)) {
    // Colisão Exata Detectada via Bounding Sphere!
    TratarDanoOuAbsorcao(inimigo.cor, player.polaridade);
}
```

### C. Inteligência Artificial (Boids e Buffer de Memória)
**O nosso grande diferencial.** O Vírus Alfa usa o algoritmo de **Boids** (Craig Reynolds) para simular comportamento de enxame de pássaros, dividindo o bando ao meio caso sejam bloqueados pelo jogador. Já o "Príon Mimético" da Fase 3 utiliza uma Fila Circular temporal, copiando o jogador com atraso.
```cpp
// Atualização de Memória do Príon Mimético (Atraso de 1,5 segundos a 60FPS = 90 posições)
posicoesAnteriores[headIndex] = {player.GetX(), player.GetZ()};
headIndex = (headIndex + 1) % maxBufferSize;

// O inimigo lê a posição que o player estava no passado e o segue:
targetX = posicoesAnteriores[tailIndex].x;
targetZ = posicoesAnteriores[tailIndex].z;
```

### D. Renderização 3D, Iluminação e Visibilidade
Usamos `GL_LIGHTING` e Modelos de Reflexão Phong via Pipeline Fixa, ativando `GL_DEPTH_TEST` (Z-Buffer) para ocultação de superfícies. Como as texturas chegam muitas vezes sem os vetores "Normais", **o jogo as calcula dinamicamente usando Produto Vetorial (Cross Product)** na carga!
```cpp
// Geração das normais dinâmicas de um triângulo usando Produto Vetorial:
float ux = v2.x - v1.x; float uy = v2.y - v1.y; float uz = v2.z - v1.z;
float vx = v3.x - v1.x; float vy = v3.y - v1.y; float vz = v3.z - v1.z;

// Produto vetorial Nx, Ny, Nz
float nx = uy * vz - uz * vy;
float ny = uz * vx - ux * vz;
float nz = ux * vy - uy * vx;

// Normaliza o vetor para luz perfeita no Gouraud Shading
float len = sqrt(nx*nx + ny*ny + nz*nz);
v1.nx = nx / len; // Aplica no vértice
```

---

### E. Aplicação Prática dos Conteúdos da Disciplina (Exemplos-Aulas)

#### 1. Transformações Geométricas e Hierárquicas (Aulas 07 e Robô)
Lembra do exercício do "Braço Robótico" ( Tarefa 07 ), onde você precisava usar `glPushMatrix` e `glPopMatrix` para rotacionar o antebraço sem afetar o ombro?
• **Onde aplicamos:** Fazemos isso dezenas de vezes a cada frame! Nós usamos as pilhas de matrizes para separar o sistema de câmera do HUD (Interface), e também dentro dos Inimigos (rotacionamos o corpo do Vírus com `glRotatef` enquanto transladamos ele pelo espaço com `glTranslatef` de forma completamente isolada).

#### 2. Projeções e Visibilidade (Aula 07 e 13)
• **Onde aplicamos:** Na inicialização da janela, não usamos a câmera perspectiva clássica (`gluPerspective`). Nós manipulamos ativamente a matriz de projeção (`glOrtho`) para achatar o eixo Y e transformar o mundo 3D num shmup isométrico de aspecto 2D, e usamos o Z-Buffer (`GL_DEPTH_TEST`) ensinado na disciplina para que os inimigos passem debaixo do HUD (interface de vida), resolvendo a oclusão visual.

#### 3. Modelagem Geométrica (Aula 22)
• **Onde aplicamos:** Em vez de desenharmos triângulos manuais no código como no arquivo `desenho3D.cpp` da aula 22, nós automatizamos a extração de vértices de arquivos `.obj` tridimensionais (usando a biblioteca `tinyobjloader`) e repassamos para a placa de vídeo desenhar centenas de milhares de triângulos (`GL_TRIANGLES`).

#### 4. Iluminação e Sombreamento / Gouraud Shading (Aula 22)
O professor focou bastante nos modelos matemáticos da luz em computação gráfica.
• **Onde aplicamos:** Usamos o modelo de Phong (`GL_LIGHTING`, `GL_LIGHT0`) e a interpolação suave de Gouraud (`GL_SMOOTH`). Mas fomos muito além: quando o `.obj` vem sem os vetores normais, nosso código em C++ implementa a Álgebra Linear nua e crua, fazendo o Produto Vetorial (Cross Product) matemático entre os vértices do triângulo para descobrir qual é a "Normal" (`Nx, Ny, Nz`) e assim refletir a luz corretamente. Esse é um algoritmo que impressiona qualquer professor de CG!

#### 5. Texturização e Blending (Unidade 3)
• **Onde aplicamos:** Fizemos a amostragem das coordenadas U, V dos modelos e implementamos o canal Alpha (Transparência matemática: `GL_BLEND` com `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`). É essa exata matemática que faz o sangue pulsar transparente no fundo e os escudos dos chefões parecerem vidro.

---

## 5. Diferenciais de Sucesso (Fale sobre isso!)
Se ele perguntar: "O que o projeto de vocês tem de tão especial?", cite:
1. **Engine Híbrida 3D/2D Dinâmica:** Nós isolamos as Pilhas de Matrizes (`glPushMatrix / glPopMatrix`). Renderizamos as camadas de Fundo (`glOrtho 2D`), as Entidades com Luz e Z-Buffer (`Perspectiva 3D`) e, por fim, a HUD e as Hitboxes (`Overlay UI puro`), **sem que um corrompa a matriz do outro**.
2. **Texturas Dinâmicas e Alpha Blending:** Para simular o plasma das veias, ativamos `GL_BLEND` manipulando o canal Alpha para os escudos dos Chefões.
3. **Display Lists para Otimização:** O chefão (Gigante Pneumococo) usa milhares de vértices. Para não engasgar o jogo em CPU lentas, alocamos os comandos pré-compilados diretamente na GPU via `glGenLists`, rodando o *Bullet-Hell* levemente a 60 FPS travados.

---

## 6. Resultados e Discussão
*(Espaço do relatório onde você vai colar de 3 a 5 **Prints (Capturas de Tela)** mostrando:*
- *A interface HUD funcionando no topo da tela;*
- *O raio laser SURGE destruindo dezenas de inimigos na Tela;*
- *O Boss final renderizado perfeitamente, provando o uso de malhas complexas texturizadas.*

## 7. Conclusão
* **Desafios Enfrentados:** A manipulação equivocada das Matrizes ModelView impedia as caixas de colisão de desenharem junto da textura rotacionada. Outro desafio foi balancear a colisão para que centenas de tiros não congelassem os *frames* da aplicação.
* **Soluções Adotadas:** Adotamos pilhas de renderização separadas para Câmera Fixa e HUD, e integramos o conceito de partição de matrizes.
* **Trabalhos Futuros:** Expandir o projeto com PBR (*Physically Based Rendering*) reescrevendo a pipeline para Shaders modernos (GLSL / OpenGL 4+) e suportar modo multijogador em rede.

## 8. Referências
1. REYNOLDS, Craig W. *Flocks, herds and schools: A distributed behavioral model.* ACM SIGGRAPH Computer Graphics, 1987.
2. FOLEY, J. et al. *Computer Graphics: Principles and Practice.* Addison-Wesley, 1990.
3. SHREINER, D. *OpenGL Programming Guide: The Official Guide to Learning OpenGL.* Pearson, 2013.
4. TREASURE. *Ikaruga.* (Jogo Eletrônico), 2001.
