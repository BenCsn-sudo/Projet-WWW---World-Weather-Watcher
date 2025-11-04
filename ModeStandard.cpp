#include <Arduino.h>
#include "ModeStandard.h"
#include "Light.h"
#include "ModeConfiguration.h"
#include "Led.h"
#include "SdManager.h"
#include "Clock.h"
#include "Gps.h"
#include "BmeManager.h"


//uint16_t val_Light;
uint8_t mode;
uint8_t lastmode;

void Mesure(void) {
    //  Variables locales : limitées en taille et durée de vie courte
    uint16_t val_Light;
    float latitude;
    float longitude;
    int8_t tem;
    uint8_t hum;
    uint16_t pre;

    //  La chaîne est mise en mémoire Flash (PROGMEM) → aucune RAM utilisée
    static const char msg_NA[]     PROGMEM = "N/A";

    //  Tableau sur la pile (RAM locale, libérée après la fonction)
/*
    if (LUMIN) {
	val_Light = readLightRaw();
        if (val_Light < LUMIN_LOW || val_Light > LUMIN_HIGH) {
            mode = 7;
        }
    }
*/
    GPS_Data gps;
    GPS_read(&gps);
    latitude = gps.latitude;
    longitude = gps.longitude;

//    if (latitude == 0.00 && longitude == 0.00) {
//	mode = 7;
//    }

    BmeManager bme;
    bme.update();
    tem = bme.getTemperatureX10() / 10.0;
    hum = bme.getHumidityX10() / 10.0;
    pre = bme.getPressurePa() / 100.0;
/*
   if ((tem < MIN_TEMP_AIR) || (tem > MAX_TEMP_AIR) || (pre < PRESSURE_MIN) || (pre > PRESSURE_MAX)) {
	mode = 7;
   }
*/

    val_Light = readLightRaw();
    char horodatage[24];
    Clock_getTimestamp(horodatage, sizeof(horodatage));
    char ligne[84];
    uint8_t pos = 0;

   if (!LUMIN) {
	pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, %s", horodatage, msg_NA);
   } else {
	pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, %u, ", horodatage, val_Light);
   }
   if (latitude <= 0.00 || latitude > 90.0 || longitude <= 0.00 || longitude > 180.0) {
	pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, %s, ", msg_NA, msg_NA);
   } else {
	char latStr[10];
	char lonStr[10];
	dtostrf(latitude, 2, 5, latStr);				// Car arduino uno ne gère pas les %f pour les float (on convertit en str)
	dtostrf(longitude, 2, 5, lonStr);
	pos += snprintf(ligne + pos, sizeof(ligne) - pos, "Lat=%s, Lon=%s ,", latStr, lonStr);	// Avec 5 décimal on a 1,1 mètre d'erreur
   }
   if (bme.begin(0x77)) {
	if (!TEMP_AIR) {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, ", msg_NA);
   	} else {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "Temp=%u, ", tem);
   	}
	if (!HYGR) {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, ", msg_NA);
   	} else {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "Hygr=%u, ", hum);
   	}
   	if (!PRESSURE) {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s", msg_NA);
   	} else {
		pos += snprintf(ligne + pos, sizeof(ligne) - pos, "Press=%u", pre);
	}
   } else {
	pos += snprintf(ligne + pos, sizeof(ligne) - pos, "%s, %s, %s", msg_NA, msg_NA, msg_NA);
   }
   Serial.println(ligne);
//   SD_write(ligne);
}
