#!/bin/bash

# Répertoire d'installation par défaut
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    INSTALL_DIR="$HOME/mp4_analyser"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    INSTALL_DIR="$HOME/mp4_analyser"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    INSTALL_DIR="$USERPROFILE\\Desktop\\mp4_analyser"
else
    echo "Système d'exploitation non pris en charge."
    exit 1
fi

# Fonction pour installer les dépendances
install_dependencies() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "Installation des dépendances sur Linux..."
        sudo apt-get update
        sudo apt-get install -y curl libncurses5-dev libjansson-dev
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "Installation des dépendances sur macOS..."
        brew install curl ncurses jansson
    elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
        echo "Installation des dépendances sur Windows (Git Bash ou MSYS)..."
        pacman -S mingw-w64-x86_64-curl mingw-w64-x86_64-ncurses mingw-w64-x86_64-jansson
    else
        echo "Système d'exploitation non pris en charge pour l'installation des dépendances."
        exit 1
    fi
}

# Fonction pour télécharger et extraire le projet
download_and_extract() {
    echo "Téléchargement du projet depuis GitHub..."
    curl -sSL https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip -o mp4_analyser.zip
    if [ $? -ne 0 ]; then
        echo "Erreur lors du téléchargement avec curl. Vérifiez les permissions et l'espace disque."
        exit 1
    fi

    echo "Extraction des fichiers..."
    unzip mp4_analyser.zip -d $INSTALL_DIR  # Utilisation de l'option -o pour forcer l'écrasement
    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'extraction des fichiers."
        exit 1
    fi
}

# Fonction pour remplacer les fichiers existants (comme README) si nécessaire
replace_existing_files() {
    echo "Vérification des fichiers existants dans $INSTALL_DIR..."

    # Si README ou d'autres fichiers existent, les supprimer avant d'extraire les nouveaux fichiers
    if [ -f "$INSTALL_DIR/mp4_analyser-main/README.md" ]; then
        echo "README.md trouvé, remplacement en cours..."
        rm -f "$INSTALL_DIR/mp4_analyser-main/README.md"
    fi

    # Ajoute ici d'autres fichiers que tu souhaites remplacer s'ils existent
    # Exemple : si un fichier .gitignore existe, tu peux le supprimer :
    if [ -f "$INSTALL_DIR/.gitignore" ]; then
        echo ".gitignore trouvé, remplacement en cours..."
        rm -f "$INSTALL_DIR/.gitignore"
    fi
}

# Fonction pour compiler les fichiers source
compile_programs() {
    cd $INSTALL_DIR/mp4_analyser-main || exit
    echo "Compilation des programmes..."

    # Compiler chaque fichier source
    gcc -o r r.c -lncurses -ljansson -lcurl
    gcc -o meta meta.c -lncurses -ljansson -lcurl
    gcc -o analyse analyse.c -lncurses -ljansson -lcurl
    gcc -o cryptage cryptage.c -lncurses -ljansson -lcurl

    if [ $? -ne 0 ]; then
        echo "Erreur lors de la compilation des programmes."
        exit 1
    fi
}

# Fonction principale d'installation
install() {
    # Créer le répertoire d'installation s'il n'existe pas
    if [ ! -d "$INSTALL_DIR" ]; then
        mkdir -p "$INSTALL_DIR"
    fi

    # Appeler les fonctions pour l'installation
    install_dependencies
    replace_existing_files  # Vérification et remplacement des fichiers existants
    download_and_extract
    compile_programs

    echo "Installation terminée avec succès."
    echo "Les programmes ont été installés dans : $INSTALL_DIR"
}

# Lancer l'installation
install
