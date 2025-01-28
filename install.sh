#!/bin/bash

# Variables
URL="https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip"
ZIP_FILE="main.zip"
EXTRACT_DIR="mp4_analyser-main"

# Fonctions auxiliaires
function check_and_install_dependencies() {
    echo "Mise à jour des dépôts et installation des dépendances..."
    sudo apt update
    sudo apt install -y gcc libncurses-dev libcurl4-openssl-dev curl libjansson-dev unzip
}

function download_and_extract() {
    echo "Téléchargement de l'archive..."
    curl -L $URL -o $ZIP_FILE
    
    echo "Extraction de l'archive..."
    unzip -o $ZIP_FILE
}

function compile_sources() {
    echo "Compilation des fichiers source..."
    cd $EXTRACT_DIR
    
    for file in *.c; do
        exe_file="${file%.c}" # Générer un nom sans extension
        echo "Compilation de $file en $exe_file..."
        gcc -o "$exe_file" "$file" -lncurses -lcurl -ljansson
    done
    
    echo "Compilation terminée. Les exécutables sont dans le dossier $EXTRACT_DIR."
}

function cleanup() {
    echo "Nettoyage des fichiers temporaires..."
    rm -f $ZIP_FILE
}

# Script principal
check_and_install_dependencies
download_and_extract
compile_sources
cleanup

echo "Installation terminée avec succès !"
