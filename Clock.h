#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>

/*
===================================
	Module RTC DS1307
===================================

Ici on ne va pas passer par la bibliothèque RTC on va faire du "bare metal" donc faire
nos conversions a l'horloge par nous même c equi permet d'utiliser suelement quelques
centaines d'octets contrairement à la librairie qui utilise entre 2 et 6 kilo-octets.
Mais le vrai intérêt se trouve surtout dans la transparence des protocoles ce qui permet
de comprendre pleinement l'horloge et savoir communiquer avec elle.
Le DS1307 expose 7 registres principaux, tous en BCD: Binary Coded Decimal qui veut
dire que chaque chiffre décimal est codé sur 4 bits (un nibble), le DS1307 stocke
l'heure en BCD dans ses refistres internes. Donc quand on écrit l'heure dans le DS1307
on convertit nos nombres décimaux en BCD. Et quand on lit l'heure du DS1307 on convertit
le BCD en décimal. Voici les 7 registres accessible par I²C,(adresse 0x68)
	0x00: secondes (bit7 = CH = Clock Halt -> 1 = oscilateur arrêté, 0 = en marche)
	0x01: minutes
	0x02: heures (bit6 = 12/24h, bit5 = AM/PM si 12h)
	0x03: jour de la semaine (1...7)
	0x04: jour du mois (1...31)
	0x05: mois (1...12)
	0x06: année (00...99) -> on ajoute 2000 coté code
On utilise BCD pour un héritage des RTC simple cela simplifie l'affichage et la lecture
mais également la logique interne. On a une interface procédurale et minimaliste.
*/

// Conteneur de date et heure en décimal (pas BCD)
struct ClockDateTime {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t weekday;
};

// Initialise I²C (Wire). Ne règle pas l'heure on conserve celle tenue par la pile
void Clock_init();

// Est ce que le DS1307 répond à l'adresse 0x68 sur le bus I²C ?
bool Clock_isConnected();

// Est ce que l'oscillateur interne du DS1307 tourne (bit CH = 0) ?
bool Clock_isRunning();

// Lit l'heure courante (convertie en décimal)
ClockDateTime Clock_now();

// Règle l'heure du DS1307 (en forcant CH = 0 pour démarrer l'oscillateur)
void Clock_setTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss, uint8_t wd);

void Clock_getTimestamp(char *buffer, size_t len);

#endif
