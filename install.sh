#!/bin/bash

# Répertoire d'installation sur le Bureau de l'utilisateur
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    INSTALL_DIR="$HOME/Desktop/mp4_analyser"  # Installation sur le bureau sous Linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
    INSTALL_DIR="$HOME/Desktop/mp4_analyser"  # Installation sur le bureau sous macOS
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then
    INSTALL_DIR="$USERPROFILE\\Desktop\\mp4_analyser"  # Installation sur le bureau sous Windows (Git Bash ou MSYS)
else
    echo "Système d'exploitation non pris en charge."
    exit 1
fi

# Fonction pour installer les dépendances
install_dependencies() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "Installation des dépendances sur Linux..."
        sudo apt-get update
        sudo apt-get install -y curl libncurses5-dev libjansson-dev build-essential
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

# Fonction pour installer libcurl dans le répertoire du projet si non installé
install_libcurl() {
    echo "Vérification de la présence de libcurl..."

    if [[ ! -f "$INSTALL_DIR/libcurl.a" && ! -f "$INSTALL_DIR/libcurl.so" ]]; then
        echo "libcurl non trouvé. Téléchargement et installation dans le répertoire du projet..."

        # Télécharger et extraire libcurl si elle n'est pas présente
        cd $INSTALL_DIR
        curl -LO https://curl.se/download/curl-7.79.1.tar.gz
        tar -xvf curl-7.79.1.tar.gz
        cd curl-7.79.1

        # Vérification de la présence des outils nécessaires
        if ! command -v autoconf &>/dev/null || ! command -v make &>/dev/null || ! command -v gcc &>/dev/null; then
            echo "Les outils nécessaires (autoconf, make, gcc) ne sont pas installés. Veuillez les installer."
            exit 1
        fi

        # Compilation de libcurl avec les bonnes options
        ./configure --prefix=$INSTALL_DIR
        make
        if [ $? -ne 0 ]; then
            echo "Erreur lors de la compilation de libcurl."
            exit 1
        fi
        make install
        if [ $? -ne 0 ]; then
            echo "Erreur lors de l'installation de libcurl."
            exit 1
        fi

        # Vérification de l'installation de libcurl
        echo "libcurl installé dans $INSTALL_DIR"
    else
        echo "libcurl déjà installé."
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
    unzip -o -q mp4_analyser.zip -d $INSTALL_DIR  # Utilisation de l'option -o pour forcer l'écrasement
    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'extraction des fichiers."
        exit 1
    fi
}

# Fonction pour remplacer les fichiers existants (comme README) si nécessaire
replace_existing_files() {
    echo "Vérification des fichiers existants dans $INSTALL_DIR..."

    # Si README ou d'autres fichiers existent, les supprimer avant d'extraire les nouveaux fichiers
    if [ -f "$INSTALL_DIR/README.md" ]; then
        echo "README.md trouvé, remplacement en cours..."
        rm -f "$INSTALL_DIR/README.md"
    fi
}

# Fonction pour compiler les fichiers source
compile_programs() {
    cd $INSTALL_DIR/mp4_analyser-main || exit
    echo "Compilation des programmes..."

    # Compiler chaque fichier source avec les chemins libcurl et ncurses
    gcc -o r r.c -lncurses -ljansson -lcurl -L$INSTALL_DIR/lib -I$INSTALL_DIR/include  # Spécifie où trouver libcurl
    gcc -o meta meta.c -lncurses -ljansson -lcurl -L$INSTALL_DIR/lib -I$INSTALL_DIR/include
    gcc -o analyse analyse.c -lncurses -ljansson -lcurl -L$INSTALL_DIR/lib -I$INSTALL_DIR/include
    gcc -o cryptage cryptage.c -lncurses -ljansson -lcurl -L$INSTALL_DIR/lib -I$INSTALL_DIR/include

    if [ $? -ne 0 ]; then
        echo "Erreur lors de la compilation des programmes."
        exit 1
    fi
}

# Fonction principale d'installation
install() {
    # Créer le répertoire d'installation sur le bureau s'il n'existe pas
    if [ ! -d "$INSTALL_DIR" ]; then
        mkdir -p "$INSTALL_DIR"
    fi

    # Appeler les fonctions pour l'installation
    install_dependencies
    install_libcurl  # Installer libcurl si nécessaire
    replace_existing_files  # Vérification et remplacement des fichiers existants
    download_and_extract
    compile_programs

    echo "Installation terminée avec succès."
    echo "Les programmes ont été installés dans : $INSTALL_DIR"
}

# Lancer l'installation
install
