#!/bin/bash

# === Définition des chemins ===
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
ORIGINAL_CLIENT_DIR="$PROJECT_DIR/client"
DOCKER_DIR="$SCRIPT_DIR"
DOCKER_CLIENT_DIR="$DOCKER_DIR/client"
SERVER_DIR="$PROJECT_DIR/server"
DOCKER_IMAGE_TAR="$DOCKER_DIR/qtcrossbuildApp.tar"
DOCKER_IMAGE_NAME="qtcrossbuild_smaller_app_builder:latest"

# === Couleurs et icônes ===
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[1;34m'
CYAN='\033[1;36m'
BOLD='\033[1m'
NC='\033[0m'

CHECKMARK="\xE2\x9C\x94"
CROSSMARK="\xE2\x9D\x8C"
HOURGLASS="\xE2\x8C\x9B"
FOLDER="\xF0\x9F\x93\x81"
COMPILER="\xF0\x9F\x94\xA7"
ARROW="\xE2\x9E\xA1"
PACKAGE="\xF0\x9F\x93\xA6"
ROCKET="\xF0\x9F\x9A\x80"

echo -e "${BLUE}${BOLD}${ROCKET}  === Script de cross-compilation RAUCB_V2 ===${NC}"
echo -e "${CYAN}${FOLDER}  Répertoire du script : $SCRIPT_DIR"
echo -e "${CYAN}${FOLDER}  Répertoire du projet : $PROJECT_DIR${NC}"

# === Étape 1 : Vérification et copie du dossier client ===
echo -e "${YELLOW}${HOURGLASS}  Étape 1 : Copie du dossier client dans docker/...${NC}"
if [ ! -d "$ORIGINAL_CLIENT_DIR" ]; then
    echo -e "${RED}${CROSSMARK}  Erreur : Dossier client introuvable à $ORIGINAL_CLIENT_DIR${NC}"
    exit 1
fi

if [ ! -d "$DOCKER_CLIENT_DIR" ]; then
    cp -r "$ORIGINAL_CLIENT_DIR" "$DOCKER_DIR/"
    echo -e "${GREEN}${CHECKMARK}  Dossier client copié dans docker/${NC}"
else
    echo -e "${BLUE}${ARROW}  Dossier client déjà présent dans docker/, aucune action nécessaire${NC}"
fi

# === Étape 2 : Copier CMakeLists.txt ===
echo -e "${YELLOW}${HOURGLASS}  Étape 2 : Copie de CMakeLists.txt dans docker/client...${NC}"
if [ -f "$DOCKER_DIR/CMakeLists.txt" ]; then
    cp "$DOCKER_DIR/CMakeLists.txt" "$DOCKER_CLIENT_DIR/"
    echo -e "${GREEN}${CHECKMARK}  CMakeLists.txt copié avec succès${NC}"
else
    echo -e "${RED}${CROSSMARK}  Erreur : Fichier CMakeLists.txt introuvable dans $DOCKER_DIR${NC}"
    exit 1
fi

# === Étape 3 : Suppression et rechargement de l’image Docker ===
echo -e "${YELLOW}${HOURGLASS}  Étape 3 : Mise à jour de l'image Docker...${NC}"
docker image rm -f "$DOCKER_IMAGE_NAME" >/dev/null 2>&1
if [ -f "$DOCKER_IMAGE_TAR" ]; then
    docker load -i "$DOCKER_IMAGE_TAR"
    echo -e "${GREEN}${CHECKMARK}  Image Docker rechargée depuis $DOCKER_IMAGE_TAR${NC}"
else
    echo -e "${RED}${CROSSMARK}  Erreur : Archive Docker introuvable à $DOCKER_IMAGE_TAR${NC}"
    exit 1
fi

# === Étape 4 : Compilation via Docker ===
echo -e "${YELLOW}${COMPILER}  Étape 4 : Lancement de la compilation Docker...${NC}"

# Chemin absolu Linux (le montage doit se faire avec le chemin Linux même sous WSL)
docker run -v "$DOCKER_CLIENT_DIR:/build/project" "$DOCKER_IMAGE_NAME"
if [ $? -ne 0 ]; then
    echo -e "${RED}${CROSSMARK}  Erreur : La compilation a échoué${NC}"
    exit 1
fi

# === Étape 5 : Copier le binaire dans server/ et renommer ===
echo -e "${YELLOW}${HOURGLASS}  Étape 5 : Déplacement du binaire vers server/...${NC}"
if [ -f "$DOCKER_CLIENT_DIR/client" ]; then
    mkdir -p "$SERVER_DIR"
    cp "$DOCKER_CLIENT_DIR/client" "$SERVER_DIR/app"
    chmod +x "$SERVER_DIR/app"
    echo -e "${GREEN}${CHECKMARK}  Binaire copié et renommé en ${BOLD}app${NC}"
    echo -e "${CYAN}${PACKAGE}  Chemin final : $SERVER_DIR/app${NC}"
else
    echo -e "${RED}${CROSSMARK}  Erreur : Binaire 'client' non trouvé dans $DOCKER_CLIENT_DIR${NC}"
    exit 1
fi

# === Fin ===
echo -e "${GREEN}${BOLD}${ROCKET}  Cross-compilation terminée avec succès !${NC}"
