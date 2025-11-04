#include "SdManager.h"

File dataFile;

bool SD_init() {
    if (!SD.begin(SD_CS)) {
        return false;
    }
    dataFile = SD.open("data.log", FILE_WRITE);
    return dataFile;
}

void SD_write(const char* text) {
    if (!dataFile) return;
    dataFile.println(text);
    dataFile.flush();
}

void SD_close() {
    if (dataFile) {
        dataFile.close();
    }
}

void SD_clear() {
    File root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        SD.remove(entry.name());
        entry.close();
    }
}

bool SD_isFull() {
  File testFile = SD.open("~TMPCHK.TXT", FILE_WRITE);
  if (!testFile) {
    // Impossible d’ouvrir → probablement carte pleine ou corrompue
    return true;
  }

  // Essaye d’écrire un tout petit truc
  testFile.println("x");
  testFile.flush();

  // Vérifie que l’écriture a bien eu lieu
  if (testFile.size() == 0) {
    testFile.close();
    SD.remove("~TMPCHK.TXT");
    return true;  // pas écrit → pleine
  }

  // Sinon, tout va bien
  testFile.close();
  SD.remove("~TMPCHK.TXT");
  return false;
}
