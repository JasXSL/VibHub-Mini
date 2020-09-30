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

bool OnOffController::isPowerReleased = false;
const uint8_t OnOffController::POWER_STATE_HELD = 0;  // Waiting to release button
const uint8_t OnOffController::POWER_STATE_RELEASED = 1;  // Power button was just released
const uint8_t OnOffController::POWER_STATE_DONE = 2;  // Release has been triggered

void IRAM_ATTR OnOffController::onInterrupt(){

	detachInterrupt(Configuration::PIN_POWER_BUTTON);
	digitalWrite(Configuration::PIN_POWEROFF, LOW);

}

// Call first to keepalive
void OnOffController::setup(){

    //set wifireset pin as input
    pinMode(Configuration::PIN_POWEROFF, OUTPUT);
	digitalWrite(Configuration::PIN_POWEROFF, HIGH);
	pinMode(Configuration::PIN_POWER_BUTTON, INPUT_PULLUP);
	

}


// Called after power button has been released, only triggered once
uint8_t OnOffController::powerReleased(){

	delay(10);
	bool read = digitalRead(Configuration::PIN_POWER_BUTTON);
	Serial.println(read);

	if( isPowerReleased )
		return POWER_STATE_DONE;

	if( read == Configuration::PWR_BUTTON_UP ){
		isPowerReleased = true;
		delay(100);	// Debounce
		attachInterrupt(Configuration::PIN_POWER_BUTTON, onInterrupt, FALLING);
		return POWER_STATE_RELEASED;
	}
		
	return POWER_STATE_HELD;

}



