# Documentação Técnica e Comportamental dos Inimigos
> **Projeto:** Imunidade: A Guerra Celular

Esta documentação detalha as mecânicas, parâmetros de saúde, inteligência artificial e comportamentos de todos os inimigos presentes na Fase 1 (Corrente Sanguínea) e Fase 2 (Pulmões). O objetivo deste documento é auxiliar o balanceamento e futuras implementações da lógica de combate.

---

## FASE 1: Corrente Sanguínea

### 1. Vírus Alfa
* **Tipo:** Inimigo Padrão (Enxame Kamikaze)
* **Polaridade:** Azul (0)
* **Atributos Principais:**
  * **HP (Vida):** 15.0 (Aproximadamente 2 tiros da mesma cor)
  * **Velocidade Base (velZ):** 80.0
  * **Velocidade de Mergulho:** 250.0
  * **Raio de Colisão:** 18.0
* **Comportamento e Inteligência (IA):**
  * O Vírus Alfa atua sob uma regra básica de Cerco/Mergulho. Ele se aproxima suavemente do jogador no eixo Z. Quando a distância Z entre ele e a Nanocell fica abaixo de `400.0f`, a variável `emMergulho` é ativada.
  * Nesse momento, sua velocidade sobe para 250.0 e ele calcula matematicamente o tempo exato para alcançar o jogador no eixo Z (`tempoParaAlcancar = distZ / velZ`), e usa isso para mirar perfeitamente e corrigir seu deslocamento lateral (velX), caindo como um torpedo diretamente onde o jogador está no eixo X.
* **Ataque:** Não possui projéteis. Seu dano é unicamente por colisão.

### 2. Bactéria Coco
* **Tipo:** Inimigo Tanque (Atirador)
* **Polaridade:** Vermelha (1)
* **Atributos Principais:**
  * **HP (Vida):** 40.0 (Demanda fogo sustentado)
  * **Velocidade Base (velZ):** 60.0 (Descida lenta e linear)
  * **Velocidade (velX):** 0.0 (Não persegue, move-se em linha reta)
  * **Raio de Colisão:** 15.0 (Comprimento em Z alongado para 35.0 - Hitbox Cilíndrica)
* **Comportamento e Inteligência (IA):**
  * Avança de forma passiva através da tela girando em seu próprio eixo (efeito de *barrel roll* constante em 60 graus por segundo). Não possui inteligência de perseguição no eixo X.
* **Ataque:**
  * Utiliza um sistema de *Cooldown* (temporizador de disparos). Quando está a uma distância de até `700.0f` da nave, ela inicia seu ciclo ofensivo.
  * O cooldown inicial é randomizado entre 1 e 2 segundos. Após o primeiro disparo, ela atira sistematicamente a cada `2.0s`.
  * **Tipo de Tiro:** Tiro duplo paralelo (lasers da cor vermelha) deslocados 10 unidades para a esquerda e para a direita do seu centro, lançados na direção da Nanocell.

### 3. Leukocyte Corrupto (BOSS)
* **Tipo:** Boss da Fase 1
* **Polaridade:** Dinâmica (Azul e Vermelha, alternando nas fases)
* **Atributos Principais:**
  * **HP Máximo:** 1500.0
  * **Raio de Colisão:** 35.0 (Hitbox Esférica Ampla)
* **Máquina de Estados Finita (FSM - IA):** O Leukocyte opera por transição de estados baseados em sua porcentagem de vida:
  * **Fase : Patrulha (Acima de 60% HP)**
    * Fica travado na polaridade Vermelha.
    * Mantém-se afastado e faz um movimento senoidal suave no eixo X (`posX = sin(tempo) * 200.0`).
    * **Ataque:** A cada `1.0s`, dispara um projétil único calibrado e teleguiado diretamente para a coordenada atual do jogador (cálculo de vetor normalizado).
  * **Fase 2: Espiral (De 25% a 60% HP)**
    * Aproxima-se mais do jogador no eixo Z.
    * Inicia um movimento errático de varredura (Figura-8) nos eixos X e Y usando cálculos senoidais assimétricos.
    * Fica mudando de polaridade de Azul para Vermelho a cada segundo (bipolaridade `(int)(stateTimer) % 2`).
    * **Ataque:** A cada `0.15s` ele emite tiros num padrão espiral duplo ("metralhadora circular"), forçando a Nanocell a esquivar entre as frestas do giro dos tiros.
  * **Fase 3: Fúria Bullet-Hell (Abaixo de 25% HP)**
    * Aproxima-se intensamente do jogador (`baseZ = 150.0`). Trava na cor Vermelha.
    * Movimento lateral frenético com vibração contínua (`shake`).
    * **Ataque:** A cada `0.4s`, explode em 8 direções (a cada 45 graus). Cada projétil intercala entre Azul e Vermelho, o que obriga a troca agressiva e constante da polaridade da nave para sobreviver (uso intenso do dash e do SURGE).

