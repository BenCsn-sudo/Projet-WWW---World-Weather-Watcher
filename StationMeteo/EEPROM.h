#ifndef EEPROM_H
#define EEPROM_H

#include <avr/io.h>	// registres matériels
#include <stdint.h>	// type fixe (uint8_t, uint16_t)

/*

L'EEPROM (Electrically Erasable Programmable Read-Only Memory) stocke les bits dans des cellules
de mémoire non volatiles.
Chaque cellule contient un transistor à grille flottante:
	- Pour écrire un 1 ou un 0 on envoie une tension élevée (10-20 V) qui piège ou libère des
	  électrons sur la grille.
	- Cette charge reste même sans alimentation (d'ou la mémoire non volatile)

Mais a chaque écriture, une partie minuscule du transistor s'use :
	- La couche isolante (oxyde) de la grille se dégrade à cause du stress électrique
	- Après trop de cycles (typiquement 100 000 à 1 000 000), la cellule ne peut plus retenir
	  correctement la charge -> le bit devient instable

Mais lire l'EEPROM ne l'abime pas du tout mais écrire ou modifier un bit use la cellule.
Donc la durée de vie est liée au nombre d'écritures par cellule et pas à l'age du composant.

Exemple:
Si on écrit une valeur toutes les secondes à la même adresse:
L'EEPROM aura une durée de vie de 27h mais si c'est une fois par minute ca monte à 69 jours



Gestion EEPROM sans bibliothèque Arduino.h ni EEPROM.h (la bibliothèque)
L'objectif est d'écrire, lire et mettre à jour des valeurs directement via les
registres matériels AVR.

On utilise juste une blibliothèque minimale indispensable qui définit les registres
matériels de l'ATmega328P (EECR, EEAR, EEDR, DDRx, PORTx, etc...)
Ce header ne charge rien en mémoire, il ne contient que des définitions de noms
et d'adresse de registres (zéro code exécutable donc 0 octets en mémoire).

Pour le projet on aura besoin de valeurs négatives enregistrés allant jusqu'à  -40 donc on
aura une partie dédié aux bit signé sur 8 octet qui nous premettra d'inscire toutes les valeurs
négatives ainsi que les valeurs inférieurs à 127. Et on utilisera des entiers plus grand qu'on
codera sur deux octets non signés avec un autre code.
*/

// Fonction écrire un octet dans l'EEPROM
void EEPROM_write_uint8(uint16_t address, uint8_t data);


// Fonction lire un octet depuis l'EEPROM
uint8_t EEPROM_read_uint8(uint16_t address);


// Fonction mis à jour de la valeur EEPROM
void EEPROM_update_uint8(uint16_t address, uint8_t data);


// Fonction écrire deux octets dans l'EEPROM
void EEPROM_write_uint16(uint16_t address, uint16_t data);


// Fonction lire deux octets dans l'EEPROM
uint16_t EEPROM_read_uint16(uint16_t address);


// Fonction mis à jour d'une valeur de deux octets dans l'EEPROM
void EEPROM_update_uint16(uint16_t address, uint16_t data);


// Fonction lire un octet signé depuis l'EEPROM
int8_t EEPROM_read_int8(uint16_t address);


// Fonction mis à jour d'une valeur signé dans l'EEPROM
void EEPROM_update_int8(uint16_t address, int8_t data);
#endif
