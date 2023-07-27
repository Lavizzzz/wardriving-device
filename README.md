# wardriving-device

Autore: Lorenzo Lavizzari

Componenti:
-  esp32
-  GPS NEO6M mini
-  schermo oled
-  pulsanti
-  cavetti vari

Descrizione:

Il progetto è un dispositivo di controllo e monitoraggio per un sistema di wardriving che sfrutta diverse funzionalità per rilevare e visualizzare le reti Wi-Fi circostanti insieme ai dati GPS. Il dispositivo è progettato per fornire informazioni dettagliate sulle reti Wi-Fi disponibili e per registrare le coordinate GPS della posizione in cui vengono rilevate.

Caratteristiche principali del dispositivo:

Il dispositivo utilizza l'antenna wifi dell'ESP32 per scansionare le reti Wi-Fi presenti nell'area circostante. I dati di ogni rete rilevata vengono memorizzati in una struttura dati e possono essere visualizzati sul display OLED.

Dati GPS: Il dispositivo è dotato di un modulo GPS, che consente di acquisire i dati GPS relativi alla posizione geografica . Le coordinate GPS, vengono registrate insieme ai dati delle reti Wi-Fi e possono essere visualizzate sul display OLED, a seguito della selezione della rete.

Modalità di Visualizzazione: Il dispositivo offre due modalità di visualizzazione. La modalità predefinita è la visualizzazione delle reti Wi-Fi scansionate, in cui vengono mostrate le informazioni delle reti rilevate, con la possibilità di scorrere tra le pagine contenenti più reti. La seconda modalità è la visualizzazione delle coordinate GPS, in cui vengono mostrate le coordinate geografiche, il numero di satelliti agganciati e la distanza (stimata) dal router.

Download dati: I dati delle reti Wi-Fi e dei GPS possono essere scaricati, in formato JSON, tramite una richiesta HTTP al server web incorporato nel sistema. 


La funzione calculateDistance() viene utilizzata per stimare la distanza approssimativa tra il dispositivo (ESP32) e il router WiFi basandosi sulla potenza del segnale RSSI (Received Signal Strength Indicator) ricevuto dal router. Il RSSI è un valore negativo espresso in decibel (dBm) e indica la potenza del segnale ricevuto dal dispositivo dal router.

Nella funzione, la stima della distanza viene effettuata utilizzando un modello empirico noto come "propagazione del segnale RSSI" che assume un decremento esponenziale della potenza del segnale con l'aumentare della distanza tra il dispositivo e il router.

La formula utilizzata per il calcolo della distanza è:

```
Distance = 10 ^ ((MeasuredPower - RSSI) / (10 * n))

``` 
dove:

MeasuredPower è la potenza del segnale RSSI misurata (in dBm) a una distanza nota dal router ( 27,55 a un metro circa).
RSSI è la potenza del segnale RSSI misurata (in dBm) dal dispositivo verso il router.
n è un fattore di attenuazione del segnale che dipende da vari fattori ambientali, come ostacoli, interferenze e riflessioni del segnale.

- link repo: https://github.com/Lavizzzz/wardriving-device

- Licenza: GNU GENERAL PUBLIC LICENSE version 3 (GPLv3)

- Data indicativa di presentazione: Luglio 2023
