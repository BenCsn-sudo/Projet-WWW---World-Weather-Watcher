#ifndef MODECONFIGURATION_H
#define MODECONFIGURATION_H

#include <Arduino.h>

// ==============================
// Variables globales
// ==============================

// Luminosité
extern bool LUMIN;
extern uint16_t LUMIN_LOW, LUMIN_HIGH;

// Température
extern bool TEMP_AIR;
extern int8_t MIN_TEMP_AIR, MAX_TEMP_AIR;

// Hygrométrie
extern bool HYGR;
//extern int8_t HYGR_MINT, HYGR_MAXT;

// Pression
extern bool PRESSURE;
extern uint16_t PRESSURE_MIN, PRESSURE_MAX;

// Paramètres globaux
extern uint8_t LOG_INTERVALL;
//extern uint16_t FILE_MAX_SIZE;
extern uint8_t TIMEOUT;

// ==============================
// Fonctions publiques
// ==============================

// Charge la configuration depuis l'EEPROM
void ModeConfiguration_load();

// Sauvegarde la configuration actuelle dans l'EEPROM
void ModeConfiguration_save();

// Lance le mode configuration via le port série
void ModeConfiguration_run();

#endif