---

## FASE 2: Pulmões

### 4. Vírus Gama
* **Tipo:** Atirador Bipolar
* **Polaridade:** Alternante (A cada 3 segundos, muda de Azul para Vermelho, e vice-versa)
* **Atributos Principais:**
  * **HP (Vida):** 15.0
  * **Raio de Colisão:** 15.0
* **Comportamento e Inteligência (IA):**
  * Avança de forma constante na vertical (Z). Sua inteligência principal não foca no movimento, mas no engajamento dos tiros e no seu temporizador bipolar (`timerBipolar = 3.0f`).
* **Ataque:**
  * Possui um sistema de disparo intermitente aleatorizado (a cada 1.5s a 2.5s).
  * Calcula a distância até a nave (apenas se a nave estiver num limite visual de `600.0f`).
  * Dispara um tiro teleguiado rápido (`250.0f`) que herda a cor exata em que o vírus está no momento do disparo. Essa mecânica exige que o jogador preste atenção tanto na cor do inimigo (se deseja absorver) quanto no momento em que o projétil for disparado.

### 5. Esporo Fúngico
* **Tipo:** Enxame Kamikaze
* **Polaridade:** Azul (0)
* **Atributos Principais:**
  * **HP (Vida):** 10.0 (Morre com 1 tiro de qualquer cor)
  * **Velocidade Base (velZ):** 380.0 (O inimigo mais rápido do jogo)
  * **Raio de Colisão:** 10.0 (Hitbox minúscula)
  * **Danos Causados:** 20.0f (Integridade Nanocell) e 5.0f (Saúde do Paciente/HSP) por impacto direto.
* **Comportamento e Inteligência (IA):**
  * Em vez de apenas voar reto, a IA do Esporo Fúngico calcula, a cada frame, a distância exata até o jogador no plano X-Y e utiliza um vetor bidimensional normalizado para realizar uma **perseguição agressiva** nos eixos X e Y à espantosa velocidade vetorial de `150.0f` por segundo. Aliado a sua velocidade no eixo Z (`380`), ele realiza um rasante letal, rodopiando sobre o próprio eixo de forma caótica. A janela de tempo para matá-lo antes do contato é exígua.

### 6. Pneumococo Gigante (BOSS 2)
* **Tipo:** Boss Bullet-Hell / Ciclo Respiratório
* **Polaridade:** Dinâmica (Atira projéteis de ambas as cores)
* **Atributos Principais:**
  * **HP Máximo:** 600.0
  * **Hitboxes:** Blindado (Raio 45.0) | Vulnerável (Raio 30.0)
* **Comportamento e Máquina de Estados (3 Fases):**
  * Mantém-se sempre a uma distância segura da câmera (eixo Z travado a `350.0`) e realiza patrulha horizontal (X) e vertical (Y).
  * O combate evolui conforme ele sofre dano:
  * **Fase 1 (600 a 400 HP - Calmo):** Ciclo respiratório dura 4 segundos. Atira 2 lasers retos intercalando as cores Azul e Vermelho a cada 0.8s.
  * **Fase 2 (399 a 200 HP - Agressivo):** Ciclo de respiração mais rápido (3 segundos). Dispara um leque de 3 lasers (Cobrindo Frente, Esquerda e Direita) a cada 0.4s. A cor principal do leque alterna dependendo se ele está inspirando ou não.
  * **Fase 3 (Abaixo de 200 HP - Fúria Bullet-Hell):** Respiração cai para 2 segundos. Ele ativa um canhão giratório contínuo, disparando rajadas em espiral a cada 0.2s. Quando no modo vulnerável (Expirando), a densidade da espiral dobra, transformando o combate numa dança caótica de troca de polaridades da nave!
* **Defesa (Ciclo):**
  * **Inspirando:** Fica Verde Escuro. O raio infla. Neste momento ele fica totalmente imune aos disparos do jogador.
  * **Expirando:** Fica Verde Limão brilhante. Retrai o escudo. Nesta janela ele recebe dano normalmente.

---

## FASE 3: Sistema Nervoso Central

### 7. Príon Mimético
* **Tipo:** Reflexo / Seguidor (Buffer Circular)
* **Polaridade:** Oposta à do Jogador (Dinâmica)
* **Atributos Principais:**
  * **HP (Vida):** 30.0
  * **Comportamento Base:** Leitura e espelhamento da posição passada do jogador.
