# Tópicos para Apresentação - Jogo "Imunidade: A Guerra Celular"

Este documento é o roteiro definitivo para a sua apresentação. Ele cruza toda a matemática e arquitetura do código com as definições de Game Design (História, Mecânicas, Inimigos e IA), garantindo que você tenha a resposta exata para qualquer pergunta, seja teórica ou técnica, feita pelo seu professor.

---

## 1. Contexto, História e Pitch do Projeto
**Como apresentar a ideia:**
"Imunidade: A Guerra Celular" é um *Shoot 'em Up* (Shmup) com visão isométrica 3D renderizado em OpenGL.
**A História:** O "Paciente Zero" foi infectado pelo vírus sintético **Nexus-7**, criado por uma corporação biohacker. O sistema imunológico entrou em colapso. O jogador controla o **NANOCELL-1**, um nanobô médico injetado via *Protocolo Ícaro* para travar uma guerra microscópica em três frentes: Corrente Sanguínea (F1), Pulmões (F2) e Sistema Nervoso Central (F3).

---

## 2. A Matemática das Mecânicas Principais (Com Código)

### A. Polaridade Cromática e Absorção Celular
É o grande diferencial tático (baseado em Ikaruga). O NANOCELL-1 alterna sua polaridade. 
- **Defesa (Absorção):** Tiros da mesma cor curam a nave e enchem a barra de especial.
- **Risco vs Recompensa:** Tiros da cor oposta causam dano extremo. O dano sofrido é matematicamente maior que a cura, forçando o jogador a ter destreza para trocar de cor em vez de apenas "tankar" dano.
```cpp
// main.cpp - Função de captura do teclado bloqueada pela Máquina de Estados
if (currentState == STATE_PLAYING) {
    if (key == 'c') player.TogglePolarity(); // Altera a cor instantaneamente
}
```

### B. Medidor de Saúde do Paciente (HSP)
Em vez de um número de "vidas" clássicas, o jogo acompanha o HSP. Se chegar a 0%, Game Over.
- Destruir vírus: Adiciona de +2% a +5% de vida ao paciente.
- Deixar vírus passar pela tela: Deduz -3% de vida (Punição severa pela inatividade).
- Chegar em 90%: Evoca o Boss da Fase automaticamente.

### C. Especial SURGE (Blizzard vs Inferno)
Ao atingir 100% de absorção de tiros da mesma cor, o especial SURGE é liberado (tecla `Q`), com formas distintas:
- **SURGE Azul (Blizzard):** Feixe contínuo letal focado que rasga a tela de ponta a ponta.
- **SURGE Vermelho (Inferno):** Explosão radial maciça que limpa projéteis e devora inimigos ao redor.

### D. Movimentação Avançada: Evasão e Screen Wrapping
- **Barrel Roll e Back-Dash:** Tecla `E` (ou `R` para aéreo puro) inicia um salto para trás gerando *i-frames* (quadros de invulnerabilidade) para escapar do perigo.
- **Screen Wrapping:** Para evitar emboscadas nos cantos, sair totalmente da tela pela direita faz a nave reaparecer à esquerda (efeito Pac-Man / Asteroids).

---

## 3. Tela Inicial, Interface e Máquina de Estados
A interface HUD não é desenhada "solta". Ela usa a projeção Ortográfica.

### Máquina de Estados (GameState)
O fluxo da aplicação é blindado. O código não deixa interações se fundirem graças à variável global `currentState`.
```cpp
// MenuUI.cpp - Hitbox do botão e mudança de estado para avançar de fase
if (mouseX >= 277.0f && mouseX <= 657.0f && realMouseY >= 162.0f && realMouseY <= 210.0f) {
    audioManager.PlayClickSound();
    if (currentPhase == 2) {
        currentPhase = 3;
        fase3Desbloqueada = true; // O Estado persistente altera a progressão
        ResetGame();
        state = STATE_PLAYING;    // Transição pura da FSM
    }
}
```

---

## 4. Áudio Assíncrono em Baixo Nível (AudioManager)
Se o professor questionar como os sons não travam a tela, explique o *Hardware Mixing*.
A biblioteca **SDL2_mixer** cria *threads* acústicas paralelas:
- **`Mix_Music`:** Lê áudios `.ogg` gradativamente (streaming) sem lotar a RAM.
- **`Mix_Chunk`:** Joga pequenos `.wav` (como lasers) direto na memória cache para tocar sem atrasos.
```cpp
// AudioManager.cpp - Ajuste dinâmico de volume com conversão matemática
void AudioManager::SetMusicVolume(float volume) {
    // Volume chega de 0.0 a 1.0 pelo Slider. Converte para o padrão 0 a 128 (MIX_MAX_VOLUME)
    Mix_VolumeMusic((int)(volume * MIX_MAX_VOLUME)); 
}
```

---

## 5. Spawners, Tempo e Fases Animadas
Em computação gráfica, as amarrações não se fazem por frames absolutos, mas por **Tempo Delta**.

