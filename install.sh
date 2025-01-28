#!/bin/bash

# Fonction pour installer les dépendances sur Linux/macOS
install_dependencies_linux_macos() {
    echo "Installation des dépendances nécessaires..."

    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Pour Linux (Debian/Ubuntu)
        sudo apt update
        sudo apt install -y curl libncurses5-dev libncursesw5-dev libjansson-dev
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # Pour macOS (Homebrew)
        brew install curl ncurses jansson
    fi

    echo "Dépendances installées avec succès."
}

# Fonction pour installer les dépendances sur Windows (WSL ou Git Bash avec MSYS2)
install_dependencies_windows() {
    echo "Installation des dépendances nécessaires sur Windows..."

    # Si on est sur WSL (Ubuntu sous Windows), on installe avec apt
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        sudo apt update
        sudo apt install -y curl libncurses5-dev libncursesw5-dev libjansson-dev
    elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "mingw"* ]]; then
        # Si on est sur Git Bash avec MinGW ou MSYS2
        pacman -S mingw-w64-x86_64-curl mingw-w64-x86_64-ncurses mingw-w64-x86_64-jansson
    fi

    echo "Dépendances installées avec succès."
}

# Détection du système d'exploitation et installation des dépendances
OS=$(uname -s)

if [[ "$OS" == "Linux" || "$OS" == "Darwin" ]]; then
    install_dependencies_linux_macos
elif [[ "$OS" == "CYGWIN"* || "$OS" == "MINGW"* ]]; then
    install_dependencies_windows
else
    echo "Système d'exploitation non pris en charge. Ce script fonctionne sur Linux, macOS, et Windows (via WSL ou Git Bash)."
    exit 1
fi

# Télécharger, décompresser et installer les fichiers sources
echo "Téléchargement du projet..."
curl -sSL https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip -o mp4_analyser.zip

echo "Décompression du fichier téléchargé..."
unzip mp4_analyser.zip -d mp4_analyser

echo "Déplacement vers le dossier décompressé..."
cd mp4_analyser/mp4_analyser-main

# Compilation des fichiers source en exécutables
echo "Compilation des fichiers source..."

gcc -o meta meta.c -lncurses -ljansson
gcc -o cryptage cryptage.c -lncurses -ljansson
gcc -o analyse analyse.c -lncurses -ljansson

# Déplacer les exécutables compilés sur le bureau
echo "Déplacement des exécutables vers le bureau..."
mkdir -p ~/Desktop/mp4_analyser
mv meta cryptage analyse ~/Desktop/mp4_analyser/

echo "Installation terminée avec succès !"
echo "Vous pouvez maintenant exécuter les programmes à partir du dossier ~/Desktop/mp4_analyser."

