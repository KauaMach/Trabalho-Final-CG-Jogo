# Sistema de Colisões - NTF-CG-Jogo

A mecânica de colisão do jogo é construída através de **Esferas Matemáticas Invisíveis (Hitboxes Esféricas ou Radial Collision)**. Esta técnica não utiliza as malhas (vértices e polígonos) dos modelos 3D originais, garantindo altíssimo desempenho no motor do jogo.

---

## 1. O Box de Colisão da Nanocel (Nave do Jogador)
- **Formato:** Esfera.
- **Implementação:** O modelo 3D não possui colisão física embutida em seu arquivo original. Em vez disso, foi definida via código uma constante `raioPlayer = 28.0f`.
- **Efeito:** Essa variável envolve o centro geométrico exato `(X, Y, Z)` da Nanocel com um raio de 28 unidades, funcionando como um "campo de força" invisível que cobre toda a lataria e as asas da nave.

---

## 2. O Box de Colisão do Vírus Alfa (Inimigo)
- **Formato:** Esfera (menor que a da nave).
- **Implementação:** Semelhante à Nanocel, a classe `Inimigo` possui a propriedade interna `raioColisao = 18.0f;`.
- **Efeito:** Uma bolha de 18 unidades envolve o modelo tridimensional do vírus, ditando os seus limites físicos para interações.

---

## 3. A Matemática de Impacto Físico
O jogo checa o impacto físico usando o Teorema de Pitágoras (implementado na função `Distancia3D`). 
O motor calcula continuamente a distância vetorial absoluta entre o ponto central da Nave e o ponto central do Vírus.
Se essa **distância for estritamente menor que a soma dos dois raios** (`28.0f + 18.0f = 46.0f`), o sistema entende que as esferas se sobrepuseram. No mesmo frame, o dano na lataria é aplicado ao jogador e o inimigo explode.

---

## 4. Interação Laser vs. Inimigos
Mesmo antes da colisão física da nave principal ser implementada, os lasers já destruíam os inimigos. Isso ocorreu por conta da arquitetura modular do motor (no `main.cpp`).

- **Rotina Independente:** Existia uma função separada, `Colisao::ChecarLaserInimigo`, rodando ativamente no loop do jogo.
- **Hitbox do Tiro:** Cada laser também possui uma hitbox esférica mínima (raio de `5.0f`).
- **Lógica Anterior:** O motor varria a posição de todos os lasers no mapa e checava suas distâncias matemáticas em relação aos inimigos vivos. A rotina do tiro já disparava os acertos radialmente, sendo completamente autônoma em relação ao corpo físico (chassi) do jogador.
