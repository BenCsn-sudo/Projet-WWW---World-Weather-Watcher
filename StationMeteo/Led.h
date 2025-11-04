#ifndef LED_H
#define LED_H

#include <ChainableLED.h>

/*           MODULE : Grove Chainable RGB LED v2.0 (P9813)

   Pourquoi il est difficile de piloter cette LED sans la bibliothèque :
  -----------------------------------------------------------------------
  Le module utilise un protocole propriétaire basé sur le driver P9813.
  Contrairement à une LED RGB classique (3 canaux PWM), le P9813
  requiert un enchaînement très précis de bits :

    - 32 bits de trame de départ (start frame)
    - 32 bits de données couleur (avec bits de contrôle inversés)
    - 32 bits de trame de fin (end frame)
    - timing critique : chaque bit dure ~20 µs et chaque trame
      doit être séparée d’au moins 500 µs

  Ce protocole repose sur un transfert série synchrone à deux fils (DATA, CLOCK),
  mais qui ne correspond ni à du SPI standard, ni à de l’I2C. Il faut donc
  le générer entièrement "à la main" avec un timing précis à la microseconde.

   En pratique :
  ----------------
  - Sans bibliothèque, le code doit générer 96 bits par LED,
    avec des délais constants. Le moindre écart empêche le P9813
    de valider la trame → la LED reste éteinte.

   Différence en taille mémoire :
  ---------------------------------
  - Avec bibliothèque ChainableLED : ~7,5 Ko de flash, ~350 octets de RAM
  - Sans bibliothèque (implémentation manuelle) : ~3,5 Ko de flash, ~50 octets de RAM

   Conclusion :
  La version "sans bibliothèque" est plus légère en mémoire,
  mais beaucoup moins fiable, et incompatible entre versions de Grove.
  Pour un projet embarqué robuste, il est préférable d’utiliser la bibliothèque. */

// Broches utilisées par la LED
#define LED_DATA_PIN 4
#define LED_CLOCK_PIN 5

// Fonctions de contrôle
void Led_init();
void Led_setColor(uint8_t r, uint8_t g, uint8_t b);
void Led_blinkError(uint8_t r, uint8_t g, uint8_t b, uint8_t r1, uint8_t g1, uint8_t b1);
void Led_setTempColor(uint8_t r, uint8_t g, uint8_t b, unsigned long durationMs);

#endif
