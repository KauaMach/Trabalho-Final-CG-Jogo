# 📚 Análise dos Materiais da Disciplina (P1, P2, P3) — Técnicas Reutilizáveis

> Mapeamento de todas as técnicas de Computação Gráfica presentes nos trabalhos anteriores da disciplina (P1-CG, P2-CG, P3-CG) e como podem ser integradas no Trabalho Final.

---

## 📁 Inventário Completo de Arquivos

### P1-CG
| Arquivo | Tamanho | Conteúdo |
|---|---|---|
| `codigoexampeloopengl.txt` | 2.4 KB | Exemplo básico OpenGL (triângulos) |
| `Tarefa 06/paisagem.cpp` | 9.3 KB | Cena de paisagem 2D completa |
| `Tarefa 07/braco.cpp` | 7.1 KB | Braço robótico v1 (com iluminação) |
| `Tarefa 07/braco_novo.cpp` | 8.0 KB | Braço robótico v2 (com detecção de colisão) |

### P2-CG
| Arquivo | Tamanho | Conteúdo |
|---|---|---|
| `Exemplos/CG.a06/janela.cpp` | 5.2 KB | Exemplo OpenGL do professor |
| `Exemplos/CG.a06/circulo.h` | 1.2 KB | Biblioteca reutilizável de círculos |
| `Exemplos/CG.a07/braco.cpp` | 5.1 KB | Braço robótico do professor |
| `Exemplos/CG.a13/paint.cpp` | 9.2 KB | Rasterização (algoritmo imediato) |
| `Exemplos/CG.a13/glut_text.h` | 2.0 KB | Biblioteca de renderização de texto |
| `Tarefa 12/ATV12/paint.cpp` | 10.6 KB | **Algoritmo de Bresenham** |
| `Tarefa 12/Controle/paint.cpp` | 5.5 KB | Bresenham simplificado |
| `Trabalho-02-Paint/paint.cpp` | 30.0 KB | **PAINT COMPLETO** (mais abrangente) |
| `Trabalho-02-Paint/glut_text.h` | 2.0 KB | Biblioteca de texto |

### P3-CG
| Arquivo | Tamanho | Conteúdo |
|---|---|---|
| `Tarefa Aula 22/cubo3D.cpp` | 10.0 KB | **Cubo 3D com mesh, normais, shading, animação** |
| `Tarefa Aula 22/Arq/cubo_malha.cpp` | 9.6 KB | Cubo com malha triangular |
| `Tarefa Aula 22/glut_text.h` | 2.0 KB | Biblioteca de texto |
| `Tarefa Aula 23/.../modelo3D.cpp` | 32.5 KB | **RENDERER 3D COMPLETO** (mais avançado) |
| `Tarefa Aula 23/.../image_view.cpp` | 3.7 KB | Visualizador de imagens com glDrawPixels |
| `Tarefa Aula 23/.../glm.h` | 12.8 KB | **Loader Wavefront OBJ** (Nate Robins) |
| `Tarefa Aula 23/.../glm.cpp` | 74.8 KB | Implementação do loader OBJ |
| `Tarefa Aula 23/.../textureRGB.h` | 63.1 KB | Dados de textura 64×64 hardcoded |
| `Tarefa Aula 24/gerar_personagem_animado.py` | — | Script Blender para personagem 3D |

---

## 🎯 Técnicas por Tópico de CG

### 1. Projeção Ortográfica 2D
**Arquivos:** `paisagem.cpp`, `janela.cpp`, paint files, `image_view.cpp`
- `glOrtho(0, w, 0, h, -1, 1)`
- `glViewport(0, 0, w, h)`
- Switching entre `GL_PROJECTION` / `GL_MODELVIEW`

**Uso no jogo:** HUD, menus, overlays 2D.

### 2. Projeção Perspectiva 3D
**Arquivos:** `braco.cpp`, `braco_novo.cpp`, `cubo3D.cpp`, `modelo3D.cpp`
- `gluPerspective(fov, aspect, near, far)`
- `gluLookAt(eye, center, up)`
- Toggle Perspectiva/Ortográfica em `modelo3D.cpp`

**Uso no jogo:** ✅ Já utilizado — câmera 3D.

### 3. Transformações Geométricas (Matrix Stack OpenGL)
**Arquivos:** `paisagem.cpp`, `braco.cpp`, `braco_novo.cpp`, `cubo3D.cpp`, `modelo3D.cpp`
- `glPushMatrix()` / `glPopMatrix()`
- `glTranslatef`, `glRotatef`, `glScalef`
- **Transformações hierárquicas:** braço robótico com articulações aninhadas (ombro→braço→cotovelo→antebraço→punho→mão→dedos)

