/* Libraries

External libraries needed:
Use https://github.com/JasXSL/WiFiManager - It has a custom ajax endpoint
https://arduinojson.org/ - Installable via Arduino IDE.
Ticker-esp32 ??? Don't know where you got this
Kadah's fork of arduinoWebSockets: https://github.com/Kadah/arduinoWebSockets
and socket.io-client: https://github.com/Kadah/socket.io-client
*/ 

#include <Arduino.h>

#include "VhWifi.h"

#include <WiFi.h>
#include <HTTPClient.h> 

#include "Configuration.h"
#include "UserSettings.h"
#include "ConfigButton.h"
#include "ApiClient.h"
#include "StatusLED.h"
#include "Pwm.h"
#include "OnOffController.h"

bool awake = false;

void onPowerReleased(){
    Serial.println("\nStarting...");

    Serial.printf("Board Version: %s\n", Configuration::VH_HWVERSION);
    Serial.printf("Firmware Version: %s\n", Configuration::VH_VERSION);

    configButton.setup();
    apiClient.setup();
   
    // Reset config and wifi if config button is held on boot
	bool reset = false;
    if( configButton.isPressed() ){

        Serial.println("Resetting everything");        
        reset = true;

    }
    

    userSettings.load(reset);
    Serial.println("Connecting to wifi");
    Serial.printf("Reset: %i\n", reset || !userSettings.initialized);
    vhWifi.connect(reset || !userSettings.initialized, reset);
    Serial.println("Connected");
    
	
    // Set socket loading state
    statusLED.setState(StatusLED::STATE_SOCKET_ERR);

    // Set system time
    setClock();

    //Connect to server
    if( vhWifi.connected ){
        apiClient.connect();
        apiClient.loop();
        yield();
    }
}

// Program begins
void setup() {

    
    OnOffController::setup();

    Serial.begin(115200);
    statusLED.initialize();
    // Set LED state
    Serial.println("Setting state BOOT");
    statusLED.setState(StatusLED::STATE_BOOT);


}


// Main program lööp
void loop() {

    const uint8_t resetState = OnOffController::powerReleased();
    if( resetState == OnOffController::POWER_STATE_HELD ){
        
        return;

    }
    
    if( resetState == OnOffController::POWER_STATE_RELEASED ){

        Serial.println("INIT");
        onPowerReleased();
        return;

    }

    apiClient.loop();
    configButton.loop();
    userSettings.loop();
    //ArduinoOTA.handle();

}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
}
