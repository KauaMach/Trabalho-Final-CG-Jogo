# Imunidade — Documento de Design de Jogo (GDD v2.0)
## Documento 3: Fases, Bestiário e Balanceamento

---

### 1. Fases e Progressão

O jogo é dividido em 5 biomas anatômicos. A transição de cena muda a paleta de luz ambiente global (Phong) e os padrões de terreno rasterizados no fundo.

* **Fase 1: Corrente Sanguínea (Infiltração)**
  * **Estética:** Fundo vermelho pulsante, com glóbulos rubros passando em alta velocidade (simulando parallax em 3D). Luz ambiente avermelhada.
  * **Foco Mecânico:** Introduzir a troca de polaridade (inimigos disparam projéteis lentos vermelhos e azuis).

* **Fase 2: Barreira Pulmonar (Resistência)**
  * **Estética:** Estruturas alveolares, fumaça verde tóxica ao fundo (transparências com `GL_BLEND`).
  * **Foco Mecânico:** Inimigos estacionários que agem como metralhadoras em spray (`Boids` com comportamento de torre). O jogador precisa praticar o *Dash*.

* **Fase 3: Nódulo Linfático (Guerra de Trincheiras)**
  * **Estética:** Paredes esbranquiçadas e ácidas. Labirintos de artérias endurecidas. Colisões de paredes com verificação AABB densa.
  * **Foco Mecânico:** O espaço é apertado, favorecendo ataques corpo-a-corpo de inimigos (comportamento de perseguição/Seek).

* **Fase 4: Sistema Nervoso (Distorção)**
  * **Estética:** Fios elétricos azuis brilhantes cortando o espaço preto. Descargas estáticas e efeitos glitch na tela (cisalhamento na projeção da tela).
  * **Foco Mecânico:** Os impulsos elétricos ocasionalmente alteram a polaridade do jogador forçadamente.

* **Fase 5: Núcleo Viral (Córtex Cerebral)**
  * **Estética:** O centro da colmeia. Luzes estroboscópicas severas.
  * **Foco Mecânico:** Exclusiva para a batalha contra o Boss Final: A Entidade Ômega.

---

### 2. Bestiário — Catálogo de Inimigos

Os inimigos agora são modelos 3D carregados dinamicamente (`glm.c` do P3) com texturas e materiais complexos.

| Inimigo | Classe | Comportamento IA (Boids) | Método de Ataque | Cor Base |
| :--- | :--- | :--- | :--- | :--- |
| **Drone Viral** | Minion | *Flocking* básico, movem-se em grupos | Corpo-a-corpo (Kamikaze) | Verde (Muda para Azul/Verm antes de atacar) |
| **Bactéria Artilheira** | Pesado | *Wander*, movimenta-se devagar | Dispara padrão de estrelas (8 direções) | Fixo (Azul ou Vermelho) |
| **Macrófago Corrompido**| Tanque | *Pursuit*, segue o jogador de perto | Emite aura de dano contínuo em volta de si | Roxo |
| **Príon Mimético** | Especial | *Shadowplay*, espelha os movimentos do jogador | Tiro teleguiado lento | Alterna a cada 2s |

#### 2.1 Boss Final: A Entidade Ômega
O Boss Final será construído usando as **Transformações Hierárquicas** da Tarefa do *Braço Robótico* (P1). 
* Ele não é apenas um modelo fixo; ele é composto por um núcleo flutuante com **4 tentáculos independentes**.
* Cada tentáculo é formado por Ombro→Cotovelo→Punho (vários cilindros interconectados com `glRotatef`).
* O jogador não atinge a hitbox do corpo inteiro. Ele precisa atirar nos pontos das "juntas" dos tentáculos para desmembrá-lo antes de atacar o núcleo.

---

### 3. Inteligência Artificial (IA)

A arquitetura da IA baseia-se em dois módulos fundamentais:

* **Swarms (Boids Engine):** Para os minions menores (Drones). A lógica calcula vetores de Alinhamento, Coesão e Separação a cada frame. Isso impede que os inimigos se sobreponham visualmente e gera formações orgânicas de ataque que se dividem se o jogador passar pelo meio deles.
* **Máquina de Estados Finita (FSM):** Para o Boss e Inimigos Pesados.
  * Estado `IDLE`: Flutua lentamente em um padrão de círculo (paramétrico).
  * Estado `ENGAGE`: Trava na coordenada atual do jogador e prepara padrão de tiro.
  * Estado `RETREAT`: Se HP < 20%, recua para os cantos da tela em velocidade dupla.
  * Estado `BERZERK`: O Boss aumenta as velocidades dos tentáculos usando `sinf(tempo)` mais rápido.

---

### 4. Balanceamento e Tabelas Numéricas

#### Atributos do NANOCELL-1 (Jogador)
* **Vida (HSP):** 100 pontos (Max)
* **Dano Recebido:** 15 pontos (Por tiro da cor errada) / 30 pontos (Por colisão corporal)
* **Velocidade Base:** 8.0 unidades por segundo
* **Velocidade do Laser:** Instantâneo (Raycast Bresenham)
* **Recarga de Polaridade:** Cooldown de 0.2s entre trocas (evita exploits de macro de teclado).

#### Curva de Dificuldade
* **Fase 1:** Max 15 inimigos na tela. Velocidade de tiro de 0.8 projéteis/segundo. Dano reduzido para o jogador (10).
* **Fase 3:** Max 40 inimigos na tela. O Algoritmo de Hashing Espacial de Colisão é testado no limite. 1.5 projéteis/segundo.
* **Fase 5:** Apenas o Boss, disparando dezenas de pequenas partículas usando o pool do sistema implementado.
