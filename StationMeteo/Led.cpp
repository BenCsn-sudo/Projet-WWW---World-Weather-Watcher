#include "Led.h"

// Objet global pour contrôler une LED sur les broches 4 (DATA) et 5 (CLOCK)
static ChainableLED leds(LED_DATA_PIN, LED_CLOCK_PIN, 1);


// Initialisation de la LED (met la LED en vert par défaut)
void Led_init() {
    leds.setColorRGB(0, 0, 255, 0); // Allume la LED en vert
}

// Changement de couleur manuellement
void Led_setColor(uint8_t r, uint8_t g, uint8_t b) {
    leds.setColorRGB(0, r, g, b);
}

// Clignotement non bloquant fréquence égale
// (utilisé pour signaler une erreur)
void Led_blinkError(uint8_t r, uint8_t g, uint8_t b, uint8_t r1, uint8_t g1, uint8_t b1) {
    static bool state = false;              // État actuel du clignotement
    static unsigned long t0 = 0;            // Moment du dernier changement
    const unsigned long PERIOD = 1000;      // Durée entre deux changements (ms)

    unsigned long t = millis();
    if (t - t0 >= PERIOD) {
        state = !state;
        t0 = t;

        if (state)
            leds.setColorRGB(0, r, g, b);
        else
            leds.setColorRGB(0, r1, g1, b1);
    }
}

// Clignotement non bloquant avec fréquence doublé
// (utilisé pour signaler une erreur)
/*
void Led_blinkError2(uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    static unsigned long t0 = 0;
    static byte etat = 0;

    unsigned long now = millis();

    if (etat == 0) {
        leds.setColorRGB(0, r0, g0, b0);
        t0 = now;
        etat = 1;
    } else if (etat == 1 && now - t0 >= 1000) {
        leds.setColorRGB(0, r1, g1, b1);
        t0 = now;
        etat = 2;
  } else if (etat == 2 && now - t0 >= 2000) {
        etat = 0;
  }
}
*/
