#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>

/*
 ===============================================================
  Grove Light Sensor v1.3 - Version fonctionnelle sans classe
 ===============================================================

   Description :
  ----------------
  Ce capteur analogique renvoie une tension (0–5 V)
  proportionnelle à la luminosité ambiante.
  Plus la lumière augmente → plus la tension augmente.

   Résultats possibles :
   - readLightRaw()        → valeur brute ADC (0 à 1023)
   - readLightMillivolts() → tension convertie en millivolts (0–5000)
   - readLightPercent()    → luminosité relative (0–100 %)

   Configuration :
   - Définir LIGHT_SENSOR_PIN sur la broche analogique utilisée (A0)
   - Aucune bibliothèque ni calcul flottant requis
 ===============================================================
 */


// Broche du capteur (modifiable selon ton câblage)
#define LIGHT_SENSOR_PIN A0

// Initialise le capteur
void LightSensor_begin();

// Lecture brute de la valeur ADC (0–1023)
uint16_t readLightRaw();

// Vérifie que le capteur est bien branché
//bool Light_Check(uint16_t newValue);

#endif
