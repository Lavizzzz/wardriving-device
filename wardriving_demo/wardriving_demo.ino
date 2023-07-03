#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <TinyGPS++.h>

#define BAvanti 32
#define BIndietro 33
#define Seleziona 14

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);

#define GPS_RX_PIN 27
#define GPS_TX_PIN 26

TinyGPSPlus gps;

int currentNetwork = 0;
int numNetworks = 0;

enum DisplayState {
  NETWORK_INFO,
  DISTANCE_INFO
};

DisplayState currentState = NETWORK_INFO;

bool displayNeedsUpdate = true;

void setup() {
  pinMode(BAvanti, INPUT_PULLUP);
  pinMode(BIndietro, INPUT_PULLUP);
  pinMode(Seleziona, INPUT_PULLUP);

  Serial.begin(115200);

  // Inizializza il display OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Errore nell'inizializzazione del display OLED"));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Wardriving");
  display.display();

  // Inizializza il GPS
  Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // Connettiti al WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Scansiona le reti WiFi
  numNetworks = WiFi.scanNetworks();
}

void loop() {
 
  if (displayNeedsUpdate) {
    display.clearDisplay();
    display.setCursor(0, 0);

    if (currentState == NETWORK_INFO) {
      // Mostra la rete WiFi corrente e le informazioni
      display.print("Rete ");
      display.print(currentNetwork + 1);
      display.print(" di ");
      display.println(numNetworks);
      display.println();

      display.print("SSID: ");
      display.println(WiFi.SSID(currentNetwork));
      display.print("Lat: ");
      display.println(gps.location.lat(), 6);
      display.print("Lon: ");
      display.println(gps.location.lng(), 6);
    } else if (currentState == DISTANCE_INFO) {
      // Mostra i dati della distanza
      display.print("Rete ");
      display.print(currentNetwork + 1);
      display.print(" di ");
      display.println(numNetworks);
      display.println();
      display.print("Rete: ");
      display.println(WiFi.SSID(currentNetwork));
      display.print("Distanza: ");
      display.print(DistanzaRouter());
      display.println(" m");
    }

    display.display();
    displayNeedsUpdate = false;
  }

  // Gestisci l'input dei pulsanti per scorrere le reti WiFi e cambiare lo stato
  if (digitalRead(BAvanti) == HIGH) {
    if (currentNetwork < numNetworks - 1) {
      currentNetwork++;
    } else {
      currentNetwork = 0; // Torna alla prima rete
    }
    displayNeedsUpdate = true;
    delay(200);
  }

  if (digitalRead(BIndietro) == HIGH) {
    if (currentNetwork > 0) {
      currentNetwork--;
    } else {
      currentNetwork = numNetworks - 1; // Vai all'ultima rete
    }
    displayNeedsUpdate = true;
    delay(200);
  }

  // Aggiunta condizione per cambiare lo stato solo se il pulsante è stato premuto una volta
  static bool previousSelectButtonState = LOW;
  bool currentSelectButtonState = digitalRead(Seleziona);
  if (currentSelectButtonState == LOW && previousSelectButtonState == HIGH) {
    if (currentState == NETWORK_INFO) {
      currentState = DISTANCE_INFO;
    } else {
      currentState = NETWORK_INFO;
    }
    displayNeedsUpdate = true;
  }
  previousSelectButtonState = currentSelectButtonState;

  delay(100);
}



float DistanzaRouter() {
  // Calcola la distanza dal router utilizzando il segnale RSSI
  int signalStrength = WiFi.RSSI(currentNetwork);
  float exp = (27.55 - (20 * log10(abs(signalStrength)))) / 20.0;
    return pow(10.0, -exp);
}
