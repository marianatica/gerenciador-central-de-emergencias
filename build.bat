@echo off
rem Compila o programa (so a main.c, que inclui os outros arquivos .c) e executa.
gcc -Iinclude src\main.c -o central.exe
if errorlevel 1 (
    echo Erro na compilacao.
    exit /b 1
)
.\central.exe
