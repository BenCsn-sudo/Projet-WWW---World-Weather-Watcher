#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

//Structure minimale des données GPS
typedef struct {
    float latitude;     // Latitude en degrés décimaux (ex: 43.8789)
    float longitude;    // Longitude en degrés décimaux (ex: 7.4633)
    uint8_t fix;        // 1 = Fix valide, 0 = Pas de fix (optionnel)
} GPS_Data;


// Fonctions publiques du module GPS
// Initialise le GPS avec un flux série (SoftwareSerial ou HardwareSerial)
void GPS_init(Stream &serialPort);

// Lit une trame NMEA, retourne true si trame valide (coordonnées remplies)
bool GPS_read(GPS_Data* data);

// Vérifie que tout est bien connecté
bool GPS_isConnected();

#endif
