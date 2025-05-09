# Makefile principal pour RAUCB_V2

# Répertoires
ROOT_DIR := $(shell pwd)
CLIENT_DIR := $(ROOT_DIR)/client
DOCKER_CLIENT_DIR := $(ROOT_DIR)/docker/client
SERVER_DIR := $(ROOT_DIR)/server
DOCKER_DIR := $(ROOT_DIR)/docker
BUILD_DIR := $(CLIENT_DIR)/build

# Scripts & Fichiers
CROSS_COMPILE_SCRIPT := $(DOCKER_DIR)/cross-compile.sh
SERVER_APP := $(SERVER_DIR)/app
PY_SERVER_SCRIPT := $(SERVER_DIR)/app.py
GEN_CERT_SCRIPT := $(ROOT_DIR)/gen_cert.py

# Binaire attendu
BINARY_NAME := client

# Phony targets
.PHONY: all clean cross-compile server gen_cert help

# Cible par défaut
all: help

# Compilation croisée
cross-compile:
	@echo "🚀 Lancement de la compilation croisée..."
	@bash $(CROSS_COMPILE_SCRIPT)

# Lancer le serveur Python
server:
	@echo "🌐 Lancement du serveur Python..."
	@python3 $(PY_SERVER_SCRIPT)

# Générer les certificats
gen_cert:
	@echo "🔐 Génération des certificats SSL..."
	@python3 $(GEN_CERT_SCRIPT)

# Nettoyage des fichiers générés
clean:
	@echo "🧹 Nettoyage du projet..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "🗑️ Suppression du dossier de build : $(BUILD_DIR)"; \
		sudo rm -rf "$(BUILD_DIR)"; \
	fi
	@if [ -d "$(DOCKER_CLIENT_DIR)" ]; then \
		echo "🗑️ Suppression du dossier client copié dans docker : $(DOCKER_CLIENT_DIR)"; \
		sudo rm -rf "$(DOCKER_CLIENT_DIR)"; \
	fi
	@if [ -f "$(SERVER_APP)" ]; then \
		echo "🗑️ Suppression du binaire compilé dans le dossier server : $(SERVER_APP)"; \
		sudo rm -f "$(SERVER_APP)"; \
	fi
	@echo "✅ Nettoyage terminé."

# Aide
help:
	@echo "📦 Commandes disponibles :"
	@echo "  make               → Affiche cette aide"
	@echo "  make cross-compile → Lance le script de compilation croisée"
	@echo "  make server        → Lance le serveur Python"
	@echo "  make gen_cert      → Génère les certificats SSL (gen_cert.py)"
	@echo "  make clean         → Supprime les fichiers générés"
