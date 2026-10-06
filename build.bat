@echo off
rem Compila o programa no Windows com o GCC e gera central.exe na pasta do projeto.
rem O laco "for" junta todos os arquivos .c da pasta src, para nao precisar lista-los a mao.

setlocal enabledelayedexpansion
set ARQUIVOS=
for %%f in (src\*.c) do set ARQUIVOS=!ARQUIVOS! %%f

gcc -std=c11 -Wall -Wextra -Iinclude !ARQUIVOS! -o central.exe
if errorlevel 1 (
    echo Erro na compilacao.
    exit /b 1
)
echo Compilado com sucesso. Para executar: central.exe
