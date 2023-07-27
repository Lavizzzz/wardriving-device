#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <TinyGPS++.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* ssid = "ESP32";
const char* password = "12345678";
// Crea l'istanza del server web sulla porta 80
AsyncWebServer server(80);

#define JSON_BUFFER_SIZE 1024

#define BAvanti 33
#define BIndietro 32
#define Seleziona 14
#define Start 13

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);

#define GPS_RX 27
#define GPS_TX 26

TinyGPSPlus gps;
HardwareSerial neogps(1);

struct WiFiNetwork {
  String ssid;
  int rssi;
};
const int maxNetworks = 50;
WiFiNetwork networks[maxNetworks];
int numNetworks = 0;
int currentNetwork = 0;
bool shouldScanNetworks = false;
bool isFirstScan = false;

//variabili gestione visualizzazione pagine display
bool displayUpdate = true;
bool selectingNetwork = false;
bool displayDistanceScreen = false;
const int networksPerPage = 3;
int totalPages = 1;
int currentPage = 0;

//variabili debouncing
unsigned long lastButtonPressTime = 0;
const unsigned long buttonDebounceDelay = 200;

void setup() {

  Serial.begin(115200);
  neogps.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);


  if (!LittleFS.begin()) {
    Serial.println("Errore nell'inizializzazione di LittleFS");
  }

  pinMode(BAvanti, INPUT_PULLUP);
  pinMode(BIndietro, INPUT_PULLUP);
  pinMode(Seleziona, INPUT_PULLUP);
  pinMode(Start, INPUT_PULLUP);



  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Errore nell'inizializzazione del display OLED"));
    while (true)
      ;
  }

  isFirstScan = false;

  connectToWiFi();

  displayInit();
}

void loop() {
  gestionePulsanti();

  while (neogps.available() > 0) {
    if (gps.encode(neogps.read())) {
      if (!displayDistanceScreen) {
        // Aggiorna il display solo se è stata effettuata una scansione e isFirstScan è true
        if (displayUpdate && isFirstScan) {
          displayNetworks();
          displayUpdate = false;
        }
      } else if (isFirstScan) {
        displayDistance();
      }
    }
  }
}

