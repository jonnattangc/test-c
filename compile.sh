#!/bin/sh

echo "Compilación..."
g++ base64.cpp -o base64

if [ -e base64 ]
then
  echo "Ejecución..."
  ./base64
fi
