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

OnOffController::OnOffController( void ){}

uint32_t OnOffController::time_released = 0;

void IRAM_ATTR OnOffController::onInterrupt(){

	const uint32_t ms = millis();
	const bool pressed = digitalRead(Configuration::PIN_POWER_BUTTON) == Configuration::PWR_BUTTON_DOWN;
	
	if( !time_released ){

		if( !pressed && ms > 500 )
			time_released = ms;
		
		return;

	}
	// Turn off after 100ms debounce after releasing the button
	if( pressed && ms-time_released > 1000 ){

		detachInterrupt(digitalPinToInterrupt(Configuration::PIN_POWER_BUTTON));
		digitalWrite(Configuration::PIN_POWEROFF, LOW);

	}
	

}

void OnOffController::setup(){

    //set wifireset pin as input
    pinMode(Configuration::PIN_POWEROFF, OUTPUT);
	digitalWrite(Configuration::PIN_POWEROFF, HIGH);

    pinMode(Configuration::PIN_POWER_BUTTON, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(Configuration::PIN_POWER_BUTTON), onInterrupt, CHANGE);

}



