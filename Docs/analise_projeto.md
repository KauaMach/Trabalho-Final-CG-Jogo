# Análise do Projeto Original (Imunidade: A Guerra Celular)

Baseado no código e na documentação do projeto `Trabalho-Final-CG-Jogo`, esta análise servirá como fundação para a refatoração e recriação em `NTF-CG-Jogo`.

## 1. Arquitetura e Tecnologias
- **Linguagem:** C++ (padrão C++17).
- **Gráficos e Janela:** OpenGL 3D (com jogabilidade em 2D ortográfica) e freeGLUT.
- **Áudio:** SDL2_mixer.
- **Matemática e Assets:** GLM (matemática vetorial), stb_image (leitura de texturas PNG), tinyobjloader (leitura de modelos OBJ 3D).
- **Estrutura de Pastas Esperada:**
  - `/src`: Código-fonte central, contendo módulos de IA, renderização, entidades, colisões e controle de cenas.
  - `/assets`: Recursos estáticos como texturas, modelos 3D e áudios.
  - `/include`: Bibliotecas de terceiros (*header-only*).

## 2. Mecânicas Principais
- **Polaridade Cromática (Azul ↔ Vermelho):** A mecânica principal do jogo (inspirada em Ikaruga). A nave do jogador absorve os projéteis da mesma cor (carregando o especial *SURGE*) e recebe dano da cor oposta.
- **Saúde do Paciente (HSP):** Funciona como barra de progresso da fase e indicador de game over. Matar inimigos ou bosses aumenta a vida, enquanto receber dano ou deixar inimigos passarem a reduz.
- **IA Avançada:**
  - Algoritmo de Boids: Cria comportamentos orgânicos de enxames para os vírus inimigos utilizando regras de separação, alinhamento e coesão.
  - Otimização com *Spatial Hashing* (Grade 32x32) para verificação de colisões e interação de boids.

## 3. Avaliação de Código e Pontos de Refatoração
- **Centralização de Lógica:** O arquivo `main.cpp` no projeto original é grande (~22KB), o que sugere acúmulo de callbacks (input, timer, display) e lógica de jogo de forma pouco modular.
- **Estruturação Orientada a Objetos:** Para a reconstrução em `NTF-CG-Jogo`, o foco principal da refatoração deve ser organizar corretamente as classes, separando os componentes:
  - Motores (Renderização, Áudio, Física/Colisão).
  - Entidades (Player, Inimigos, Projéteis).
  - Controle de Estados/Cenas (Menu, Fases, Game Over).

## Conclusão
A recriação do projeto deve focar em modularizar rigorosamente os componentes declarados, isolando especialmente a forte dependência com OpenGL e freeGLUT em classes *wrappers* ou gerenciadoras específicas, deixando a lógica das entidades mais limpa e testável.
