#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include <SPI.h>
#include "header.h"


int main(){
	setup();
	
	while(true){
		flylander();
	}


	Serial.end();
	return 0;
}
