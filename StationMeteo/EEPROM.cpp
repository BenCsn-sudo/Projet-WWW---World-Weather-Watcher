#include "EEPROM.h"
#include <avr/io.h>

// Ecrire un octet
void EEPROM_write_uint8(uint16_t address, uint8_t data) {

	// Attendre la fin d'une éventuelle écriture précédente
	// Le bit EEPE dans EECR indique si une écriture est en cours (1 = OUI)
	while (EECR & (1 << EEPE));

	// Placer l'adresse cible et la donnée à écrire
	EEAR = address;		// EEAR = EEPROM Adress Register (10 bits)
	EEDR = data;		// EEDR = EEPROM Data Register (8 bits)

	// Déclencher l'écriture
	// La séquence imposée par le matériel est:
	//	- mettre à 1 le bit EEMPE (EEPROM Master Write Enable)
	//	- puis dans les 4 cycles d'horloge mettre a 1 EEPE (EEPROM Write Enable)
	EECR |= (1 << EEMPE);	// Armement de l'écriture
	EECR |= (1 << EEPE);	// Démarrage de l'écriture
}


// Lire un octet
uint8_t EEPROM_read_uint8(uint16_t address) {

	// Attendre que l'écriture précédente soit terminée
	while (EECR & (1 << EEPE));

	// Charger l'adresse à lire
	EEAR = address;

	// Déclencher la lecture en mettant à 1 le bit EERE (EEPROM READ ENABLE)
	EECR |= (1 << EERE);

	// Récupérer la donner lue (dans EEDR)
	return EEDR;
}


// Fonction mise à jour de la valeur
// N'écrit que si la valeur est différente -> économise la durée de vie des cellules
void EEPROM_update_uint8(uint16_t address, uint8_t data) {
	uint8_t current = EEPROM_read_uint8(address);	// Lire la valeur actuelle
	if (current != data) {
		EEPROM_write_uint8(address, data);	// Ecrit seulement si nécessaire
	}
}


// Fonction d'écriture mais sur 16 bits
// Plus efficace de refaire une fonction que de faire une grosse fonction de détection du
// nombre d'octet qui serai plus gourmande en flash car boucle.
void EEPROM_write_uint16(uint16_t address, uint16_t value) {
	uint8_t low = value & 0xFF;
	uint8_t high = (value >> 8) & 0xFF;
	EEPROM_write_uint8(address, low);
	EEPROM_write_uint8(address + 1, high);
}


// Fonction de lecture sur 16 bits
uint16_t EEPROM_read_uint16(uint16_t address) {
	uint8_t low = EEPROM_read_uint8(address);
	uint8_t high = EEPROM_read_uint8(address +1);
	return ((uint16_t)high << 8) | low;
}


// Fonction d'update sur 16 bits
void EEPROM_update_uint16(uint16_t address, uint16_t value) {

	// On découpe la valeur en deux octet
	uint8_t low = value & 0xFF;
	uint8_t high = (value >> 8) & 0xFF;

	// Lecture des deux octets déja enregistrés en EEPROM
	uint8_t currentLow = EEPROM_read_uint8(address);
	uint8_t currentHigh = EEPROM_read_uint8(address + 1);

	// Ecriture uniquement si un des deux octets diffère
	if (currentLow != low) {
		EEPROM_write_uint8(address, low);
	}
	if (currentHigh != high) {
		EEPROM_write_uint8(address + 1, high);
	}
}

// Pour faciliter la lecture on va le faire en bit signé
int8_t EEPROM_read_int8(uint16_t address) {
	return (int8_t)EEPROM_read_uint8((uint8_t*)address);
}


void EEPROM_update_int8(uint16_t address, int8_t data) {
	if (EEPROM_read_int8(address) != data) EEPROM_write_uint8((uint8_t*)address, (uint8_t)data);
}