* **Comportamento e Inteligência (IA):**
  * Não se move por forças normais físicas. Em vez disso, mantém uma fila (Buffer Circular) com o histórico de posições X e Y exatas que o NANOCELL-1 ocupou no passado.
  * O Príon se teletransporta fluidamente para o local exato onde o jogador estava há 1.5 a 0.8 segundos atrás (o atraso cai conforme a vida do Príon abaixa).
  * Como ele é literalmente um fantasma do passado temporal, a melhor estratégia de evasão contra ele é nunca fazer movimentos circulares onde você cruza o próprio rastro.
* **Ataque:** Dispara projéteis predatórios mirando no jogador.

### 8. Vírus Delta
* **Tipo:** Hacker Bipolar (Atirador e Espelho)
* **Polaridade:** Dinâmica (Vermelho/Azul)
* **Atributos Principais:**
  * **HP (Vida):** 45.0
* **Comportamento e Inteligência (IA):**
  * Reflete a movimentação horizontal (X) do jogador. Se a nave vai para a esquerda, ele desliza para a direita, agindo como um espelho de manobras de desvio.
* **Ataque:** Lê a cor atual do escudo do jogador e sempre dispara projéteis simultâneos da cor contrária, forçando o jogador a estar pronto para usar a mecânica de Barrel Roll (dash) ou trocar freneticamente de cor.

### 9. Nexus Omega
* **Tipo:** Boss Final (Formas Múltiplas)
* **Polaridade:** Dinâmica
* **Atributos Principais:**
  * **HP Máximo:** 6000.0 (O Titã Absoluto)
  * **Raio de Colisão:** 40.0 (Hitbox de núcleo, requer extrema precisão do jogador)
* **Comportamento e Máquina de Estados (3 Modos):**
  * O Chefão definitivo do jogo, a máquina viral blindada que reage à iluminação como metal polido.
  * Dança majestosamente no eixo X formando uma figura em "Oito" que fica mais rápida conforme sofre dano.
  * **Estágio 1 - Majestoso (Acima de 4000 HP):** Dispara estrelas cadentes duplas ultra-rápidas em 5 pontas rotativas.
  * **Estágio 2 - Oclusão (Entre 4000 e 1500 HP):** Acelera o passo, persegue o jogador levemente no X, e projeta *Paredes de Lasers Intercaladas*. O jogador precisa dar dash perfeitamente entre as frestas ou alternar a polaridade correndo de um lado pro outro.
  * **Estágio 3 - Fúria Final (Abaixo de 1500 HP):** Movimentação agressiva rastreando e seguindo o jogador. Libera uma Dupla Espiral Bullet-Hell monstruosa e gigantesca em 360 graus, enquanto atira "Snipes" rápidos focados (teleguiados) na Nanocell aleatoriamente.

---

## Resumo Comparativo

| Inimigo | Fase | Tipo | Polaridade | HP | Destaque / Ataque Principal |
|---|---|---|---|---|---|
| **Vírus Alfa** | 1 | Enxame Kamikaze | Azul | 15.0 | Mergulho guiado em Z (Vel: 250). |
| **Bactéria Coco** | 1 | Tanque Atirador | Vermelha | 40.0 | Descida lenta. Tiro frontal duplo a cada 2s. |
| **Leukocyte Corrupto** | 1 | BOSS | Dinâmica | 1500.0 | Patrulha, metralhadora em espiral e Bullet-Hell. |
| **Vírus Gama** | 2 | Atirador Bipolar | Alternante (3s) | 15.0 | Troca de cor. Tiro teleguiado rápido herdando a cor. |
| **Esporo Fúngico** | 2 | Kamikaze | Azul | 10.0 | Tracking hostil X/Y. Altíssimo dano explosivo no impacto. |
| **Pneumococo Gigante** | 2 | BOSS | Dinâmica (Tiros) | 600.0 | Ciclo respiratório em 3 Fases. Bullet-Hell rotativo escalonado. |
| **Príon Mimético** | 3 | Reflexo Temporal | Oposta (Jogador) | 30.0 | Copia o movimento passado do jogador (Buffer). |
| **Vírus Delta** | 3 | Espelho Atirador | Dinâmica (Oposta)| 20.0 | Espelha o eixo X e atira na cor inversa. |
| **Nexus Omega** | 3 | BOSS FINAL | Dinâmica | 6000.0 | 3 Modos (Estrelas, Paredes Lasers, Fúria Espiral). |
