# wardriving-device

Autore: Lorenzo Lavizzari

Componenti:
-  esp32
-  GPS NEO6M mini
-  schermo oled
-  pulsanti
-  cavetti vari


Il progetto è un dispositivo di controllo e monitoraggio per un sistema di wardriving che sfrutta diverse funzionalità per rilevare e visualizzare le reti Wi-Fi circostanti insieme ai dati GPS. Il dispositivo è progettato per fornire informazioni dettagliate sulle reti Wi-Fi disponibili e per registrare le coordinate GPS della posizione in cui vengono rilevate.

Caratteristiche principali del dispositivo:

Il dispositivo utilizza l'antenna wifi dell'ESP32 per scansionare le reti Wi-Fi presenti nell'area circostante. I dati di ogni rete rilevata vengono memorizzati in una struttura dati e possono essere visualizzati sul display OLED.

Display OLED: Il dispositivo è dotato di un display OLED a matrice di punti di dimensioni 128x64 pixel. Sul display vengono visualizzati i dettagli delle reti Wi-Fi rilevate con la possibilità di scorrere tra le reti disponibili utilizzando pulsanti fisici.

Dati GPS: Il dispositivo è dotato di un modulo GPS, che consente di acquisire i dati GPS relativi alla posizione geografica . Le coordinate GPS, vengono registrate insieme ai dati delle reti Wi-Fi e possono essere visualizzate sul display OLED, a seguito della selezione della rete.

Modalità di Visualizzazione: Il dispositivo offre due modalità di visualizzazione. La modalità predefinita è la visualizzazione della reti Wi-Fi selezionata, in cui vengono mostrate le informazioni delle reti rilevate, con la possibilità di scorrere tra le pagine contenenti più reti. La seconda modalità è la visualizzazione delle coordinate GPS, in cui vengono mostrate le coordinate geografiche, il numero di satelliti agganciati e la distanza (stimata) dal router.

Monitoraggio Remoto: I dati delle reti Wi-Fi e dei GPS possono essere scaricati tramite una richiesta HTTP al server web incorporato nel sistema. Il server è gestito da una libreria AsyncWebServer che consente di scaricare un file JSON contenente i dati registrati.


- link repo: https://github.com/Lavizzzz/wardriving-device

- Licenza: GNU GENERAL PUBLIC LICENSE version 3 (GPLv3)

- Data indicativa di presentazione: Luglio 2023
