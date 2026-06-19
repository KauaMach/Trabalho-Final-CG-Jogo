# Imunidade — Documento de Design de Jogo (GDD v2.0)
## Documento 6: Arquitetura e Engenharia de Software

---

### 1. Arquitetura Técnica

O projeto sofre uma reestruturação para sair do modelo "tudo em um arquivo `main.cpp`" para um paradigma **Orientado a Componentes (Modular)**. 
* **`main.cpp`:** Fica responsável APENAS pelo loop de eventos do GLUT e instanciar o Game Manager.
* **`/core`:** Módulo de gerenciamento de estado da cena, configurações gerais.
* **`/entities`:** Classes puras de `Player`, `Enemy`, `Projectile` e `Boss`.
* **`/physics`:** O motor de colisão refatorado (Spatial Hash).
* **`/rendering`:** Isolamento completo de todos os comandos `glBegin`, carregadores `glm` e UI.
* **`/audio`:** Encapsulamento SDL2 (já existente).

---

### 2. Recursos OpenGL Utilizados (Mapeamento UFPI)

Para validar todos os critérios da disciplina, as seguintes técnicas ensinadas foram resgatadas e aplicadas ativamente no motor do jogo:

| Técnica de Aula (P1/P2/P3) | Como foi Aplicada no Jogo | Arquivo de Referência Original |
| :--- | :--- | :--- |
| **Projeções e Câmera** | `gluPerspective` combinada com `gluLookAt` num ângulo isométrico de cima. | `modelo3D.cpp` (P3) |
| **Bresenham (Retas)** | Os tiros lasers do jogador são plotados diretamente no framebuffer ignorando malhas 3D para serem instantâneos. | `ATV12/paint.cpp` (P2) |
| **Transformações 3D Hierárquicas** | Construção procedural dos tentáculos do Boss Final (Push/Pop sucessivos). | `braco_novo.cpp` (P1) |
| **Flood Fill Iterativo** | Mecânica especial de "infecção" do SURGE: Ao destruir um inimigo, o Flood Fill acha todos os inimigos próximos na mesma cor na tela e propaga a destruição. | `paint.cpp` (P2) |
| **Gouraud Shading** | Iluminação padrão dos modelos orgânicos, normais suavizadas lidas pelo Loader OBJ. | `cubo3D.cpp` (P3) |
| **Culling e Depth Testing** | `GL_CULL_FACE` e Z-Buffer ativados para garantir performance e sobreposição correta. | Múltiplos |
| **Carregador de Malhas Wavefront** | Substituição do script Python quebrado pelo loader robusto `.obj` com materiais completos. | `glm.h/glm.cpp` (P3) |
| **Cisalhamento (Shear)** | Efeito de distorção visual na Fase 4 (Sistema Nervoso) em cima de todo o framebuffer. | `paint.cpp` (P2) |

---

### 3. Detecção de Colisão

* **Spatial Hash Grid:** O código resgatado do atual `collision.cpp` será ativado. Como o jogo terá até 40-50 inimigos na tela ao mesmo tempo (Bullet Hell) + mais dezenas de projéteis, checar todo mundo contra todo mundo (Força Bruta O(N²)) vai engasgar a engine em 60fps.
* **Física de Bounding Box / Esfera:** 
  * O Jogador possui uma esfera central exata (`Sphere-to-Sphere`).
  * O Boss Final possui AABBs rotacionadas (Bounding Boxes) nos tentáculos. Calculamos interseção usando cinemática linear simples adaptada da tarefa de colisão do *Braço Robótico* (onde prevíamos penetração do solo baseada nos ângulos).

---

### 4. Dependências e Compilação

O projeto não deve sofrer com "Dependency Hell" e deve compilar de primeira nos laboratórios da UFPI ou na máquina do professor usando apenas um `make`.

* **Bibliotecas Base:**
  * Base Gráfica: `freeglut` (Windows/Linux) ou `GLUT.framework` (macOS).
  * API 3D: `OpenGL` core legada (pipeline fixo glBegin/glEnd).
  * Áudio: `SDL2` + `SDL2_mixer`.
* **Bibliotecas Headers-Only:**
  * Imagens: `stb_image.h` (apenas para background de menus).
  * OBJ: `glm.h` / `glm.cpp` adaptado (sem dependências externas malucas).

O `Makefile` possui as flags universais `-O2 -Wall` e detecta magicamente a plataforma (Darwin, Windows_NT ou Linux default).
