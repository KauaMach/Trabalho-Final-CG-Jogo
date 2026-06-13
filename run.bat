@echo off
title Imunidade: A Guerra Celular

:: Injeta o caminho exato do seu compilador UCRT64
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo ========================================
echo   IMUNIDADE: A GUERRA CELULAR - ENGINE  
echo ========================================

echo [1/3] Limpando compilacoes anteriores...
:: Usa o comando nativo que o pacote do MSYS2 UCRT64 disponibiliza
mingw32-make clean 2>nul

echo [2/3] Compilando modulos do projeto...
:: Executa a compilação oficial utilizando o Makefile
mingw32-make

if exist imunidade_jogo.exe (
    echo [3/3] Sucesso! Iniciando NANOCELL-1...
    echo ----------------------------------------
    imunidade_jogo.exe
) else (
    echo.
    echo Erro Critico: O executavel nao pode ser gerado.
    echo Verifique se os caminhos dos arquivos no seu Makefile estao corretos.
    pause
    exit /b 1
)