**Uso no jogo:** ✅ Parcialmente utilizado. Pode ser expandido para articulações dos bosses.

### 4. Transformações Geométricas Manuais (Matemática Pura)
**Arquivo:** `Trabalho-02-Paint/paint.cpp` (linhas 834–950)
- **Translação:** `v.x += dx; v.y += dy`
- **Rotação** (em torno do centroide): `v.x = x*cos(rad) - y*sin(rad) + cx`
- **Escala** (em torno do centroide): `v.x = (v.x - cx) * sx + cx`
- **Reflexão:** `v.x = cx - (v.x - cx)`
- **Cisalhamento:** `v.x = ox + kx*oy + cx`

**Uso no jogo:** ⭐ Power-ups com efeito de escala, rotação de projéteis, cisalhamento para efeito de "distorção sináptica" na Fase 4.

### 5. Algoritmos de Rasterização
**Arquivos:** `ATV12/paint.cpp`, `Controle/paint.cpp`, `Trabalho-02-Paint/paint.cpp`

| Algoritmo | Arquivo | Uso potencial no jogo |
|---|---|---|
| **Reta Imediata** | `Exemplos/CG.a13/paint.cpp` | Demonstração acadêmica |
| **Bresenham (Reta)** | `ATV12/paint.cpp`, `Trabalho-02/paint.cpp` | Raios laser do NANOCELL-1 |
| **Bresenham (Circunferência)** | `Trabalho-02/paint.cpp` | Raio de alcance, explosões |
| **Flood Fill** (iterativo com pilha) | `Trabalho-02/paint.cpp` | Efeito de "infecção" se espalhando |
| **Scanline Fill** | `Trabalho-02/paint.cpp` | Preenchimento de polígonos no HUD |

### 6. Iluminação (Modelo de Phong)
**Arquivos:** `braco.cpp`, `braco_novo.cpp`, `cubo3D.cpp`, `modelo3D.cpp`
- `glLightfv(GL_LIGHT0, GL_POSITION/GL_DIFFUSE/GL_SPECULAR/GL_AMBIENT, ...)`
- `glMaterialfv(GL_FRONT, GL_SPECULAR/GL_AMBIENT/GL_DIFFUSE/GL_EMISSION, ...)`
- `glMateriali(GL_FRONT, GL_SHININESS, value)`
- `glEnable(GL_COLOR_MATERIAL)` + `glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE)`
- **Melhor exemplo:** `modelo3D.cpp` — modelo Phong completo com Ka, Kd, Ks, Ke para material "brass"

**Uso no jogo:** ✅ Já utilizado. Pode ser expandido com material emissivo para projéteis.

### 7. Modelos de Sombreamento
**Arquivos:** `cubo3D.cpp`, `cubo_malha.cpp`, `modelo3D.cpp`
- **Gouraud (Smooth):** `glShadeModel(GL_SMOOTH)` — normais por vértice interpoladas
- **Flat:** `glShadeModel(GL_FLAT)` — normais por face, sem interpolação
- Toggle em runtime entre os dois modos
- **Cálculo de normais:**
  - Normais de face via produto vetorial: `n = a × b`
  - Normais de vértice como média das normais das faces adjacentes
  - Função `normalizar()` para normalização de vetores

**Uso no jogo:** ⭐ CRÍTICO — O `cubo3D.cpp` tem exatamente o código para calcular normais que falta no `conveter.py`!

### 8. Texturização
**Arquivos:** `modelo3D.cpp`, `image_view.cpp`, `textureRGB.h`, `glm.h/glm.cpp`
- `glEnable(GL_TEXTURE_2D)`
- Modos de aplicação: `GL_DECAL`, `GL_MODULATE`, `GL_BLEND`, `GL_ADD`
- `glDrawPixels` para renderização direta de pixels
- `glPixelZoom` para zoom de pixel
- Carregamento de textura PPM via `glm.cpp`

**Uso no jogo:** ✅ Já utilizado com stb_image. Modos de textura podem ser expandidos.

### 9. Carregamento de Modelos 3D (Wavefront OBJ)
**Arquivos:** `glm.h`, `glm.cpp`, `modelo3D.cpp`
- Parser OBJ completo: vértices, normais, coordenadas de textura, materiais, grupos
- Carregamento de biblioteca de materiais (.mtl) com Ka, Kd, Ks, Ke, shininess
- Carregamento de textura PPM
- Funções: `glmReadOBJ()`, `glmDraw()`, `glmFacetNormals()`, `glmVertexNormals()`
- Modos de renderização: `GLM_FLAT`, `GLM_SMOOTH`, `GLM_TEXTURE`, `GLM_COLOR`, `GLM_MATERIAL`