void connectToWiFi() {
  scanNetworks();
  // Verifica se la rete ssid è disponibile nella lista delle reti trovate
  bool networkFound = false;
  for (int i = 0; i < numNetworks; i++) {
    if (networks[i].ssid == ssid) {
      networkFound = true;
      break;
    }
  }

  // Effettua la connessione WiFi solo se la rete ssid è disponibile
  if (networkFound) {

    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.on("/download", HTTP_GET, handleDataDownload);
    server.on("/", HTTP_GET, handleRoot);
    server.begin();

  } else {
    Serial.println("Rete WiFi non trovata");
  }
}
void handleDataDownload(AsyncWebServerRequest* request) {
  if (LittleFS.exists("/data.json")) {
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/data.json", "application/octet-stream", true);
    request->send(response);
  } else {
    request->send(404, "text/plain", "File not found");
  }
}
void handleRoot(AsyncWebServerRequest* request) {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Wardriving Data</title>";
  html += "<style>body{font-family:Arial,sans-serif;}table{border-collapse:collapse;width:100%;}th,td{border:1px solid #dddddd;text-align:left;padding:8px;}</style>";
  html += "</head><body>";
  html += "<h2>Wardriving Data</h2>";

  if (!isFirstScan) {

    html += "<p>Clicca sul link per scaricare i dati salvati in formato JSON:</p>";
    html += "<a href=\"/download\" download>Scarica dati</a>";
  } else {
    // Mostra il link per scaricare i dati
    html += "<p>Clicca sul link per scaricare i dati salvati in formato JSON:</p>";
    html += "<a href=\"/download\" download>Scarica dati</a>";

    // Mostra la tabella solo se il primo scan è stato completato
    html += "<h3>Elenco reti Wi-Fi rilevate</h3>";
    html += "<table><tr><th>SSID</th><th>RSSI</th><th>Latitudine</th><th>Longitudine</th></tr>";
    // Popola la tabella con i dati salvati
    for (int i = 0; i < numNetworks; i++) {
      html += "<tr><td>" + networks[i].ssid + "</td><td>" + String(networks[i].rssi) + "</td><td>" + String(gps.location.lat(), 6) + "</td><td>" + String(gps.location.lng(), 6) + "</td></tr>";
    }
  }

  html += "</table></body></html>";

  // Invia la risposta al client
  request->send(200, "text/html", html);
}
void salvaDati() {

  DynamicJsonDocument doc(JSON_BUFFER_SIZE);

  for (int i = 0; i < numNetworks; i++) {
    JsonObject networkObj = doc.createNestedObject();
    networkObj["ssid"] = networks[i].ssid;
    networkObj["rssi"] = networks[i].rssi;
    networkObj["latitudine"] = String(gps.location.lat(), 6);
    networkObj["longitudine"] = String(gps.location.lng(), 6);
  }

  // Apri il file in modalità scrittura
  File dataFile = LittleFS.open("/data.json", "w");
  if (!dataFile) {
    Serial.println("Errore nell'apertura del file");
    return;
  }

  // Scrivi l'oggetto JSON sul file
  serializeJson(doc, dataFile);
  dataFile.close();

  Serial.println("Dati salvati con successo");
}
void scanNetworks() {
  numNetworks = WiFi.scanNetworks();
  Serial.println("Scan completato");
  Serial.print("Trovate ");
  Serial.print(numNetworks);
  Serial.println(" reti");



  for (int i = 0; i < numNetworks; i++) {
    networks[i].ssid = WiFi.SSID(i);
    networks[i].rssi = WiFi.RSSI(i);
  }
}
void displayNetworks() {

  display.clearDisplay();
  display.setCursor(0, 0);

  int startIndex = currentPage * networksPerPage;
  int endIndex = min(startIndex + networksPerPage, numNetworks);

  int networksToShow = min(networksPerPage, numNetworks - startIndex);  // Numero di reti da visualizzare nella pagina corrente

  for (int i = 0; i < networksToShow; i++) {
    int networkIndex = startIndex + i;

    if (networkIndex == currentNetwork) {
      display.print(">");
    } else {
      display.print(" ");
    }

    display.print(networks[networkIndex].ssid);
    display.print(" (");
    display.print(networks[networkIndex].rssi);
    display.println(")");
    display.println("");
  }

  display.display();
}
bool isButtonPressed(int buttonPin) {
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && millis() - lastButtonPressTime >= buttonDebounceDelay) {
    lastButtonPressTime = millis();
    return true;
  }

  return false;
}
void gestionePulsanti() {
  totalPages = (numNetworks + networksPerPage - 1) / networksPerPage;
  bool pageChanged = false;

  if (isButtonPressed(Start)) {
    shouldScanNetworks = true;
  }

  if (shouldScanNetworks) {
    scanNetworks();
    salvaDati();
    shouldScanNetworks = false;  // Imposta nuovamente su false per evitare di riscansionare continuamente
    isFirstScan = true;          //abilita aggiornamento schermo
  }

  // Gestione del pulsante "Seleziona"
   if (isButtonPressed(Seleziona)) {
    if (!selectingNetwork) {
      // Seleziona una rete per visualizzare la distanza
      selectingNetwork = true;
      displayDistanceScreen = true;
      displayDistance();

    } else {
      // Torna alla visualizzazione delle reti disponibili
      selectingNetwork = false;
      displayUpdate = true;
      displayDistanceScreen = false;
      displayNetworks();
      displayUpdate = true;  // Aggiornamento del display
    }
  }

  // Gestione del pulsante "Avanti"
  if (isButtonPressed(BAvanti)) {
    currentNetwork++;
    if (currentNetwork >= numNetworks) {
      currentNetwork = 0;
    } 
    pageChanged = true;
    displayUpdate = true;
  }

  // Gestione del pulsante "Indietro"
  if (isButtonPressed(BIndietro)) {
    currentNetwork--;
    if (currentNetwork < 0) {
      currentNetwork = numNetworks - 1;
    } 
    pageChanged = true;
    displayUpdate = true;
  }

  if (pageChanged) {
    currentPage = currentNetwork / networksPerPage;
    displayUpdate = true;
  }

  // Aggiorna il display solo se è stata effettuata una scansione e isFirstScan è true
  if (displayUpdate && !displayDistanceScreen && !selectingNetwork && isFirstScan) {
    displayNetworks();
    displayUpdate = false;
  }
}
void displayDistance() {

  float distance = calculateDistance(networks[currentNetwork].rssi);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("SSID: ");
  display.println(networks[currentNetwork].ssid);
  display.print("Distance: ");
  display.print(distance);
  display.print(" meters");

  displayGPS();

  display.display();
}
void displayGPS() {

  if (gps.location.isValid()) {
    display.print("Lat: ");
    display.print(gps.location.lat(), 6);
    display.println("");
    display.print("Lng: ");
    display.print(gps.location.lng(), 6);
    display.println();
    display.print("Satelliti in uso: ");
    display.println(gps.satellites.value());
  } else {
    display.print("GPS non agganciato");
    display.println();
    display.print("Satelliti in uso: ");
    display.println(gps.satellites.value());
  }

  display.display();
}
void displayInit() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);


  display.setCursor(0, 0);

  // Mostra il testo
  display.println("Wardriving");
  display.println("");
  display.println("Premere Start per");
  display.println("effettuare lo scan ");
  display.println("");

  // Verifica se il WiFi è connesso e ottieni l'indirizzo IP
  if (WiFi.status() == WL_CONNECTED) {
    display.println("Per scaricare i dati:");
    display.print("http:\\ ");
    display.println(WiFi.localIP());
    display.print("sulla rete:");
    display.println(ssid);
  } else {
    display.println("Connettiti a una rete");
    display.println("WiFi per visualizzare l'IP");
  }

  display.display();
}
float calculateDistance(int rssi) {
  //stima della distanza rispetto al router
  float exp = (27.55 - (20 * log10(abs(rssi)))) / 20.0;
  return pow(10.0, -exp);
}