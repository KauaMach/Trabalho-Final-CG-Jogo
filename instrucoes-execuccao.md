# Instruções de Execução - Jogo "Imunidade"

Este projeto utiliza C++17, OpenGL, GLUT, GLEW e as bibliotecas SDL2/SDL2_mixer para áudio. 

Para gerenciar e automatizar o processo de compilação, o projeto utiliza o **Make** (através do arquivo `Makefile`). O Make é uma ferramenta de compilação que lê as instruções definidas no `Makefile` para organizar os comandos e dependências de forma automática. Ele foi escolhido para este projeto pois simplifica a vinculação das diversas bibliotecas em diferentes sistemas operacionais, permitindo que o jogo seja compilado rapidamente apenas digitando o comando `make`, sem a necessidade de reescrever manualmente longas linhas de comando no terminal.

Abaixo estão as instruções detalhadas para configurar o ambiente, compilar e executar o jogo nas plataformas Linux, macOS e Windows.

---

## 1. Linux

### Dependências
Certifique-se de ter os compiladores e as bibliotecas de desenvolvimento instaladas no seu sistema. Em distribuições baseadas em Debian/Ubuntu, você pode instalar todas as dependências rodando:

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libglew-dev libsdl2-dev libsdl2-mixer-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Compilação e Execução
1. Abra o terminal na pasta raiz do projeto (`NTF-CG-Jogo`).
2. Compile o código utilizando a regra padrão do Makefile:
   ```bash
   make
   ```
3. Execute o jogo:
   ```bash
   ./imunidade
   ```

---

## 2. macOS

### Dependências
O macOS já inclui nativamente os *frameworks* do OpenGL e GLUT. No entanto, é necessário instalar o GLEW, SDL2 e SDL2_mixer. A maneira mais simples é utilizando o gerenciador de pacotes [Homebrew](https://brew.sh/):

```bash
brew install glew sdl2 sdl2_mixer
```
*(Nota: Certifique-se de que o Command Line Tools da Apple está instalado. Caso não esteja, rode `xcode-select --install` no terminal).*

### Compilação e Execução
1. Abra o terminal na pasta raiz do projeto.
2. Compile o código:
   ```bash
   make
   ```
3. Execute o jogo:
   ```bash
   ./imunidade
   ```

---

## 3. Windows

O projeto foi configurado no `Makefile` prevendo duas abordagens principais para a geração do executável `.exe` do Windows.

### Abordagem A: Compilação Cruzada (A partir do Linux/macOS)
Se você estiver no Linux ou macOS, pode gerar o executável do Windows nativamente utilizando o MinGW. O `Makefile` já tem a regra pronta (`make win`).

1. Instale o pacote de compilação cruzada MinGW-w64 no seu sistema host. No Ubuntu, por exemplo:
   ```bash
   sudo apt install mingw-w64
   ```
2. Na raiz do projeto, execute:
   ```bash
   make win
   ```
3. Isso irá gerar o arquivo `imunidade.exe`. Para testá-lo ou rodá-lo diretamente no Windows, você precisará transferi-lo e garantir que as bibliotecas dinâmicas `.dll` (freeglut.dll, glew32.dll, SDL2.dll, SDL2_mixer.dll) estejam na mesma pasta do executável.

### Abordagem B: Compilação Nativa (Usando MSYS2/MinGW no Windows)
Para desenvolver e rodar o projeto diretamente no Windows:

1. Baixe e instale o [MSYS2](https://www.msys2.org/).
2. Abra o terminal **MSYS2 MinGW 64-bit** e instale a *toolchain* de desenvolvimento e as bibliotecas:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-freeglut mingw-w64-x86_64-glew mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer
   ```
3. Pelo terminal do MSYS2, navegue até a pasta do projeto.
4. Execute o make (caso não encontre o `make`, use `mingw32-make`):
   ```bash
   mingw32-make
   ```
5. Execute o jogo criado:
   ```cmd
   imunidade.exe
   ```

*(Nota: Para executar o `.exe` diretamente com um duplo clique fora do terminal MSYS2, você precisará adicionar o diretório `C:\msys64\mingw64\bin` nas variáveis de ambiente `PATH` do Windows).*

---

### Limpeza de Arquivos
Sempre que for trocar de plataforma ou precisar de uma compilação limpa, utilize o comando abaixo para limpar os arquivos de objeto (`.o`) e executáveis antigos:
```bash
make clean
```
