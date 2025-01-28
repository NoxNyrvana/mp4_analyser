#!/bin/bash

# Fonction pour télécharger, dézipper et compiler sous Linux/macOS
install_linux_macos() {
    echo "Détecté : $1"
    
    # Créer un répertoire sur le bureau pour l'installation
    DESKTOP_PATH="$HOME/Desktop/mp4_analyser"
    mkdir -p "$DESKTOP_PATH"
    echo "Dossier de destination : $DESKTOP_PATH"

    # Télécharger l'archive depuis GitHub (sources ou exécutables précompilés)
    echo "Téléchargement de l'archive depuis GitHub..."
    curl -L https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip -o mp4_analyser-main.zip

    # Extraire l'archive dans le répertoire du bureau
    echo "Extraction de l'archive..."
    unzip -q mp4_analyser-main.zip -d "$DESKTOP_PATH"

    # Aller dans le répertoire où les fichiers ont été extraits
    cd "$DESKTOP_PATH/mp4_analyser-main" || { echo "Erreur : impossible d'accéder au répertoire extrait."; exit 1; }

    # Compiler les fichiers source si nécessaire (Linux/macOS)
    echo "Compilation des sources..."
    if gcc -o mp4_analyser r.c meta.c analyse.c cryptage.c -lcurl -ljansson; then
        echo "Compilation réussie."
    else
        echo "Erreur : la compilation a échoué."
        exit 1
    fi

    echo "Installation terminée. Le programme est installé sur votre bureau."
}

# Fonction pour installer sous Windows (WSL ou Git Bash)
install_windows() {
    echo "Détecté : Windows"

    # Créer un répertoire sur le bureau pour l'installation
    DESKTOP_PATH="$HOME/Desktop/mp4_analyser"
    mkdir -p "$DESKTOP_PATH"
    echo "Dossier de destination : $DESKTOP_PATH"

    # Télécharger l'archive depuis GitHub (sources ou exécutables précompilés)
    echo "Téléchargement de l'archive depuis GitHub..."
    curl -L https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip -o mp4_analyser-main.zip

    # Extraire l'archive dans le répertoire du bureau
    echo "Extraction de l'archive..."
    unzip -q mp4_analyser-main.zip -d "$DESKTOP_PATH"

    # Aller dans le répertoire où les fichiers ont été extraits
    cd "$DESKTOP_PATH/mp4_analyser-main" || { echo "Erreur : impossible d'accéder au répertoire extrait."; exit 1; }

    # Compiler les fichiers source si nécessaire (Windows avec WSL ou Git Bash)
    echo "Compilation des sources..."
    if gcc -o mp4_analyser.exe main.c meta.c analyse.c cryptage.c -lcurl -ljansson; then
        echo "Compilation réussie."
    else
        echo "Erreur : la compilation a échoué."
        exit 1
    fi

    echo "Installation terminée. Le programme est installé sur votre bureau."
}

# Détecter le système d'exploitation
OS=$(uname -s)

# Appeler la fonction appropriée en fonction du système d'exploitation
if [[ "$OS" == "Linux" || "$OS" == "Darwin" ]]; then
    install_linux_macos "$OS"
elif [[ "$OS" == "CYGWIN"* || "$OS" == "MINGW"* ]]; then
    install_windows
else
    echo "Système d'exploitation non pris en charge. Ce script fonctionne sur Linux, macOS, et Windows (via WSL ou Git Bash)."
    exit 1
fi
