# Imunidade — Documento de Design de Jogo (GDD v2.0)
## Documento 5: Direção de Arte e Som

---

### 1. Direção de Arte e Estética Visual

A direção de arte baseia-se em alto contraste para favorecer a clareza mecânica (jogadores precisam discernir imediatamente a polaridade das coisas). O visual será um "Neon Orgânico", unindo biologia com luzes elétricas extremas.

#### Iluminação (Gouraud e Phong)
* A maior parte dos shaders aplicará `GL_SMOOTH` em vez de `GL_FLAT`, já que estamos dentro do corpo e queremos evitar geometrias "duras" ou blocadas.
* A fonte de luz (Luz Difusa) será totalmente ditada pela Polaridade do jogador, o que muda a atmosfera de toda a fase dinamicamente.
* Se o escudo for azul: Toda a cena ganha um matiz frio (`Luz Difusa = {0.3, 0.4, 1.0}`). Inimigos vermelhos saltam muito os olhos na escuridão.
* Se o escudo for vermelho: A cena ferve num tom alaranjado (`Luz Difusa = {1.0, 0.3, 0.2}`).

#### Paletas de Cores Hexadecimais
* **NANOCELL-1 (Protagonista):** Metálico Base (`#888888`), Brilho Especular extremo (`Shininess: 128`).
* **Polaridade Fria (Cura/Segurança):** Cyan Neon (`#00FFFF`) e Azul Elétrico (`#0000FF`).
* **Polaridade Quente (Perigo/Absorção):** Vermelho Sangue (`#FF0000`) e Laranja Alerta (`#FF4500`).
* **Inimigos Normais:** Verde Ácido (`#39FF14`) — remete a infecções e toxicidade.
* **Cenário de Fundo (Backdrop):** Preto (`#050505`) para evidenciar o efeito neon das emissões dos personagens.

#### Efeitos Visuais (VFX)
As **Partículas** usam o sistema recuperado (`glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`). Explosões, rastros de projéteis e fumaça ambiental não possuem texturas — eles são desenhados usando `GL_POINTS` ou círculos radiais paramétricos que sofrem *fade-out* linear no canal Alpha.

---

### 2. Design de Som e Música

O áudio via SDL2_mixer suporta o impacto da jogabilidade. A premissa é "Pulsante e Eletrônico". 

#### Trilha Sonora
* **Música das Fases 1 a 4:** *Synthwave Darksynth*, compasso acelerado (120-140 BPM). O ritmo acompanha a cadência do pulso sanguíneo.
* **Música do Boss (Fase 5):** Eletrônica industrial pesada com batidas graves distorcidas para enfatizar o tamanho do Inimigo Ômega.
* **Telas de Menu/Upgrade:** Ambientação minimalista profunda. Som de fluidos correndo.

#### Lista de Efeitos Sonoros (SFX)
* `laser_shoot.wav`: Sintetizado. Som agudo rápido estilo blaster.
* `absorb_success.wav`: Som de água batendo suave, sino digital de "recompensa".
* `damage_taken.wav`: Estática digital misturada com vidro quebrando (feedback forte).
* `surge_activation.wav`: Explosão de baixo pesado longo (*Bass Drop*).
* `boss_scream.wav`: Som animal distorcido digitalmente (para a entrada e morte do Boss).

---

### 3. Pipeline de Assets 3D

Para garantir modelos 3D eficientes e de boa aparência (já que o pipeline fixo não usa shaders modernos), este será o fluxo de trabalho de qualquer arte:

1. **Modelagem:** Criar modelos no **Blender**. Foco em baixo número de polígonos (Low Poly) já que as subdivisões serão simuladas pelo Gouraud Shading em runtime.
2. **Exportação:** Exportar no formato `.obj` puro (apenas vértices e normais, sem modificadores que deformem fora do centro).
3. **Conversão de Textura:** Gerar materiais com cores chapadas. As luzes farão o trabalho de deixá-los realistas.
4. **Integração no Jogo:** Utilizaremos o loader do *Nate Robins* (o módulo `glm.cpp` das aulas de P3). Este loader carrega a malha, aplica *Vertex Normals* calculadas internamente e lê arquivos `.mtl` sem precisar da conversão antiga que quebrava as normais.
5. **Geração Procedural (Backup):** Inimigos secundários podem ser montados hardcoded utilizando as primitivas GLUT `glutSolidSphere`, `glutSolidCone` se houver limitação de arquivo. O Boss final SERÁ procedural (baseado no código do Braço Robótico).
