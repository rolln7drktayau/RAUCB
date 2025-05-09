# 📄 Projet RAUCB - Remote Application Update Client & Backend  

## 👤 Auteur : Roland Cédric TAYO  

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/rct/) [![GitHub](https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/rolln7drktayau/)

---

## 🗂 Structure du projet

```bash
project_root/
├── client/        # Application Qt (GUI) côté Raspberry Pi
│   ├── main.cpp
│   ├── mainwindow.*
│   ├── client.pro
│   ├── network/
│   │   └── networkmanager.*
│   └── security/
│       ├── encryption.*
│       ├── tlsmanager.*
│       ├── client_cert.pem
│       └── client_key.pem
├── server/        # Serveur Flask avec TLS pour fournir les mises à jour
│   ├── app.py
│   ├── app.sha256
│   ├── server_cert.pem
│   └── server_key.pem
├── docker/        # Fichiers utiles pour la compilation Docker
│   ├── cross-compile.sh # Script pour la compilation croisée
│   ├── CMakeLists.txt # CMake qui sera auto' rajouté au dossier client importé dans ce dossier
│   └── qtcrossbuildApp.tar # Image Docker pour la compilation croisée
├── gen_cert.py    # Script pour générer des certificats auto-signés
├── makefile       # Fichier Make pour automatiser les tâches
└── README.md      # Ce fichier
```

---

## 🚀 Objectif

Permettre à un client Qt (exécuté sur Raspberry Pi) de :

* Vérifier les mises à jour via HTTPS
* Télécharger un fichier binaire signé
* Vérifier le SHA256
* Le déchiffrer
* L'exécuter si tout est valide

---

## 🐳 Compilation via Docker

1. Assurez-vous que Docker est installé.
2. Chargez l'image si nécessaire :

```bash
docker load -i docker/qtcrossbuildApp.tar
```

3. Lancez la compilation croisée :

```bash
make cross-compile
```

Cela compile l'application Qt dans le dossier `client/` qui sera exporté dans le dossier docker (pour plus de propreté).
Le binaire sera généré et copié vers le dossier server depuis lequel il pourra être télécharger dans le raspberry pi.

---

## ⚙️ Commandes Make disponibles

```bash
make             # Affiche l'aide
make clean       # Supprime les builds, binaires et artefacts
make server      # Lance le serveur Flask (server/app.py)
make cross-compile # Lance la compilation croisée (docker/cross-compile.sh)
make gen_cert    # Génère un certificat SSL (via gen_cert.py)
```

---

## 🖥️ Lancement du serveur

1. Positionnez-vous dans le dossier `server/` (ou utilisez le script `make server`) :

```bash
cd server
```

2. Générez un certificat SSL auto-signé (ou utilisez `make gen_cert`) :

```bash
openssl req -x509 -newkey rsa:4096 -keyout server_key.pem -out server_cert.pem \
-days 365 -nodes -subj "/CN=localhost"
```

3. Préparez une mise à jour :

```bash
echo -n "Hello from new version!" > update_v2.bin
sha256sum update_v2.bin | awk '{print $1}' > app.sha256
```

4. Lancez le serveur :

```bash
make server
```

---

## 📲 Lancement du client (Qt)

1. Ouvrez le fichier `client/client.pro` dans QtCreator.
2. Compilez et exécutez l'application.
3. Cliquez sur "Check for Updates" pour déclencher une mise à jour automatique.

---

## 🔐 Sécurité

* ✅ Connexion HTTPS via TLS
* ✅ Certificats auto-signés pour développement
* ✅ SHA256 pour vérification d’intégrité
* 🔒 Chiffrement rsa

---

## 📦 Docker : Utilisation manuelle

Exécution manuelle de l’image (avec volume monté) :

```bash
docker run -v $(pwd)/client:/build/project qtcrossbuild_smaller_app_builder:latest
```

Avec option de debug :

```bash
docker run -v $(pwd)/client:/build/project qtcrossbuild_smaller_app_builder:latest -DQT_DEBUG_FIND_PACKAGE=ON
```

Remplace `$(pwd)` par le chemin absolu si nécessaire.

---

## 🍓 Déploiement sur Raspberry Pi

1. Flashez une carte SD avec Raspberry Pi OS 64 bits.
2. Installez les bibliothèques Qt sur le RPi.
3. Transférez l’exécutable compilé :

```bash
scp client/app pi@<ip_raspberry>:/home/pi/
```

4. Exécutez l'application sur le Raspberry :

```bash
export LD_LIBRARY_PATH=/home/pi/qt-pi-binaries64/lib:$LD_LIBRARY_PATH
./app
```

---

## 📜 Licence

Ce projet est développé à des fins éducatives et peut être modifié selon vos besoins.

---
