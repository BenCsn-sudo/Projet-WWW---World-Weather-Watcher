#include "ModeConfiguration.h"
#include "EEPROM.h"
#include "Clock.h"
#include <Arduino.h>
#include "Led.h"
#include "ModeStandard.h"

// ==========================================================
//     DÉFINITION DES VARIABLES GLOBALES
// ==========================================================
//
// Les variables ont été déclarées en “extern” dans le .h
// On les définit ici pour qu'elles aient une instance réelle.
//

bool LUMIN;
uint16_t LUMIN_LOW;
uint16_t LUMIN_HIGH;

bool TEMP_AIR;
int8_t MIN_TEMP_AIR;
int8_t MAX_TEMP_AIR;

bool HYGR;
//int8_t HYGR_MINT;
//int8_t HYGR_MAXT;

bool PRESSURE;
uint16_t PRESSURE_MIN;
uint16_t PRESSURE_MAX;

uint8_t LOG_INTERVALL;
//uint16_t FILE_MAX_SIZE;
//uint8_t TIMEOUT;

// ==========================================================
//     ADRESSES EEPROM FIXES
// ==========================================================
//
// Chaque variable a sa propre adresse en EEPROM.
// Cela permet une gestion fine et indépendante de la mémoire.
// Chaque type occupe une taille spécifique :
//   - bool     → 1 octet
//   - int8_t   → 1 octet
//   - uint16_t → 2 octets
//

#define ADDR_LUMIN        0x00
#define ADDR_LUMIN_LOW    0x02
#define ADDR_LUMIN_HIGH   0x04

#define ADDR_TEMP_AIR     0x06
#define ADDR_MIN_TEMP_AIR 0x07
#define ADDR_MAX_TEMP_AIR 0x08

#define ADDR_HYGR        0x09
//#define ADDR_HYGR_MINT    0x0A
//#define ADDR_HYGR_MAXT    0x0B

#define ADDR_PRESSURE    0x0C
#define ADDR_PRESSURE_MIN 0x0D
#define ADDR_PRESSURE_MAX 0x0E

#define ADDR_LOG_INTERVALL 0x11
//#define ADDR_FILE_MAX_SIZE 0x12
//#define ADDR_TIMEOUT 0x14

// ==========================================================
//        FONCTION : ModeConfiguration_load()
// ==========================================================
//
// Lit les valeurs sauvegardées dans l’EEPROM et les stocke dans
// les variables globales. Si tu veux définir des valeurs par défaut
// au premier démarrage, tu peux ajouter un test ici.
//

void ModeConfiguration_load() {

    // Lecture d'une signature de vérification au tout début de l’EEPROM
    // pour savoir si elle contient déjà une config valide ou pas
    const uint16_t SIGNATURE_ADDR = 0xFF;
    const uint8_t SIGNATURE_VALUE = 0x42;

    uint8_t sig = EEPROM_read_uint8(SIGNATURE_ADDR);

    // Si la signature n'existe pas c'est un premier démarrage
    if (sig != SIGNATURE_VALUE) {
        // Valeurs par défaut
        LUMIN = true;
        LUMIN_LOW = 255;
        LUMIN_HIGH = 768;
        TEMP_AIR = true;
        MIN_TEMP_AIR = -10;
        MAX_TEMP_AIR = 60;
        HYGR = true;
       // HYGR_MINT = 0;
        //HYGR_MAXT = 50;
        PRESSURE = true;
        PRESSURE_MIN = 850;
        PRESSURE_MAX = 1080;
        LOG_INTERVALL = 10;
  //      FILE_MAX_SIZE = 4096;
  //      TIMEOUT = 30;

        ModeConfiguration_save();
        EEPROM_update_uint8(SIGNATURE_ADDR, SIGNATURE_VALUE);
    } else {
        LUMIN = EEPROM_read_uint8(ADDR_LUMIN);
        LUMIN_LOW = EEPROM_read_uint16(ADDR_LUMIN_LOW);
        LUMIN_HIGH = EEPROM_read_uint16(ADDR_LUMIN_HIGH);
        TEMP_AIR = EEPROM_read_uint8(ADDR_TEMP_AIR);
        MIN_TEMP_AIR = EEPROM_read_int8(ADDR_MIN_TEMP_AIR);
        MAX_TEMP_AIR = EEPROM_read_int8(ADDR_MAX_TEMP_AIR);
        HYGR = EEPROM_read_uint8(ADDR_HYGR);
        //HYGR_MINT = EEPROM_read_int8(ADDR_HYGR_MINT);
        //HYGR_MAXT = EEPROM_read_int8(ADDR_HYGR_MAXT);
        PRESSURE = EEPROM_read_uint8(ADDR_PRESSURE);
        PRESSURE_MIN = EEPROM_read_uint16(ADDR_PRESSURE_MIN);
        PRESSURE_MAX = EEPROM_read_uint16(ADDR_PRESSURE_MAX);
        LOG_INTERVALL = EEPROM_read_uint8(ADDR_LOG_INTERVALL);
    //    FILE_MAX_SIZE = EEPROM_read_uint16(ADDR_FILE_MAX_SIZE);
    //    TIMEOUT = EEPROM_read_uint8(ADDR_TIMEOUT);
    }
}

