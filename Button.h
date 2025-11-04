#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#define BUTTON1_PIN 7	// Bouton vert
#define BUTTON2_PIN 8	// Bouton rouge


void Button_init();
void Button_update();

bool Button1_justReleased();
bool Button2_justReleased();

unsigned long Button1_lastPressDuration();
unsigned long Button2_lastPressDuration();

#endif 
