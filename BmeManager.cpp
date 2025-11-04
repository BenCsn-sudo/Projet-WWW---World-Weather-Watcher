#include "BmeManager.h"

// --- Fonctions bas niveau ---
uint8_t BmeManager::read8(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}

uint16_t BmeManager::read16(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)2);
    uint16_t value = (Wire.read() << 8) | Wire.read();
    return value;
}

int16_t BmeManager::readS16(uint8_t reg) {
    return (int16_t)read16(reg);
}

uint16_t BmeManager::read16_LE(uint8_t reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
}

int16_t BmeManager::readS16_LE(uint8_t reg) {
    return (int16_t)read16_LE(reg);
}

void BmeManager::write8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// --- Lecture des coefficients de calibration ---
void BmeManager::readCalibrationData() {
    dig_T1 = read16_LE(0x88);
    dig_T2 = readS16_LE(0x8A);
    dig_T3 = readS16_LE(0x8C);
    dig_P1 = read16_LE(0x8E);
    dig_P2 = readS16_LE(0x90);
    dig_P3 = readS16_LE(0x92);
    dig_P4 = readS16_LE(0x94);
    dig_P5 = readS16_LE(0x96);
    dig_P6 = readS16_LE(0x98);
    dig_P7 = readS16_LE(0x9A);
    dig_P8 = readS16_LE(0x9C);
    dig_P9 = readS16_LE(0x9E);
    dig_H1 = read8(0xA1);
    dig_H2 = readS16_LE(0xE1);
    dig_H3 = read8(0xE3);

    uint8_t e4 = read8(0xE4);
    uint8_t e5 = read8(0xE5);
    uint8_t e6 = read8(0xE6);

    dig_H4 = ((int16_t)e4 << 4) | (e5 & 0x0F);
    dig_H5 = ((int16_t)e6 << 4) | (e5 >> 4);
    dig_H6 = (int8_t)read8(0xE7);
}


// --- Initialisation du capteur ---
bool BmeManager::begin(uint8_t address) {
    _address = address;
    Wire.begin();

    uint8_t chipID = read8(0xD0);
    if (chipID != 0x60) return false; // 0x60 = ID BME280

    write8(0xE0, 0xB6); // Reset
    delay(300);

    readCalibrationData();

    // Réglages minimaux
    write8(0xF2, 0x01); // Humidité oversampling x1
    write8(0xF4, 0x25); // Temp x1, Press x1, mode forced
    while (read8(0xF3) & 0x08) delay(5);
    write8(0xF5, 0x00); // No filter

    return true;
}

// --- Lecture et calcul des mesures ---
void BmeManager::update() {
    // Déclenche une mesure en mode forcé
    write8(0xF4, 0x25);
    delay(100);

    Wire.beginTransmission(_address);
    Wire.write(0xF7);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)8);

    uint32_t press_raw = ((uint32_t)Wire.read() << 12) | ((uint32_t)Wire.read() << 4) | ((Wire.read() & 0xF0) >> 4);
    uint32_t temp_raw  = ((uint32_t)Wire.read() << 12) | ((uint32_t)Wire.read() << 4) | ((Wire.read() & 0xF0) >> 4);
    uint16_t hum_raw   = ((uint16_t)Wire.read() << 8) | Wire.read();

    // --- Température ---
    int32_t var1 = ((((int32_t)temp_raw >> 3) - ((int32_t)dig_T1 << 1)) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((temp_raw >> 4) - ((int32_t)dig_T1)) *
                 (((temp_raw >> 4) - ((int32_t)dig_T1))) >> 12)) *
                 ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    int32_t T = (t_fine * 5 + 128) >> 8;
    temp_x10 = (int16_t)(T / 10);

    // --- Pression ---
    int64_t var1p = ((int64_t)t_fine) - 128000;
    int64_t var2p = var1p * var1p * (int64_t)dig_P6;
    var2p = var2p + ((var1p * (int64_t)dig_P5) << 17);
    var2p = var2p + (((int64_t)dig_P4) << 35);
    var1p = ((var1p * var1p * (int64_t)dig_P3) >> 8) + ((var1p * (int64_t)dig_P2) << 12);
    var1p = (((((int64_t)1) << 47) + var1p)) * ((int64_t)dig_P1) >> 33;
    if (var1p == 0) return;
    int64_t p = 1048576 - press_raw;
    p = (((p << 31) - var2p) * 3125) / var1p;
    var1p = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2p = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1p + var2p) >> 8) + (((int64_t)dig_P7) << 4);
    pressPa = (uint32_t)(p >> 8);

	// --- Humidité ---
	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = ((((((int32_t)hum_raw << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * 
			(((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + 
		((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));


	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) *
	             (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));

	// --- Limitation pour rester entre 0 et 100 % ---
	if (v_x1_u32r < 0) v_x1_u32r = 0;
	else if (v_x1_u32r > 419430400) v_x1_u32r = 419430400;

	// --- Conversion finale vers %RH ×10 ---
	// (var >> 12) / 1024 * 10  ≃  (var * 10) >> 22
	hum_x10 = (uint16_t)(((int64_t)v_x1_u32r * 10) >> 22);

}

int16_t BmeManager::getTemperatureX10() const { return temp_x10; }
uint16_t BmeManager::getHumidityX10() const { return hum_x10; }
uint32_t BmeManager::getPressurePa() const { return pressPa; }
