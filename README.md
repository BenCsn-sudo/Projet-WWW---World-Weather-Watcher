# 🌍 Worldwide Weather Watcher – Station Météo Embarquée
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Projet développé dans le cadre du programme **AIVM (Agence Internationale pour la Vigilance Météorologique)**.
Objectif : déployer sur des navires de surveillance des **stations météo autonomes** capables de mesurer les paramètres influençant la formation des cyclones et autres phénomènes météorologiques extrêmes.

---

## 🚢 Contexte du projet

L’AIVM a conclu un partenariat avec plusieurs sociétés de transport maritime pour équiper leurs bateaux de **stations météo embarquées**.
Ces stations doivent être :

* simples et fiables,
* utilisables par un membre d’équipage non spécialiste,
* capables d’enregistrer et transmettre des données environnementales en mer.

Une startup spécialisée en systèmes embarqués a été chargée de concevoir le prototype de référence.

---

## ⚙️ Objectif

Développer une station météo **modulaire et autonome** fonctionnant avec un **microcontrôleur AVR ATmega328 (Arduino UNO)**, capable de :

* collecter les données de plusieurs capteurs (pression, humidité, température, luminosité, position GPS),
* sauvegarder les mesures sur carte SD,
* fonctionner selon différents **modes d’opération**,
* et fournir un **retour visuel clair via LED RGB**.

---

## 🧩 Matériel utilisé

| Élément                                        | Fonction                        | Type / Bus    |
| ---------------------------------------------- | ------------------------------- | ------------- |
| **Microcontrôleur :** Arduino UNO (ATmega328P) | Unité centrale                  | —             |
| **Capteur BME280**                             | Température, humidité, pression | I²C           |
| **Module GPS (NEO-6M)**                        | Latitude, longitude, horodatage | UART          |
| **Capteur de luminosité (LDR Grove)**          | Luminosité ambiante             | Analogique    |
| **Lecteur carte SD (Seeed Studio)**            | Sauvegarde des données          | SPI           |
| **Horloge RTC (DS3231)**                       | Référence temporelle            | I²C           |
| **LED RGB (P9813)**                            | Indication d’état système       | 2 fils (GPIO) |
| **2 boutons poussoirs**                        | Interaction utilisateur         | Numérique     |
| **EEPROM interne**                             | Stockage paramètres             | Mémoire MCU   |

### Modules futurs :

* Capteur de **température de l’eau** (analogique)
* Capteur de **force du courant marin** (I²C)
* Capteur de **vitesse du vent** (I²C)
* Capteur de **taux de particules fines** (2 fils)

---

## 🔁 Modes de fonctionnement

Le système dispose de **4 modes préprogrammés** accessibles via les **boutons poussoirs** :

| Mode              | Activation                                    | Description                                                                                                         |
| ----------------- | --------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| **Standard**      | Démarrage normal                              | Mesure et enregistrement des données toutes les 10 minutes (configurable).                                          |
| **Configuration** | Démarrage avec le bouton rouge pressé         | Paramétrage du système via l’interface série (UART). Retour automatique en mode standard après 30 min d’inactivité. |
| **Maintenance**   | Depuis standard/économique → bouton rouge 5 s | Consultation directe des données via le port série. Carte SD retirable en sécurité.                                 |
| **Économique**    | Depuis standard → bouton vert 5 s             | Réduction des acquisitions GPS et doublement de l’intervalle de mesure pour économiser la batterie.                 |

---

## 💡 Codes couleur de la LED

| Couleur / Clignotement              | État du système      |
| ----------------------------------- | -------------------- |
| Vert fixe                           | Mode standard        |
| Jaune fixe                          | Mode configuration   |
| Bleu fixe                           | Mode économique      |
| Orange fixe                         | Mode maintenance     |
| Rouge/Bleu clignotant               | Erreur RTC           |
| Rouge/Jaune clignotant              | Erreur GPS           |
| Rouge/Vert clignotant               | Erreur capteur       |
| Rouge/Vert clignotant (vert long)   | Données incohérentes |
| Rouge/Blanc clignotant              | Carte SD pleine      |
| Rouge/Blanc clignotant (blanc long) | Erreur d’écriture SD |

---

## 📋 Commandes de configuration (mode série)

| Commande                                | Description                                | Exemple              |
| --------------------------------------- | ------------------------------------------ | -------------------- |
| `LOG_INTERVAL`                          | Intervalle entre deux mesures (min)        | `LOG_INTERVAL=10`    |
| `FILE_MAX_SIZE`                         | Taille max. d’un fichier (octets)          | `FILE_MAX_SIZE=4096` |
| `TIMEOUT`                               | Délai avant abandon de lecture capteur (s) | `TIMEOUT=30`         |
| `RESET`                                 | Réinitialise les paramètres par défaut     | `RESET`              |
| `VERSION`                               | Affiche la version logicielle              | `VERSION`            |
| `LUMIN`, `TEMP_AIR`, `HYGR`, `PRESSURE` | Active/désactive un capteur                | `LUMIN=1`            |
| `CLOCK`, `DATE`, `DAY`                  | Configure l’heure et la date               | `CLOCK=14:25:00`     |

---

## 🗃️ Organisation du projet

```
📁 StationMeteo/
│
├── StationMeteo.ino          # Fichier principal (setup / loop)
├── Makefile                  # Compilation et téléversement
│
├── ModeStandard.cpp/.h       # Acquisition et enregistrement des données
├── ModeConfiguration.cpp/.h  # Interface série pour la configuration
│
├── BmeManager.cpp/.h         # Capteur BME280 (I²C)
├── Gps.cpp/.h                # Module GPS (UART)
├── Light.cpp/.h              # Capteur de luminosité (analogique)
├── SdManager.cpp/.h          # Gestion carte SD (SPI)
├── Clock.cpp/.h              # RTC (I²C)
├── Led.cpp/.h                # LED RGB (2 fils)
├── Button.cpp/.h             # Gestion boutons
└── EEPROM.cpp/.h             # Stockage paramètres utilisateur
```

---

## 🧠 Fonctionnement général

* Mesures toutes les **10 minutes** (configurable).
* Données enregistrées dans un fichier `YYMMDD_0.LOG`.
* Lorsqu’un fichier atteint la taille max, il est archivé automatiquement.
* En cas d’erreur capteur ou carte SD, le système signale l’état par LED.
* Les valeurs incohérentes sont ignorées et loguées en “NA”.

---

## 🧰 Développement

* **Langage :** C++ (Arduino)
* **Environnement :** Arduino IDE / `arduino-cli`
* **Microcontrôleur :** ATmega328P
* **Alimentation :** 5V DC
* **Fréquence CPU :** 16 MHz

---

## 📜 Licence

Ce projet est distribué sous licence **MIT**.
Libre à l’utilisation, la modification et la diffusion à des fins éducatives et expérimentales.
