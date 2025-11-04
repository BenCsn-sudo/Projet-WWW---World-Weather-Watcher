#include "Gps.h"
#include <stdlib.h>
#include <string.h>

static Stream* gpsSerial = nullptr;

void GPS_init(Stream &serialPort) {
    gpsSerial = &serialPort;
}

// Conversion ddmm.mmmm -> dd.dddd
static float convertToDecimal(float val) {
    int deg = (int)(val / 100);
    float minutes = val - (deg * 100);
    return deg + (minutes / 60.0);
}

// Lecture simplifiée d’une trame NMEA
bool GPS_read(GPS_Data* data) {
    static char line[100];
    static uint8_t i = 0;

    while (gpsSerial->available()) {
        char c = gpsSerial->read();

        // fin de trame
        if (c == '\n') {
            line[i] = '\0';
            i = 0;

            // accepte $GNRMC / $GPRMC / $GNGGA / $GPGGA
            if (strstr(line, "RMC") || strstr(line, "GGA")) {
                char* token = strtok(line, ",");
                uint8_t field = 0;

                while (token) {
                    if (field == 3) data->latitude = convertToDecimal(atof(token));
                    else if (field == 5) data->longitude = convertToDecimal(atof(token));

                    token = strtok(NULL, ","); // ← avance vers le champ suivant
                    field++;
                }

                // Trame valide ?
                if (data->latitude != 0 && data->longitude != 0) {
                    return true;
                }
            }
        }
        else if (i < sizeof(line) - 1) {
            line[i++] = c;
            line[i] = '\0'; // sécurité
        }
    }

    return false; // aucune trame complète
}


// Pour vérifier la connection
bool GPS_isConnected() {
  unsigned long start = millis();
  while (millis() - start < 2000) {
    if (gpsSerial && gpsSerial->available()) {
      return true; // Le module envoie des données
    }
  }
  return false; // Rien reçu
}

