/*
	VibHub Button
    
    Handles single button interface
*/

#include <functional>
#include "OnOffController.h"
#include "StatusLED.h"
#include "Configuration.h"
#include "VhWifi.h"
#include "UserSettings.h"
#include "ApiClient.h"

uint32_t time_pressed;
OnOffController::OnOffController( void ){}

void OnOffController::setup(){

    //set wifireset pin as input
    pinMode(Configuration::PIN_POWEROFF, OUTPUT);
	digitalWrite(Configuration::PIN_POWEROFF, HIGH);

    pinMode(Configuration::PIN_POWER_BUTTON, INPUT);


}


void OnOffController::loop(){

	const bool pressed = digitalRead(Configuration::PIN_POWER_BUTTON) == Configuration::PWR_BUTTON_DOWN;
	// If not released, we're holding it because we booted
	if( !time_released ){
		if( !pressed )
			time_released = millis();
		return;
	}
	// It has been released after boot, if not pressed, do nothing
	if( !pressed )
		return;
	// Debounce
	if( millis()-time_released < 100 )
		return;

	// Turn off
	// Todo: check this
	//digitalWrite(Configuration::PIN_POWEROFF, LOW);

}


OnOffController onOffController = OnOffController();
