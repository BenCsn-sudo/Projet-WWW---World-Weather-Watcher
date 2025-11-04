#ifndef BMEMANAGER_H
#define BMEMANAGER_H

#include <Arduino.h>
#include <Wire.h>

class BmeManager {
public:
    bool begin(uint8_t address = 0x77);
    void update();

    int16_t getTemperatureX10() const;
    uint16_t getHumidityX10() const;
    uint32_t getPressurePa() const;

private:
    uint8_t _address;
    int16_t temp_x10;
    uint16_t hum_x10;
    uint32_t pressPa;

    // Calibration data (extraits du capteur)
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t  dig_H1, dig_H3;
    int16_t  dig_H2, dig_H4, dig_H5, dig_H6;
    int32_t  t_fine;

    void readCalibrationData();
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    int16_t readS16(uint8_t reg);
    uint16_t read16_LE(uint8_t reg);
    int16_t readS16_LE(uint8_t reg);
    void write8(uint8_t reg, uint8_t value);
};

#endif
