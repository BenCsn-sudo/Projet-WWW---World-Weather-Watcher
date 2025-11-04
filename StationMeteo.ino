#include <Arduino.h>
#include <Wire.h>
#include "Clock.h"
#include "Led.h"
#include "Button.h"
#include "ModeConfiguration.h"
//#include "EEPROM.h"
#include "Light.h"
#include "ModeStandard.h"
#include "SdManager.h"
#include <SoftwareSerial.h>
#include "Gps.h"
#include "BmeManager.h"

#include <SPI.h>
#include <SD.h>
#define CS_PIN 4

extern uint8_t mode;
extern uint8_t lastmode;
SoftwareSerial gpsSerial(2, 3);  // RX = 3, TX = 2
BmeManager bme;

/*
=====================================

	Mode d'emploi rapide

=====================================

- LED verte fixe : DS1307 présent + oscillateur en marche (CH=0)
- LED rouge/bleu : DS1307 absent OU oscillateur arrêté (CH =1)
- Re-détection automatique : si on rebranche l'horloge, on repasse au vert
- Mise à l'heure manuelle via Moniteur Série (9600 bauds) :
	Tape : 2025 10 06 14 30 00 (YYYY MM DD hh mm ss)
parseInt() lit les 6 entiers (il ignore les espace, tirets, etc...)

Sous linux (et surtout sur machine virtuelle) l'ouverture du port série coupe et rouvre
briévement la connexion USB ce qui provoque lors de l'exécution un doublage de la boucle
setup donc on a deux fois l'affichage de notre boucle setup. On pourrai désactiver le reset
automatique avec un condensateur mais cela demande beaucoup d'expérience et empeche le également
le téleversement automatique.

*/

void setup() {
    Serial.begin(9600);
    Wire.begin();
    Clock_init();
    Led_init();
    gpsSerial.begin(9600);
    GPS_init(gpsSerial);
    LightSensor_begin();
    ModeConfiguration_load();
    bme.begin(0x77);
    mode = 0;
//    SD.begin(SD_CS);
    Clock_setTime(2025, 11, 3, 10, 0, 0, 'MON');
}


void loop() {

    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 30000) {	//  (unsigned long)LOG_INTERVALL * 6000UL
        previousMillis = currentMillis;
        Mesure();
    }
						// --- DETECTION BRANCHEMENT ET FONCTIONNEMENT --

    if (!Clock_isConnected() && !Clock_isRunning()) {
	if (mode == 4) {
		mode = 0;
	} else {
		mode = 4;
	}
    }
//    if (!GPS_isConnected()) {
//	mode = 5;
//    }
/*
    if (!SD_init()) {
	mode = 9;
	Serial.println(mode);
    }
*/
    if (!bme.begin(0x77)) {
	mode = 6;
    }

						//     --- BOUTONS ---

    Button_update();
    if (Button1_justReleased()) {
        if (Button1_lastPressDuration() > 5000) {
            if (mode == 3) {
		mode = lastmode;
		LOG_INTERVALL = LOG_INTERVALL / 2;
            } else if (mode == 0) {
		mode = 3;
		lastmode = 0;
		LOG_INTERVALL = LOG_INTERVALL * 2;
            }
	}
    }

    if (Button2_justReleased()) {
	    if (Button2_lastPressDuration() < 5000) {
	        // Appui court : entrer/sortir mode configuration uniquement depuis le mode standard
	        if (mode == 0) {
	            lastmode = mode;
	            mode = 1;  // entrer en mode configuration
	        } else {
	            mode = lastmode;  // sortir du mode configuration
	        }
	    } else {
	        // Appui long : maintenance
	        if (mode == 0 || mode == 3) {
	            lastmode = mode;
	            mode = 2;
	            SD_close();
	        } else if (mode == 2) {
	            mode = lastmode;
	            lastmode = 2;
	            if (!SD_init()) {
			mode = 9;
	           }
	        }
	    }
     }


    if (mode == 0) Led_setColor(0, 255, 0);           // Standard -> vert
    else if (mode == 1) {                                  // Configuration -> jaune
	    Led_setColor(255, 255, 0);
	    ModeConfiguration_run();
	}
    else if (mode == 2 && lastmode != 1) Led_setColor(255, 128, 0); // Maintenance -> orange
    else if (mode == 3 && lastmode == 0) Led_setColor(0, 0, 255);   // Economique -> bleu
    else if (mode == 4) Led_blinkError(255, 0, 0, 0, 0, 255);	// Erreur RTC -> Rouge et bleu
    else if (mode == 5) Led_blinkError(255, 0, 0, 255, 255, 0);	// Erreur GPS -> Rouge et jaune
    else if (mode == 6) Led_blinkError(255, 0, 0, 0, 255, 0);	// Erreur accès capteur -> Rouge et verte
    else if (mode == 7) Led_blinkError(255, 0, 0, 0, 255, 0);	// Donnée recu incohérente -> Rouge et verte * 2
    else if (mode == 8) Led_blinkError(255, 0, 0, 255, 255, 255);	// Carte SD pleine -> Rouge et blanche
    else if (mode == 9) Led_blinkError(255, 0, 0, 255, 255, 255);	// Erreur d'accès ou d'écriture -> Rouge et blanche * 2
}
