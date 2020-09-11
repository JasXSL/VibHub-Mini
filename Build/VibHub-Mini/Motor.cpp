/*
Driver for one channel of a DRV8833
http://www.ti.com/lit/ds/symlink/drv8833.pdf
*/

#include "Arduino.h"
#include "Motor.h"
#include <ArduinoJson.h> 

//#define DEBUG

Motor::Motor( uint8_t pin_en ) :
    _duty(0),
	pwm(pin_en)
{
    // Force initial state
    //pwm_in1 = Pwm(pin_in1);
    //pwm_in2 = Pwm(pin_in2);
	setPWM(0);
}

void Motor::loadProgram( JsonArray stages, int repeats = 0 ){

	#ifdef DEBUG
		Serial.println();
		Serial.printf("Loading new program with #%i stages on motor.\n", stages.size());
	#endif
	
    program.completed = true;
	program.reset(repeats);


	for( auto stage : stages )
		program.addStageFromJson(stage);
	
    Serial.println("Calling program start");
	program.start();

}

void Motor::stopProgram(){
	program.completed = true;	// Makes sure we don't run loop on the program
	program.reset(0);			// Frees up some memory
}

void Motor::update(){

	if( !program.loop() )
		return;

	_duty = min(max((uint8_t)program.value,(uint8_t)0), (uint8_t)255);
	setPWM(_duty);

}

bool Motor::running(){
    return _duty > 0;
}

// fast_decay and forward are false by default
void Motor::setPWM( uint8_t duty ){

    #ifdef DEBUG
        Serial.printf("Setting duty: %i on channels %i\n", duty, pwm._channel);
    #endif
    
	_duty = duty;

    pwm.setPWM(duty);

}

