# Análise do Material de Referência da Disciplina (Documentos e Exemplos-Aulas)

Esta análise tem como base os materiais em PDF e os códigos das aulas (ex: `codigoexampeloopengl.txt`), servindo como bússola para o desenvolvimento do projeto. O objetivo é unir as exigências teóricas da disciplina com uma arquitetura bem implementada e compatibilidade com múltiplos sistemas operacionais (Linux, macOS, Windows).

## 1. Tecnologias Base e Multiplataforma
O material de aula apresenta uma abordagem voltada ao **OpenGL Clássico (Pipeline Fixo)** usando primitivas básicas como `glBegin()`/`glEnd()`, em conjunto com a biblioteca `freeGLUT` para a manipulação da janela e eventos.
O código de exemplo evidencia a preocupação com os *includes* de diferentes plataformas via macros de pré-processador (`#ifdef __APPLE__`, etc.). A arquitetura do novo projeto abraçará essas diretrizes para manter conformidade com a Regra de Multiplataforma estabelecida no `AGENTS.md`.

## 2. Fundamentos e Algoritmos Exigidos
O conteúdo ministrado nas aulas abrange todo o fluxo tradicional da Computação Gráfica. Para atendermos às expectativas do professor, garantiremos que os seguintes conceitos fiquem transparentes na lógica do jogo:

- **Transformações Geométricas 2D e 3D:** Implementação e uso de matrizes matemáticas de translação, rotação e escala. O uso de **hierarquia de transformações** será empregado (ex: manipulação de `push`/`pop` em matrizes) para os membros de bosses ou entidades multicomponentes.
- **Projeções Geométricas:** Uso híbrido e consistente de projeções. A projeção ortográfica servirá ao HUD e aos elementos 2D do gameplay, enquanto a projeção perspectiva comporá os cenários orgânicos 3D de fundo.
- **Rasterização e Preenchimento:** Algoritmos clássicos ensinados em sala (ex: Bresenham para retas e circunferências, algoritmos incrementais e FloodFill) influenciarão certas utilidades do motor ou abordagens de preenchimento quando aplicável fora do escopo automático do OpenGL.
- **Recorte (Clipping) e Visibilidade:** Teorias como as de Cohen-Sutherland ou Liang-Barsky guiarão as abstrações do nosso motor de física simples (Broadphase/Narrowphase e *Culling*) para detectar quando descartar ou processar interações entre projéteis e inimigos.
- **Iluminação e Sombreamento:** Os fundamentos de reflexão e uso do Gouraud shading darão vida à modelagem biológica do vírus e das defesas celulares do paciente, junto com texturas em formato PNG.

## 3. Arquitetura e Abordagem
Como temos flexibilidade para ir além desde que a base exigida seja mantida, adotaremos os seguintes princípios de software:

- **Orientação a Objetos em C++:** Todo conceito matemático e estrutural do OpenGL será encapsulado (ex: classe `Renderer` ou classe `Transform`). O jogo não terá um código processual misturado com a renderização em um arquivo gigantesco, mas sim classes modularizadas.
- **Abstração Limpa:** As chamadas do pipeline fixo do OpenGL exigidas pelas aulas ficarão contidas em funções renderizadoras das classes, deixando a lógica de jogo invisível a esses detalhes de baixo nível.
- **Cross-Platform Build System:** Adotaremos ferramentas flexíveis de *build* (como CMake ou Makefiles granulares) focadas na compatibilidade universal entre os sistemas operacionais estipulados no projeto.