// ==========================================================
//        FONCTION : ModeConfiguration_save()
// ==========================================================
//
// Écrit toutes les variables dans l’EEPROM (mise à jour uniquement
// si la valeur change, pour éviter d’user inutilement la mémoire).
//

void ModeConfiguration_save() {
    EEPROM_update_uint8(ADDR_LUMIN, LUMIN);
    EEPROM_update_uint16(ADDR_LUMIN_LOW, LUMIN_LOW);
    EEPROM_update_uint16(ADDR_LUMIN_HIGH, LUMIN_HIGH);

    EEPROM_update_uint8(ADDR_TEMP_AIR, TEMP_AIR);
    EEPROM_update_int8(ADDR_MIN_TEMP_AIR, MIN_TEMP_AIR);
    EEPROM_update_int8(ADDR_MAX_TEMP_AIR, MAX_TEMP_AIR);

    EEPROM_update_uint8(ADDR_HYGR, HYGR);
    //EEPROM_update_int8(ADDR_HYGR_MINT, HYGR_MINT);
    //EEPROM_update_int8(ADDR_HYGR_MAXT, HYGR_MAXT);

    EEPROM_update_uint8(ADDR_PRESSURE, PRESSURE);
    EEPROM_update_uint16(ADDR_PRESSURE_MIN, PRESSURE_MIN);
    EEPROM_update_uint16(ADDR_PRESSURE_MAX, PRESSURE_MAX);

    EEPROM_update_uint8(ADDR_LOG_INTERVALL, LOG_INTERVALL);
    //EEPROM_update_uint16(ADDR_FILE_MAX_SIZE, FILE_MAX_SIZE);
    //EEPROM_update_uint8(ADDR_TIMEOUT, TIMEOUT);
}


// ==========================================================
// Mode interactif via le moniteur série.
// ==========================================================
//
// Permet de taper des commandes du type :
//   SET LUMIN 1
//   SET LUMIN_LOW 200
//   SHOW
//   RESET
//   EXIT
//
// Chaque commande est interprétée et appliquée en direct.
//

