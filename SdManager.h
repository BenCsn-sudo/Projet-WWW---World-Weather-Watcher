#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <Arduino.h>
#include <SD.h>

#define SD_CS 4
extern File dataFile;

bool SD_init();
void SD_write(const char* text);
void SD_close();
void SD_clear();
bool SD_isFull();

#endif