**Uso no jogo:** ⭐ ALTAMENTE REUTILIZÁVEL — pode substituir o `conveter.py` e carregar OBJs em runtime com normais corretas!

### 10. Sistema de Animação
**Arquivos:** `modelo3D.cpp`, `cubo3D.cpp`, `cubo_malha.cpp`
- Animação por timer: `glutTimerFunc(1000/fps, timer, 0)`
- Cálculo de FPS: `computeFPS()` via `glutGet(GLUT_ELAPSED_TIME)`
- Animação por keyframes: múltiplos modelos OBJ carregados e ciclados
- Máquina de estados: `idle → bored → running → jumping → shooting`
- Blending de animação via `keyframe_rate`
- Rotação 3D: `angulo += graus; glRotatef(angulo, 1, 1, 0)`

**Uso no jogo:** ⭐ Rotação dos vírus, animação do NANOCELL-1, FPS counter.

### 11. Sistema de Câmera
**Arquivos:** `modelo3D.cpp`, `braco.cpp`, `braco_novo.cpp`
- Câmera orbital: rotação em torno do objeto via trigonometria (`cos`, `sin`)
- Rotação em eixos X e Y
- Detecção de flip do vetor "up"

**Uso no jogo:** ✅ Já utilizado parcialmente.

### 12. Back-Face Culling
**Arquivo:** `modelo3D.cpp`
- `glEnable(GL_CULL_FACE)` / `glCullFace(GL_BACK)`

**Uso no jogo:** ⭐ Otimização de renderização — reduz triângulos processados pela metade!

### 13. Anti-Aliasing
**Arquivo:** `paisagem.cpp`
- `glEnable(GL_BLEND)` + `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`
- `glEnable(GL_LINE_SMOOTH)` / `glEnable(GL_POLYGON_SMOOTH)`
- `glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)`

**Uso no jogo:** ⭐ Suavização visual de bordas e partículas.

### 14. Primitivas GLUT 3D
**Arquivos:** `braco.cpp`, `braco_novo.cpp`, `modelo3D.cpp`
- `glutSolidSphere`, `glutSolidCube`, `glutSolidCone`
- `gluCylinder` (objetos quadráticos)
- Variantes wireframe: `glutWireCube`, `glutWireSphere`, `glutWireCone`

**Uso no jogo:** ⭐ Power-ups como esferas/cubos, projéteis como cilindros.

### 15. Sistema de Projéteis
**Arquivo:** `modelo3D.cpp` (linhas 340–397)
- Spawn na posição do personagem
- Direção calculada por rotação: `sinf(rad)`, `cosf(rad)`
- Atualização por frame: `pos += dir * speed`
- Desativação por distância: `dist² > 400`
- Material emissivo para efeito "brilhante"
- `glPushAttrib(GL_LIGHTING_BIT)` / `glPopAttrib()` para isolamento de material

**Uso no jogo:** ⭐ ALTAMENTE REUTILIZÁVEL — sistema de projéteis do jogador E dos inimigos!

### 16. Movimentação de Personagem
**Arquivo:** `modelo3D.cpp` (linhas 671–928)
- Direções: `frente, tras, esquerda, direita`
- Padrão turn-then-move: personagem rotaciona antes de mover
- Controles por teclas direcionais
- Estado de animação vinculado ao movimento

**Uso no jogo:** ⭐ Padrão de referência para movimentação do NANOCELL-1.

### 17. Detecção de Colisão
**Arquivo:** `braco_novo.cpp` (linhas 94–111)
- `posicaoSegura(ombro, cotovelo)` — cinemática direta
- Cálculo de posição Y via `cos()` dos ângulos articulares
- Prevenção de penetração do chão

**Uso no jogo:** ✅ Conceito já aplicado no sistema de colisão esfera-esfera.

### 18. Interface UI Customizada
**Arquivo:** `Trabalho-02-Paint/paint.cpp` (linhas 952–1128)
- Toolbar com botões clicáveis
- Sidebar com histórico de formas
- Paleta de cores com seleção visual
- Hit-testing: `handleInterfaceClick()` verifica bounds dos botões
- `glutBitmapWidth()` para medir largura de texto
- Feedback visual do estado ativo

**Uso no jogo:** ⭐ Padrão para menu de upgrades, seleção de polaridade.

### 19. Renderização de Texto
**Arquivo:** `glut_text.h` (presente em múltiplos diretórios)
- Texto bitmap: `glutBitmapCharacter(font, char)` + `glRasterPos2f(x, y)`
- Texto stroke: `glutStrokeCharacter(font, char)` com `glScalef`
- Funções reutilizáveis: `draw_text_bitmap()` e `draw_text_stroke()`

