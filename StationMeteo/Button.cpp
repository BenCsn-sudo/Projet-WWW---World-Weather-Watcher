#include "Button.h"
#include <Arduino.h>

// Config
static const unsigned long DEBOUNCE_MS = 20;	// anti rebond

// Etats bruts lus sur les pins
static bool lastRaw1 = HIGH;
static bool lastRaw2 = HIGH;

// Etats stables (après anti rebond)
static bool state1 = HIGH;
static bool state2 = HIGH;

// Timers de rebond
static unsigned long lastFlip1 = 0;
static unsigned long lastFlip2 = 0;

// Suivi des appuis
static bool button1Pressed = false;
static bool button2Pressed = false;

static unsigned long button1PressStart = 0;
static unsigned long button2PressStart = 0;

static unsigned long button1PressDuration = 0;
static unsigned long button2PressDuration = 0;

// FLags d'évènement (restent vrais qu'une fois)
static bool button1JustReleased = false;
static bool button2JustReleased = false;

void Button_init() {
	pinMode(BUTTON1_PIN, INPUT_PULLUP);
	pinMode(BUTTON2_PIN, INPUT_PULLUP);

	// Initialise avec l'état réel du matériel
	lastRaw1 = state1 = digitalRead(BUTTON1_PIN);
	lastRaw2 = state2 = digitalRead(BUTTON2_PIN);
}


void Button_update() {
	const unsigned long now = millis();

	// Lire l'état instantané
	bool raw1 = digitalRead(BUTTON1_PIN);
	bool raw2 = digitalRead(BUTTON2_PIN);

	// Détecter un changement et démarrer la fenetre anti rebond
	if (raw1 != lastRaw1) { lastRaw1 = raw1; lastFlip1 = now; }
	if (raw2 != lastRaw2) { lastRaw2 = raw2; lastFlip2 = now; }

	// Valider le changement une fois stable

	// Bouton 1
	if (state1 != lastRaw1 && (now - lastFlip1) >= DEBOUNCE_MS) {
		bool prev = state1;
		state1 = lastRaw1;

		if (prev == HIGH && state1 == LOW) {		// appui (actif bas)
			button1Pressed = true;
			button1PressStart = now;
		} else if (prev == LOW && state1 == HIGH) {	// Relachement
			button1Pressed = false;
			button1PressDuration = now - button1PressStart;	// durée de l'appui
			button1JustReleased = true;		// FLAG
		}
	}
	if (state2 != lastRaw2 && (now - lastFlip2) >= DEBOUNCE_MS) {
		bool prev = state2;
		state2 = lastRaw2;

		if (prev == HIGH && state2 == LOW) {		// appui (actif bas)
			button2Pressed = true;
			button2PressStart = now;
		} else if (prev == LOW && state2 == HIGH) {	// Relachement
			button2Pressed = false;
			button2PressDuration = now - button2PressStart;	// durée de l'appui
			button2JustReleased = true;		// FLAG
		}
	}
}

// Getters d'événement
bool Button1_justReleased() {
	bool r = button1JustReleased;
	button1JustReleased = false;	// ne sera vrai qu'une fois
	return r;
}

bool Button2_justReleased() {
	bool r = button2JustReleased;
	button2JustReleased = false;	// ne sera vrai qu'une fois
	return r;
}

// Durées de l'appui précédent
unsigned long Button1_lastPressDuration() { return button1PressDuration; }
unsigned long Button2_lastPressDuration() { return button2PressDuration; }




