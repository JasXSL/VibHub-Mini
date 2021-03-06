/*

    This script handles the captive portal end of the device

*/
#include "VhWifi.h"
#include "StatusLED.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include "Configuration.h"
#include "UserSettings.h"
#include <stdint.h> 

void VhWifi::connect( bool force, bool reset ){

    // Start the wifimanager
    WiFiManager wifiManager;
    _wifiManager = &wifiManager;
    
    if( reset )
        clearSettings();   
    
    // Custom CSS shared among the whole site
    String head = FPSTR(CSS_SHARED);
    head += "<script>window.onload = () => {";
        head += getCustomJSPre();
        head += FPSTR(JS_SHARED);
        head += getCustomJSPost();
    head += "};</script>";
    wifiManager.setCustomHeadElement(head.c_str());
    
    // The extra parameters to be configured
    //WiFiManagerParameter devId("deviceid", "Device ID", userSettings.deviceid, 64);
    WiFiManagerParameter serverHost("server", "Server Host", userSettings.server, 64);
    char port[6];
    itoa(userSettings.port, port, 10);
    WiFiManagerParameter serverPort("port", "Server Port", port, 6);

    char sleepTimerVal[2];
    itoa(userSettings.sleep_after_min, sleepTimerVal, 10);
    WiFiManagerParameter sleepTimer("sleep_after_min", "Turn off after minutes of inactivity", sleepTimerVal, 6);
    
    //wifiManager.addParameter(&devId);
    wifiManager.addParameter(&serverHost);
    wifiManager.addParameter(&serverPort);
    wifiManager.addParameter(&sleepTimer);

    //set config save notify callback
    wifiManager.setSaveParamsCallback(std::bind(&VhWifi::saveConfigCallback, this));
    
    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(std::bind(&VhWifi::configModeCallback, this, _1));
    
    //set minimu quality of signal so it ignores AP's under that quality
    //defaults to 8%
    wifiManager.setMinimumSignalQuality();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setConfigPortalTimeout(120);

    
    wifiManager.setAjaxCallback(std::bind(&VhWifi::onAjax, this, _1));

    String ssid = Configuration::WIFI_SSID;
    if( force ){

        if( !wifiManager.startConfigPortal(ssid.c_str()) ){

            Serial.println("VhWifi: Failed to connect and hit timeout");
            handleFatalError();

        }

    }

    // Try to connect to AP, if that doesn't work, enter config mode
    else{
        
        Serial.println("VhWiFi: WIFI Begin");
        WiFi.begin();
        int i = 0;
        while (WiFi.status() != WL_CONNECTED && i < 200) { // Wait 10 sec for the Wi-Fi to connect
            ++i;
            delay(100);
            if( (i%10) == 0 ){
                Serial.printf("%i...", i/10);
            }
        }
        Serial.println();

        connected = WiFi.status() == WL_CONNECTED;

        Serial.printf("VhWifi: Connected %i\n", connected);

        //if( !wifiManager.autoConnect(ssid.c_str()) ){
        
        if( !connected ){

            Serial.println("VhWifi: Not connected. Starting config portal");
            if( !wifiManager.startConfigPortal(ssid.c_str()) ){
                
                // Config mode failed to enter
                // Note that this happens also if you hit exit even while connected. Let the fatal error reset handler take care of it.
                Serial.println("VhWifi: Failed to connect and hit timeout");
                handleFatalError();

            }

        }

    }

    Serial.print("VhWifi: local ip: ");
    Serial.println(WiFi.localIP());
    
    Serial.println("VhWifi: connected");

    if( force ){
        ESP.restart();
        delay(1000);
    }


}

String VhWifi::onAjax( WiFiManager* wm ){

    Serial.println( wm->server->method() == HTTP_GET  ? FPSTR(S_GET) : FPSTR(S_POST) );
    String page = "";

    String task = wm->server->arg(F("t")).c_str();
    if( task == "id" || task == "ids" ){
        
        userSettings.generateDeviceId(task == "ids", true);
        page = userSettings.deviceid;

    }

    return page;

}

// Unrecoverable connection error
void VhWifi::handleFatalError(){

    statusLED.setState( StatusLED::STATE_WIFI_ERR );
    delay(5000);
    ESP.restart();

}

void VhWifi::clearSettings(){
    Serial.println("VhWifi::clearSettings(");
    if (_wifiManager){
        _wifiManager->resetSettings();
    }
}

String VhWifi::getParam(String name){
    //read parameter from server, for customhmtl input
    String value;
    if(_wifiManager->server->hasArg(name)){
        value = _wifiManager->server->arg(name);
    }
    return value;
}

//callback notifying us of the need to save config
void VhWifi::saveConfigCallback(){

    Serial.println("VhWifi: Configuration change detected, saving and rebootski");

    // Read updated parameters
    strcpy(userSettings.server, getParam("server").c_str());
    char p[5];
    strcpy(p, getParam("port").c_str());
    userSettings.port = atoi(p);
    userSettings.sleep_after_min = atoi(getParam("sleep_after_min").c_str());
    userSettings.initialized = true;
    userSettings.save();


}

//gets called when WiFiManager enters configuration mode
void VhWifi::configModeCallback( WiFiManager *myWiFiManager ){

    // Set portal LED state
    statusLED.setState(StatusLED::STATE_PORTAL);
    
    Serial.println("VhWifi: Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());

}

String VhWifi::getCustomJSPre(){
    String out;
    out += "window.DEVID='";
        out+= userSettings.deviceid;
    out += "';";
    return out;
}

String VhWifi::getCustomJSPost(){

    String out = "";
    // Anything with class VH_VERSION gets innerText set to the version
    out+= "document.querySelectorAll('.VH_VERSION').forEach(el => {";
        out+="el.innerText='";
        out+= Configuration::VH_VERSION;
        out+= "';";
    out+= "});";

    
    /*
    // Update with the DEVICE ID
    out+= "document.querySelectorAll('.VH_DEV_ID').forEach(el => {";
        out+="el.innerText='";
        out+= userSettings.deviceid;
        out+= "';";
    out+= "});";
    */
    return out;
		
}



VhWifi vhWifi = VhWifi();
