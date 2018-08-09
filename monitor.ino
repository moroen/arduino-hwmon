/**
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// Modes
#define MODE_CPU 0
#define MODE_AVG_LOAD 1
#define MODE_LAST 1

int currentMode = MODE_CPU;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinToggle = 8;

const byte numChars = 128;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;

char* cpu_percent;

void recvWithEndMarker() {
	static byte ndx = 0;
	char endMarker = '\n';
	char rc;
 
 // if (Serial.available() > 0) {
	while (Serial.available() > 0 && newData == false) {
 		rc = Serial.read();

 		if (rc != endMarker) {
 			receivedChars[ndx] = rc;
 			ndx++;
 			if (ndx >= numChars) {
 				ndx = numChars - 1;
 			}
 		}
 		else {
 			receivedChars[ndx] = '\0'; // terminate the string
 			ndx = 0;
 			newData = true;
 		}
 	}
}

void showNewData() {
	const size_t bufferSize = JSON_OBJECT_SIZE(2) + 50;
	StaticJsonBuffer<300> jsonBuffer;

	if (newData == true) {
 		Serial.print("This just in: ");
		Serial.print(receivedChars);
		Serial.print("\n");
		// lcd.setCursor(0, 5);
		// lcd.print((receivedChars);
		// lcd.print(cpu_percent);
 		newData = false;

		// char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

		JsonObject& root = jsonBuffer.parseObject(receivedChars);
		if (!root.success()) {
    		lcd.clear();
			lcd.print("Json error");
    		return;
  		}	
		const char* cpu = root["cpu_percent"];
		const char* mem = root["mem"];
		const char* avgLoad = root["average_load"];

		if (currentMode == MODE_CPU) {
			lcd.setCursor(4,0);
			lcd.print(cpu);
			lcd.setCursor(4,1);
			lcd.print(mem);
		} 
		
		if (currentMode == MODE_AVG_LOAD) {
			lcd.setCursor(0,1);
			lcd.print(avgLoad);
		}
 	}
}

void setup()
{
	pinMode (pinToggle, INPUT_PULLUP);
	
	lcd.begin();
	lcd.backlight();
  
	// Initialize the serial port at a speed of 9600 baud
	Serial.begin(115200);
	setMode(MODE_CPU);

}

int buttonState;
int lastButtonState = 0;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 70; 

void setMode(int mode) {
	lcd.clear();

	if (mode==0) {
		lcd.setCursor(0,0);
		lcd.print("CPU:");
		lcd.setCursor(0,1);
		lcd.print("Mem:");
	}

	if (mode==1) {
		lcd.setCursor(0,0);
		lcd.print("Load average:");
	}
	
	currentMode = mode;
}

void loop()
{

	recvWithEndMarker();
	showNewData();

	int reading = digitalRead(pinToggle);

	if (reading != lastButtonState) {
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay) {
		if (reading != buttonState) {
			buttonState = reading;

			if (buttonState == 0) {
				if (currentMode == 1)
					setMode(0);
				else {
					++currentMode;
					setMode(currentMode);
				}
			}
		}
	}

	lastButtonState = reading;
}