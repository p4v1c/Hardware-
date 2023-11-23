from flask import Flask, request, jsonify
import datetime
 
app = Flask(__name__)
 
cartes_autorisees = {
 
}  
def enregistrer_log(id_carte, nom_personne):
    now = datetime.datetime.now()
    log_info = f"Carte : {id_carte}, Nom : {nom_personne}, Date : {now.strftime('%Y-%m-%d')}, Heure : {now.strftime('%H:%M:%S')} \n"
    print(log_info)
    with open('log.txt', 'a') as file : 
        file.write(log_info)
 
@app.route('/verifier_id', methods=['POST'])
def verifier_id():
    donnees = request.get_json()    
    print (donnees.get("id_carte"))  
 
    if 'id_carte' in donnees:  
        id_carte = donnees['id_carte']
 
        if id_carte in cartes_autorisees: 
            nom_personne = cartes_autorisees[id_carte]
            reponse = {'autorisation': True, 'nom' : nom_personne }
            enregistrer_log(id_carte,nom_personne)
        else:
            reponse = {'autorisation': False}
            nom_personne = "inconnu" 
            id_carte = donnees['id_carte']
            enregistrer_log(id_carte,nom_personne)
 
    else:
        reponse = 'Not found'
 
    return jsonify(reponse)
 
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0') 