**Uso no jogo:** ✅ Já utilizado para HUD. `glut_text.h` pode substituir a implementação manual.

### 20. Leitura de Framebuffer
**Arquivo:** `Trabalho-02-Paint/paint.cpp`
- `glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer)`
- `glPixelStorei(GL_PACK_ALIGNMENT, 1)`
- `glReadBuffer(GL_BACK)`

**Uso no jogo:** Screenshots, pixel picking para debug.

---

## 🏆 Top 5 Arquivos Mais Reutilizáveis

| # | Arquivo | Por que é importante |
|---|---|---|
| 1 | **`P2-CG/Trabalho-02-Paint/paint.cpp`** | Bresenham (reta + círculo), Flood Fill, Scanline Fill, TODAS as transformações 2D, UI system, gestão de formas, paleta de cores |
| 2 | **`P3-CG/Tarefa Aula 23/modelo3D.cpp`** | Protótipo 3D completo: OBJ loader, animação, câmera, iluminação, texturização, projéteis, movimentação, FPS counter |
| 3 | **`P3-CG/Tarefa Aula 22/cubo3D.cpp`** | Mesh com normais calculadas, toggle Gouraud/Flat, iluminação, animação |
| 4 | **`P1-CG/Tarefa 07/braco_novo.cpp`** | Transformações hierárquicas 3D, cinemática direta, detecção de colisão |
| 5 | **`P3-CG/Tarefa Aula 23/glm.h + glm.cpp`** | Loader Wavefront OBJ completo com materiais e texturas |

---

## 📋 Mapeamento: Técnica → Uso no Jogo

| Técnica da Disciplina | Arquivo Fonte | Aplicação no Jogo | Status Atual |
|---|---|---|---|
| Projeção Ortográfica | `janela.cpp` | HUD, menus | ✅ Implementado |
| Projeção Perspectiva | `modelo3D.cpp` | Câmera 3D | ✅ Implementado |
| Transformações (Push/Pop) | `braco.cpp` | Posição de entidades | ✅ Implementado |
| Transformações manuais | `paint.cpp` | Efeitos visuais, distorção F4 | ❌ Não usado |
| Bresenham (reta) | `ATV12/paint.cpp` | Raios laser | ❌ Não usado |
| Bresenham (circunferência) | `paint.cpp` | Raio de alcance | ❌ Não usado |
| Flood Fill | `paint.cpp` | Efeito de infecção | ❌ Não usado |
| Scanline Fill | `paint.cpp` | Polígonos preenchidos | ❌ Não usado |
| Iluminação Phong | `modelo3D.cpp` | Iluminação dinâmica | ✅ Implementado |
| Gouraud Shading | `cubo3D.cpp` | Shading de modelos | ⚠️ Parcial (faltam normais) |
| Flat Shading | `cubo3D.cpp` | Toggle visual | ❌ Não usado |
| Cálculo de normais | `cubo3D.cpp` | Fix do conveter.py | ❌ NECESSÁRIO |
| Texturas 2D | `modelo3D.cpp` | Fundos, menus | ✅ Implementado |
| Loader OBJ (glm) | `glm.h/glm.cpp` | Carregar modelos runtime | ❌ Não usado |
| Animação por timer | `modelo3D.cpp` | Game loop 60fps | ✅ Implementado |
| FPS counter | `cubo3D.cpp` | Debug/HUD | ❌ Não usado |
| Keyframe animation | `modelo3D.cpp` | Animação de entidades | ❌ Não usado |
| Back-face culling | `modelo3D.cpp` | Otimização | ❌ Não usado |
| Anti-aliasing | `paisagem.cpp` | Suavização visual | ❌ Não usado |
| Primitivas GLUT 3D | `braco.cpp` | Power-ups, projéteis | ❌ Não usado |
| Sistema de projéteis | `modelo3D.cpp` | Tiros jogador/inimigos | ❌ Não usado |
| Movimentação | `modelo3D.cpp` | Controle do NANOCELL-1 | ✅ Parcial |
| UI/Botões | `paint.cpp` | Menu de upgrades | ❌ Não usado |
| Texto (glut_text.h) | `glut_text.h` | HUD mais completo | ❌ Não usado |
| Colisão cinemática | `braco_novo.cpp` | Contenção de fronteiras | ✅ Parcial |
| Material emissivo | `modelo3D.cpp` | Projéteis brilhantes | ❌ Não usado |
| Depth Test | `cubo3D.cpp` | Z-buffer | ✅ Implementado |
| `glPushAttrib/PopAttrib` | `modelo3D.cpp` | Isolamento de estado GL | ❌ Não usado |
