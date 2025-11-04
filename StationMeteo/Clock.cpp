#include "Clock.h"
#include <Wire.h>

static const uint8_t DS1307_ADDRESS = 0x68;	//Adresse I²C fixe du DS1307


// Conversion décimal -> BCD : 42 -> 0x42 (0100 0010)
static uint8_t decToBCD(uint8_t v) {
	// v / 10 (chiffre des dizaine) -> on décale le resultat pour mettre 
	// dans le nibble haut ->  v % 10 (chiffre des unités)-> nibble bas et on
	// utilise OU (binaire |) pour recomposer (on aurait pu mettre un +)
	return ((v / 10) << 4) | (v % 10);
}

// Conversion BCD -> décimal: 0x42 -> 42
static uint8_t bcdToDec(uint8_t b) {
	// b>>4 décale de 4 bits à droite (on récupère chiffre des dizaines)
	// b&0x0F masque les 4 bits bas (on récupère le chiffre des unités)
	// On recompose avec le +
	return ((b >> 4) * 10) + (b & 0x0F);
}


void Clock_init() {
	// Démare la pile I²C, et ne touche pas aux registres du DS1307
	Wire.begin();
}


bool Clock_isConnected() {
	// "Ping" I²C : si endTransmission() == 0 alors l'escalve 0x68 à répondu
	// donc l'horloge est connecté et on return true
	Wire.beginTransmission(DS1307_ADDRESS);
	return Wire.endTransmission() == 0;
}


bool Clock_isRunning() {
	// On vérifie bien que l'horloge est la
	if (!Clock_isConnected()) return false;

	// Pointer le registre 0x00 (secondes) pour le lire
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write((uint8_t)0x00);	//pointeur interne -> registre secondes
	Wire.endTransmission();

	// Démarre une lecture I²C d'un octet (qui sera sur le registre pointer des
	// secondes)
	Wire.requestFrom(DS1307_ADDRESS, (uint8_t)1);
	if (!Wire.available()) return false;	//Si aucun octet est lu = débranché

	// On lit l'octet des secondes (8bit) sous forme
	// CH s6 s5 s4 s3 s2 s1 s0
	// CH étant le Clock Halt (1 = oscillateur arrêté et 0 en marche) et les
	// s6...s0 sont les secondes en BCD (donc CH à ignorer lors de la reconversion)
	uint8_t secReg = Wire.read();

	// On masque le bit 7 afin de ne garder que le CH (le bit qui nous intéresse)
	// On fait un ET binaire avec le masque 0x80 donc tous les bit sauf le 7 = 0
	// secReg: CH s6 s5 s4 s3 s2 s1 s0
	// 0x80:    1  0  0  0  0  0  0  0
	// result: CH  0  0  0  0  0  0  0
	// Alors on obtient un clockHalt avec true (oscillateur arrêté) et false
	// (oscillateur en marche).
	bool clockHalt = secReg & 0x80;
	return !clockHalt;	// On inverse car on veut true pour clockisRunning
}


ClockDateTime Clock_now() {
	ClockDateTime dt{0,0,0,0,0,0,0};

	if (!Clock_isConnected()) return dt;

	// On pointe au début des registres temporels (0x00)
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.endTransmission();

	// On demande 7 octet: sec, min, hour, weekday, day, month, year
	Wire.requestFrom(DS1307_ADDRESS, (uint8_t)7);
	if (Wire.available() < 7) return dt;	// Si pas assez d'octet lu

	// Lecture brute des registres
	uint8_t secReg = Wire.read();	// bit7 = CH
	uint8_t minReg = Wire.read();
	uint8_t hourReg = Wire.read();	// bit6 = 12/24h, bit5 = AM/PM si 12h
	uint8_t weekdayReg = Wire.read();
	uint8_t dayReg = Wire.read();
	uint8_t monthReg = Wire.read();
	uint8_t yearReg = Wire.read();

	// Conversion BCD -> décimal et gestion du 12h/24h pour tout mettre en 24h
	dt.second = bcdToDec(secReg & 0x7F);	// Effacer le bit CH: 0x7F=01111111
	dt.minute = bcdToDec(minReg);

	if (hourReg & 0x40) {	// Garde bit 6: 0x40=01000000 si égale à 1 alors 12h

		// Du coup on garde les 5 derniers bit pour l'heure de 1 à 12
		uint8_t h12 = bcdToDec(hourReg & 0x1F);
		bool pm = hourReg & 0x20;	// bit 5 = PM en 12h donc 1 si PM
		if (pm) {
			if (h12 != 12) h12 += 12;	// Ex = 1pm = 13h
		} else {
			if (h12 == 12) h12 = 0;		// 12 am -> 0h
		}
		dt.hour = h12;
	} else {
		// bit6 = 0 -> mode 24h; masque 0x3F pour virer bits de controle
		dt.hour = bcdToDec(hourReg & 0x3F);
	}

	dt.weekday = bcdToDec(weekdayReg);
	dt.day = bcdToDec(dayReg);
	dt.month = bcdToDec(monthReg);
	dt.year = bcdToDec(yearReg) + 2000;	// DS1307 stocke 00..99 + 2000

	return dt;
}


void Clock_setTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss, uint8_t wd) {
	if (!Clock_isConnected()) return;	// Juste un check pour etre sur

	/*
	Ecriture "atomique" des 7 registres 0x00...0x06
	- secondes: CH (bit7) doit être à 0 pour que l'oscillateur tourne
	- heures : on force le format 24h (bit6 = 0)
	- jourSemaine
	*/

	uint8_t secBCD = decToBCD(ss) & 0x7F;	// CH = 0 (bit7 = 0)
	uint8_t minBCD = decToBCD(mm);
	uint8_t hourBCD = decToBCD(hh) & 0x3F;	// 24h (bit6 = 0)
	uint8_t wdayBCD = decToBCD(wd);		// valeur arbitraire car inutile
	uint8_t dayBCD = decToBCD(d);
	uint8_t monthBCD = decToBCD(m);

	// Si y >= 2000 alors on fait y-2000 pour avoir que l'année genre 25 l'unique
	// nombre que prend le DS1307 sinon on prend les 2 derniers chiffre comme
	// 99 de 1999
	uint8_t yearBCD = decToBCD((y >= 2000) ? (y - 2000) : (y % 100));

	// Pointeur -> 0x00 puis on enchaine avec l'écriture des 7 octets
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.write(secBCD);
	Wire.write(minBCD);
	Wire.write(hourBCD);
	Wire.write(wdayBCD);
	Wire.write(dayBCD);
	Wire.write(monthBCD);
	Wire.write(yearBCD);
	Wire.endTransmission();
}

void Clock_getTimestamp(char *buffer, size_t len) {
    ClockDateTime now = Clock_now();
    snprintf(buffer, len, "%04u-%02u-%02u %02u:%02u:%02u",
             now.year, now.month, now.day,
             now.hour, now.minute, now.second);
}
