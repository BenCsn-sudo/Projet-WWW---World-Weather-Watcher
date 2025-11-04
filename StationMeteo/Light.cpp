#include "Light.h"
#include "ModeConfiguration.h"
#define SENSOR_NA 0xFFFF

// ===============================================================
//  Initialisation du capteur
// ===============================================================
void LightSensor_begin() {
  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

// ===============================================================
//  Lecture brute : retourne la valeur de l’ADC entre 0 et 1023
// ===============================================================
uint16_t readLightRaw() {
   return analogRead(LIGHT_SENSOR_PIN);
}



static uint8_t compteur = 0;
static uint16_t lastValue = 0;
// ===============================================================
//	On vérifie si le capteur est bien présent
// ===============================================================
/*
bool Light_Check(uint16_t newValue) {
    if (lastValue == 0) {
        // Première mesure → on initialise
        lastValue = newValue;
        return true;
    }

    uint16_t diff = (newValue > lastValue) ? (newValue - lastValue) : (lastValue - newValue);

    // Si la variation est anormale
    if (diff > 200) {
        compteur++;
    } else {
        // Mesure stable → on réinitialise le compteur
        compteur = 0;
    }

    // Mise à jour pour la prochaine comparaison
    lastValue = newValue;

    // Si deux anomalies d'affilée → capteur suspect
    if (compteur >= 3) {
        return false;
    }

    return true;

}
*/
