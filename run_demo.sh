#!/bin/bash

echo "Compilando o projeto..."
make all

if [ $? -eq 0 ]; then
    echo "Compilacao bem sucedida!"
    echo "Executando em modo DEMO..."
    ./codebattle demo
else
    echo "Erro na compilacao."
fi
