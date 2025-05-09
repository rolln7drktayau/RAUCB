import os
import subprocess

# Définir les répertoires de destination pour les certificats
client_cert_dir = "client/security"
server_cert_dir = "server"

# Créer les répertoires s'ils n'existent pas
os.makedirs(client_cert_dir, exist_ok=True)
os.makedirs(server_cert_dir, exist_ok=True)

# Commandes OpenSSL pour générer les certificats et les clés

# Générer la clé privée du client
subprocess.run(["openssl", "genpkey", "-algorithm", "RSA", "-out", f"{client_cert_dir}/client_key.pem", "-pkeyopt", "rsa_keygen_bits:2048"])

# Générer le certificat auto-signé du client
subprocess.run(["openssl", "req", "-new", "-x509", "-key", f"{client_cert_dir}/client_key.pem", "-out", f"{client_cert_dir}/client_cert.pem", "-days", "365", "-subj", "/CN=client"])

# Générer la clé privée du serveur
subprocess.run(["openssl", "genpkey", "-algorithm", "RSA", "-out", f"{server_cert_dir}/server_key.pem", "-pkeyopt", "rsa_keygen_bits:2048"])

# Générer le certificat auto-signé du serveur
subprocess.run(["openssl", "req", "-new", "-x509", "-key", f"{server_cert_dir}/server_key.pem", "-out", f"{server_cert_dir}/server_cert.pem", "-days", "365", "-subj", "/CN=server"])

print("Certificats et clés générés avec succès.")