void ModeConfiguration_run() {

    Serial.println(F("Commandes disponible :"));
    Serial.println(F("SET [parametre] [valeur]"));
    Serial.println(F("CLOCK mm,dd,yyyy"));
    Serial.println(F("DATE  MON|TUE|WED|THU|FRI|SAT|SUN"));
    Serial.println(F("RESET"));
    Serial.println(F("EXIT"));

    char buffer[32];
    unsigned long preMillis = 0;

    while (true) {
        unsigned long nowMillis = millis();

        if (nowMillis - preMillis >= 1800000) {
            break;
        }

        if (Serial.available() > 0) {
            size_t len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';
            if (len > 0 && buffer[len - 1] == '\r') buffer[len - 1] = '\0';

            // === Commande SHOW ===
		/* 
            if (strncmp(buffer, "SHOW", 4) == 0) {
                Serial.println(F("\n--- CONFIG ACTUELLE ---"));

                Serial.print(F("LUMIN: "));
                Serial.println(LUMIN);
                Serial.print(F(" LOW/HIGH: "));
                Serial.print(LUMIN_LOW);
                Serial.print(F("/"));
                Serial.println(LUMIN_HIGH);

                Serial.print(F("TEMP_AIR: "));
                Serial.println(TEMP_AIR);
                Serial.print(F(" MIN/MAX: "));
                Serial.print(MIN_TEMP_AIR);
                Serial.print(F("/"));
                Serial.println(MAX_TEMP_AIR);

                Serial.print(F("HYGR: "));
                Serial.println(HYGR);
//                Serial.print(F(" MIN/MAX: "));
//                Serial.print(HYGR_MINT);
//                Serial.print(F("/"));
//                Serial.println(HYGR_MAXT);

                Serial.print(F("PRESSURE: "));
                Serial.println(PRESSURE);
                Serial.print(F(" MIN/MAX: "));
                Serial.print(PRESSURE_MIN);
                Serial.print(F("/"));
                Serial.println(PRESSURE_MAX);

                Serial.print(F("LOG_INTERVALL: "));
                Serial.println(LOG_INTERVALL);
//                Serial.print(F("FILE_MAX_SIZE: "));
//                Serial.println(FILE_MAX_SIZE);
//                Serial.print(F("TIMEOUT: "));
//                Serial.println(TIMEOUT);

                ClockDateTime now = Clock_now();
                Serial.print(F("RTC TIME: "));
                if (now.hour < 10) Serial.print('0');

		if (now.hour < 10) Serial.print('0');
		Serial.print(now.hour);
		Serial.print(":");
		if (now.minute < 10) Serial.print('0');
		Serial.print(now.minute);
		Serial.print(":");
		if (now.second < 10) Serial.print('0');
		Serial.println(now.second);

		Serial.print(F("RTC DATE: "));
		if (now.month < 10) Serial.print('0');
		Serial.print(now.month);
		Serial.print("/");
		if (now.day < 10) Serial.print('0');
		Serial.print(now.day);
		Serial.print("/");
		Serial.println(now.year);

		const char* days[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
		if (now.weekday >= 1 && now.weekday <= 7) {
		    Serial.print(F("DAY: "));
		    Serial.println(days[now.weekday - 1]);
		}
	     }*/


	// === Commande RESET ===
	if (strncmp(buffer, "RESET", 5) == 0) {
	    LUMIN = TEMP_AIR = HYGR = PRESSURE = true;
	    LUMIN_LOW = 255;
	    LUMIN_HIGH = 768;
	    MIN_TEMP_AIR = -10;
	    MAX_TEMP_AIR = 60;
//	    HYGR_MINT = 0;
//	    HYGR_MAXT = 50;
	    PRESSURE_MIN = 850;
	    PRESSURE_MAX = 1080;
	    LOG_INTERVALL = 10;
//	    FILE_MAX_SIZE = 4096;
//	    TIMEOUT = 30;
	    ModeConfiguration_save();
	}

	// === Commande EXIT ===
	else if (strncmp(buffer, "EXIT", 4) == 0) {
	    mode = 0;
	    break;
	}

	// === Commande CLOCK hh:mm:ss ===
	else if (strncmp(buffer, "CLOCK", 5) == 0) {
	    int hh, mm, ss;
	    if (sscanf(buffer + 5, "%d:%d:%d", &hh, &mm, &ss) == 3 && hh < 24 && mm < 60 && ss < 60) {
	        ClockDateTime now = Clock_now();
	        Clock_setTime(now.year, now.month, now.day, hh, mm, ss, now.weekday);
	    } else {
	        Led_setColor(255, 0, 0);
	        delay(1000);
	        Led_setColor(255, 255, 0);
	    }
	}

	// === Commande DATE mm,dd,yyyy ===
	else if (strncmp(buffer, "DATE", 4) == 0) {
	    int mo, d, y;
	    if (sscanf(buffer + 4, "%d,%d,%d", &mo, &d, &y) == 3 && mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && y >= 2000 && y <= 2099) {
	        ClockDateTime now = Clock_now();
	        Clock_setTime(y, mo, d, now.hour, now.minute, now.second, now.weekday);
	    } else {
	        Led_setColor(255, 0, 0);
	        delay(1000);
	        Led_setColor(255, 255, 0);
	    }
	}

	// === Commande DAY MON|TUE|WED|THU|FRI|SAT|SUN ===
	else if (strncmp(buffer, "DAY", 3) == 0) {
	    const char* days[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
	    uint8_t wd = 0;
	    for (uint8_t i = 0; i < 7; i++) {
	        if (strcasecmp(buffer + 3, days[i]) == 0) {
	            wd = i + 1; // 1 = Lundi et 7 = Dimanche
	            break;
	        }
	    }

	    if (wd) {
	        ClockDateTime now = Clock_now();
	        Clock_setTime(now.year, now.month, now.day, now.hour, now.minute, now.second, wd);
	    } else {
	        Led_setColor(255, 0, 0);
	        delay(1000);
	        Led_setColor(255, 255, 0);
	    }
	}

	// === Commande SET [param] [val] ===
	else if (strncmp(buffer, "SET", 4) == 0) {
	    char *param = strtok(buffer + 4, " ");
	    char *valStr = strtok(NULL, " ");
	    int val = atoi(valStr);

	    // Luminosité
	    if (strcmp(param, "LUMIN") == 0) LUMIN = val;
	    else if (strcmp(param, "LUMIN_LOW") == 0) LUMIN_LOW = val;
	    else if (strcmp(param, "LUMIN_HIGH") == 0) LUMIN_HIGH = val;

	    // Température
	    else if (strcmp(param, "TEMP_AIR") == 0) TEMP_AIR = val;
	    else if (strcmp(param, "MIN_TEMP_AIR") == 0) MIN_TEMP_AIR = val;
	    else if (strcmp(param, "MAX_TEMP_AIR") == 0) MAX_TEMP_AIR = val;

	    // Hygrométrie
	    else if (strcmp(param, "HYGR") == 0) HYGR = val;
//	    else if (strcmp(param, "HYGR_MIN") == 0) HYGR_MINT = val;
//	    else if (strcmp(param, "HYGR_MAX") == 0) HYGR_MAXT = val;

	    // Pression
	    else if (strcmp(param, "PRESSURE") == 0) PRESSURE = val;
	    else if (strcmp(param, "PRESSURE_MIN") == 0) PRESSURE_MIN = val;
	    else if (strcmp(param, "PRESSURE_MAX") == 0) PRESSURE_MAX = val;

	    // Globale
	    else if (strcmp(param, "LOG_INTERVAL") == 0) LOG_INTERVALL = val;
//	    else if (strcmp(param, "FILE_MAX_SIZE") == 0) FILE_MAX_SIZE = val;
//	    else if (strcmp(param, "TIMEOUT") == 0) TIMEOUT = val;

	    ModeConfiguration_save(); // Sauvegarde immédiate
	}
    }
}
}
