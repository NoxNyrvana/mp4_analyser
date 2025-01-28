

# Fonction pour installer les dépendances sous Linux
install_linux() {
    echo "Détecté : Linux"
    echo "Mise à jour des paquets et installation des dépendances..."
    sudo apt update
    sudo apt install -y gcc make libcurl4-openssl-dev libjansson-dev unzip
    echo "Dépendances installées pour Linux."
}

# Fonction pour installer les dépendances sous macOS
install_macos() {
    echo "Détecté : macOS"
    
    # Vérification si Homebrew est installé
    if ! command -v brew &> /dev/null
    then
        echo "Homebrew n'est pas installé. Veuillez l'installer d'abord en suivant les instructions ici : https://brew.sh/"
        exit 1
    fi

    # Installer les dépendances avec Homebrew
    echo "Installation des dépendances avec Homebrew..."
    brew install gcc make curl jansson unzip
    echo "Dépendances installées pour macOS."
}

# Fonction pour installer les dépendances sous Windows (WSL ou Chocolatey)
install_windows() {
    echo "Détecté : Windows"
    
    # Vérifier si WSL (Windows Subsystem for Linux) est utilisé
    if grep -q Microsoft /proc/version; then
        # Si WSL est utilisé (Linux sous Windows)
        echo "WSL détecté, installation des dépendances sous Linux..."
        sudo apt update
        sudo apt install -y gcc make libcurl4-openssl-dev libjansson-dev unzip
        echo "Dépendances installées sous WSL (Linux sur Windows)."
    else
        # Si WSL n'est pas utilisé, utiliser Chocolatey
        echo "Installation des dépendances avec Chocolatey (Windows)..."
        choco install -y mingw make curl jansson unzip
        echo "Dépendances installées avec Chocolatey."
    fi
}

# Détection du système d'exploitation
OS=$(uname -s)

# Appeler la fonction appropriée en fonction de l'OS
if [[ "$OS" == "Linux" ]]; then
    install_linux
elif [[ "$OS" == "Darwin" ]]; then
    install_macos
elif [[ "$OS" == "CYGWIN"* || "$OS" == "MINGW"* ]]; then
    install_windows
else
    echo "Système d'exploitation non pris en charge. Ce script ne fonctionne que sur Linux, macOS et Windows."
    exit 1
fi

# Téléchargement du projet depuis GitHub
echo "Téléchargement du projet depuis GitHub..."
curl -L https://github.com/NoxNyrvana/mp4_analyser/archive/refs/heads/main.zip -o mp4_analyser-main.zip

# Extraction de l'archive
echo "Extraction du projet..."
unzip -q mp4_analyser-main.zip
cd mp4_analyser-main || { echo "Erreur : impossible d'accéder au répertoire du projet."; exit 1; }

# Compilation du projet
echo "Compilation du projet..."
make || { echo "Erreur : la compilation a échoué."; exit 1; }

# Installation terminée
echo "Installation terminée zemzemzem"
