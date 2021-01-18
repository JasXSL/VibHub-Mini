/*
	
    Handles socket events from the vibhub server
    
*/

#include "ApiClient.h"
#include "Configuration.h"
#include "StatusLED.h"
#include "UserSettings.h"
#include "FirmwareUpdate.h"
#include <ArduinoJson.h>
#include "VhWifi.h"

ApiClient::ApiClient(void) :
    _socket(),
    _connected(false),
    _running(false)
{
    
}


void ApiClient::setup(){

    for( uint8_t i = 0; i < Configuration::NUM_MOTORS; ++i )
	    motors.push_back(Motor(Configuration::PIN_MOTORS[i]));

    // Attach event handlers
    // For simplicity, events are always attached regardless
    _socket.on("connect", std::bind(&ApiClient::event_connect, this, _1, _2));
    _socket.on("disconnect", std::bind(&ApiClient::event_disconnect, this, _1, _2));
    _socket.on("vib", std::bind(&ApiClient::event_vib, this, _1, _2));
    _socket.on("p", std::bind(&ApiClient::event_p, this, _1, _2));
    _socket.on("ota", std::bind(&ApiClient::event_ota, this, _1, _2));

	resetMotors();

}

bool ApiClient::motorRunning(){
    uint8_t i;
    for( i = 0; i < Configuration::NUM_MOTORS; ++i ){
        if( motors[i].running() )
            return true;
    }
    return false;
}


void ApiClient::connect(){

    Serial.printf("ApiClient::connect -> %s:%i\n", userSettings.server, userSettings.port);
    _running = true; // Start loop
    
    if( userSettings.port == 443 ){

        Serial.println("Using SSL");
        _socket.beginSSL(userSettings.server, userSettings.port, Configuration::API_URL, Configuration::ROOTCA);
    }
    else{
        _socket.begin(userSettings.server, userSettings.port, Configuration::API_URL);
    }
}

void ApiClient::disconnect(){

    Serial.println("ApiClient::disconnect");
    _connected = false; // we don't need the disconnect event
    _running = false; // Don't run loop (will cause reconnect)
    _socket.disconnect();


}

void ApiClient::event_connect( const char * payload, size_t length ){

	resetMotors();
    Serial.println("ApiClient::event_connect");
    _connected = true;
    _socket.emit("id", ("\"" + (String)userSettings.deviceid + "\"").c_str());
    statusLED.setState(StatusLED::STATE_RUNNING);

    // KC: Force test OTA
    // fwUpdate.start("0.0.1/Board_Test_32.bin", "ba23a5ca48356df4aac57df6a2634dbe");
}

void ApiClient::event_disconnect( const char * payload, size_t length ){

    if( _connected ){ // Fire only once

        Serial.println("ApiClient::event_disconnect");
        statusLED.setState(StatusLED::STATE_SOCKET_ERR);
        _connected = false;
        
        resetMotors();

    }

}


void ApiClient::event_vib( const char * payload, size_t length ){

    Serial.printf("ApiClient::event_vib: %s\n", payload);

    userSettings.resetSleepTimer();

    DynamicJsonDocument jsonBuffer(4096);
    DeserializationError error = deserializeJson(jsonBuffer, payload);
    // Payload: [{stages:[...], port:(int)port}]
    //variant.printTo(Serial);

    if( error ){
        Serial.println("Unable to read vib event");
        return;
    }
    Serial.println("Vib received with");
    serializeJson(jsonBuffer, Serial);
    Serial.println();

    DynamicJsonDocument js(2048);
    JsonArray arr = js.to<JsonArray>();

    if( jsonBuffer.is<JsonObject>() ){
        arr.add(jsonBuffer);
    }
    else if( jsonBuffer.is<JsonArray>() ){
        for( byte i=0; i<jsonBuffer.size(); ++i ){
            if( jsonBuffer[i].is<JsonObject>() )
                arr.add(jsonBuffer[i]);
        }
    }else{
        Serial.println("Error: Invalid Vib program received");
    }

    // Cycle through all programs
    for( byte i=0; i<arr.size(); ++i ){

        JsonObject j = arr[i];

        //Serial.printf("Program %i\n", i);
        //serializeJson(j, Serial);
        //Serial.println();

        bool mo[Configuration::NUM_MOTORS] = {true};
        
        if( j.containsKey("port") ){

            int port = j["port"];
            if( port > 0 ){

                for( int i = 0; i<Configuration::NUM_MOTORS; ++i )
                    mo[i] = port&(1<<i);

            }

        }

        int repeats = 0;
        if( j.containsKey("repeats") )
            repeats = j["repeats"];

        for( byte n=0; n < Configuration::NUM_MOTORS; ++n ){

            if( mo[n] )
                motors[n].loadProgram(j["stages"], repeats);

        }

    }
        

}


void ApiClient::event_p( const char * payload, size_t length ){

    uint32_t data = strtoul(payload, 0, 16);
    uint8_t vibArray[Configuration::NUM_MOTORS];
    for( uint8_t i = 0; i < Configuration::NUM_MOTORS; ++i )
        vibArray[i] = (data>>(i*8))&0xFF;

    Serial.printf("ApiClient::event_p - 0x%08x\n", data);

    userSettings.resetSleepTimer();

    int i;
    for( i = 0; i < Configuration::NUM_MOTORS; ++i )
        setFlatPWM(i, vibArray[i]);

}


void ApiClient::event_ota( const char * payload, size_t length ){
    
    Serial.printf("ApiClient::event_ota - payload: %s\n", payload);
    
    userSettings.resetSleepTimer();
    DynamicJsonDocument jsonBuffer(2048);
    deserializeJson(jsonBuffer, payload);
    JsonObject root = jsonBuffer.to<JsonObject>();

    const char* file = root["file"];
    const char* md5 = root["md5"];
    
    fwUpdate.start(file, md5);

}

void ApiClient::setFlatPWM( uint8_t motor, uint8_t value = 0 ){
    motors[motor].stopProgram();  // Stop any running program when this is received
    motors[motor].setPWM(value);
}

void ApiClient::resetMotors(){
    // Reset the motors
    for( uint8_t i = 0; i < Configuration::NUM_MOTORS; ++i )
        setFlatPWM(i);

}

void ApiClient::loop() {

    if (_running){

        _socket.loop();

        for( int i=0; i<Configuration::NUM_MOTORS; ++i )
            motors[i].update();

    }

}


ApiClient apiClient = ApiClient();
