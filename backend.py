from flask import Flask, request, jsonify
 
app = Flask(__name__)
 
cartes_autorisees = {
    " 33 C4 21 99" : "Mohamed"
}  
 
 
@app.route('/verifier_id', methods=['POST'])
def verifier_id():
    donnees = request.get_json()    
    print (donnees.get("id_carte"))  
 
    if 'id_carte' in donnees:  
        id_carte = donnees['id_carte']
 
        if id_carte in cartes_autorisees: 
            nom_personne = cartes_autorisees[id_carte]
            reponse = {'autorisation': True, 'nom' : nom_personne }
        else:
            reponse = {'autorisation': False}
 
    else:
        reponse = 'Not found'
 
    return jsonify(reponse)
 
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
