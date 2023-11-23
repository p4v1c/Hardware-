#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN D2
#define SS_PIN D0
#define WIFI_SSID "Ss"
#define WIFI_PASSWORD "12345678"
#define SERVER_IP "192.168.137.145"
#define SERVER_PORT 5000
#define ENDPOINT "/verifier_id"

MFRC522 mfrc522;                     // Déclaration du module MFRC522 sans les broches définies
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Définir l'adresse I2C et la taille de l'écran

void initializeLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}


void initMFRC522() {
  mfrc522 = MFRC522(SS_PIN, RST_PIN);  // Initialisation du module MFRC522 avec les broches définies
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void setup() {
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  // Appel de la fonction pour initialiser l'écran LCD
  initMFRC522();  // Appel de la fonction pour initialiser le module MFRC522
  initializeLCD();
}

void loop() {
  lcd.setCursor(0, 0);

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  log_d("Badge found");

  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    UID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  UID.toUpperCase();

  HTTPClient http;
  String url = "http://" + String(SERVER_IP) + ":" + String(SERVER_PORT) + String(ENDPOINT);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"id_carte\": \"" + UID + "\"}";

  int httpCode = http.POST(json);

  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println(response);

    lcd.clear();

    if (response.indexOf("true") != -1) {
      int debut_nom = response.indexOf("nom") + 6;                   // Trouver le début du nom dans la réponse JSON
      int fin_nom = response.indexOf('\n}', debut_nom);              // Trouver la fin du nom dans la réponse JSON
      String nom_personne = response.substring(debut_nom +1, fin_nom-2);  // Extraire le nom de la réponse JSON

      lcd.setCursor(0, 0);
      lcd.print("Badge OK");
      digitalWrite(D1, HIGH);
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bonjour");
      lcd.setCursor(0, 1);
      lcd.print(nom_personne);  // Afficher le nom de la personne
      delay(2000);
      lcd.clear();
      digitalWrite(D1, LOW);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Badge Incorrect");
      //    initMFRC522();
      digitalWrite(D3, HIGH);
      delay(2000);
      lcd.clear();
      digitalWrite(D3, LOW);
    }
  }
  http.end();
  delay(1000);
}