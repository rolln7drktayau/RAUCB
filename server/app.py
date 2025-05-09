from flask import Flask, send_from_directory, jsonify # type: ignore
import ssl

app = Flask(__name__)

@app.route("/check-update")
def check_update():
    return jsonify({
        "file": "/app",
        "sha256": "/app.sha256"
    })

@app.route("/<path:filename>")
def serve_file(filename):
    return send_from_directory(".", filename)

if __name__ == "__main__":
    # Créer le contexte SSL pour l'authentification mutuelle
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    
    # Charger le certificat et la clé privée du serveur
    context.load_cert_chain(certfile='server_cert.pem', keyfile='server_key.pem')
    
    # Configurer le mode de vérification du certificat client
    context.verify_mode = ssl.CERT_OPTIONAL  # Le serveur vérifie le certificat du client
    context.load_verify_locations(cafile="../client/security/client_cert.pem")  # Charger le certificat du client pour validation
    
    # Démarrer le serveur avec le contexte SSL configuré
    app.run(host="0.0.0.0", port=8443, ssl_context=context)
