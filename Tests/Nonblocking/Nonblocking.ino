const uint8_t PIN_POWER_BUTTON  = 33;   // LOW when pressed. Use internal pullup.

bool up = true;

void setup() {

	Serial.begin(115200);
	Serial.println();
	Serial.println("IT BEGINS");

	pinMode(PIN_POWER_BUTTON, INPUT_PULLUP);


}

void loop(){

	Serial.println(digitalRead(PIN_POWER_BUTTON));
	delay(1000);

}