### A. O Relógio Central do Game Loop
Inimigos surgem na cena baseados na leitura relógio-tempo, e não "andando pelo chão".
```cpp
// main.cpp - O Spawner Temporal
phase1Time += 0.016f; // Avança o tempo baseado em 60 FPS fixos (1/60s = ~0.016)

if (phase1Time > 2.0f && phase1Time < 30.0f) {
    spawnAlphaTimer -= 0.016f;
    if (spawnAlphaTimer <= 0.0f) {
        // Z do jogador + 800 garante que o inimigo nasça fora do escopo e venha na direção da câmera
        listaInimigos.push_back(new VirusAlfa(sorteiaX(), 0.0f, player.GetZ() + 800.0f));
        spawnAlphaTimer = 1.5f;
    }
}
```

### B. Fundos Orgânicos Animados (Spritesheets e Módulo)
A sensação do tecido biológico pulsar se dá por atualizar quadros calculando o módulo (`%`) em relação ao tempo e à quantidade de fotos na pasta.
```cpp
// Converte o tempo do jogo para rodar o fundo a exatos 24 FPS
int frame = (int)(gameTimer * 24.0f) % bgFramesFase1.size(); 
Renderer::DrawTexture(bgFramesFase1[frame], 0, 0, 1024, 768);
```

---

## 6. O Sistema Gráfico: Pilhas de Matrizes (Push/Pop)
Se rotacionarmos a nave inimiga, por que o cenário inteiro não roda junto? A resposta é o gerenciamento de escopo espacial.
```cpp
// main.cpp - Protegendo a matriz espacial via empilhamento (Stack)
for(auto inimigo : listaInimigos) {
    glPushMatrix();      // Guarda as coordenadas mundiais (Câmera)
    inimigo->Desenhar(); // Aplica glRotatef / glTranslatef que afetam APENAS a matriz superior
    glPopMatrix();       // Descarta tudo o que o inimigo fez e devolve a matriz limpa!
}
glLoadIdentity();        // Limpeza absoluta
RenderHUD();             // Renderiza a interface do usuário em 2D
```

---

## 7. Inteligência Artificial e Algoritmos Complexos

### A. O Boid Agressivo (Vírus Alfa - Fase 1)
O Alfa imita enxames de aves (Boids de Craig Reynolds). Além do bando, ele possui um limite de quebra: ao cruzar a barreira vetorial, acelera num *Dash* para cima da nave.
```cpp
// inimigo.cpp - Gatilho de Mergulho Agressivo
void VirusAlfa::Atualizar(float dt, const Player& player) {
    float distZ = player.GetZ() - this->posZ; // Diferença escalar no eixo Z
    if (distZ < 300.0f && !emMergulho) {
        emMergulho = true;
        velZ = 400.0f; // Multiplica a aceleração subitamente para atropelar
    }
}
```

### B. O Inimigo Mais Inteligente (Príon Mimético - Fase 3)
A matemática temporal é usada de forma brilhante aqui. O Príon usa um *Deque* (Fila Circular) para estocar os passos do jogador e repeti-los com exatos 1,5 segundos de atraso (espelho temporal).
```cpp
// inimigo.cpp - IA de Espelhamento Atrasado (Buffer Circular)
void PrionMimetico::Atualizar(float dt, const Player& player) {
    posHistory.push_back({player.GetX(), player.GetZ()}); // Grava o presente no fim da fila
    
    if (posHistory.size() > (size_t)maxBufferSize) { // Se a fila estourar seu limite de delay...
        auto oldPos = posHistory.front(); // Le o início da fila
        posHistory.pop_front(); 
        posX = oldPos.first; // O inimigo adota a exata posição que o player estava no passado!
    }
}
```

### C. A Máquina de Estados Interna dos Bosses
Cada boss opera sobre temporizadores que ativam mutações no padrão de batalha.
- **Leukocyte Corrupto:** Alterna entre *Patrulha* (Andar), *Espiral* (Atira 360º) e *Fúria*.
- **Pneumococo Gigante:** Uma batalha de cadência. Seu escudo maciço só se apaga quando ele "expira" fluidos, exigindo que o jogador atire nos frames certos.
- **Nexus Omega:** Forma tripla (Escudo Dual -> Invocador de Minions -> Núcleo Exposto para fase Bullet-Hell terminal).

---
> **DICA FINAL:** Se o professor perguntar: *"Como vocês renderizaram centenas de inimigos cheios de polígonos usando OpenGL Antigo e CPU no loop de colisão?"*
> **A Resposta Definitiva:** Nós implementamos **Bounding Spheres** para elevar cálculos ao quadrado e fugir das raízes (`sqrt`) que matam o processador, e empacotamos as malhas 3D em **Display Lists (`glGenLists`)** durante o carregamento! As lógicas ficam na nossa CPU (via C++17), mas quem desenha as centenas de triângulos do Boss em uma tacada só é a própria Placa de Vídeo (GPU)!
