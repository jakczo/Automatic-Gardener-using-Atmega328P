/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <LiquidCrystal.h>
#include <Wire.h>
#include "DS3231.h"
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio
#ifndef F_CPU																// if F_CPU was not defined in Project -> Properties
#define F_CPU 16000000UL													// define it now as 16 MHz unsigned long
#endif

/*FUNCTIONS*/
void viewDefault();
void considerData();
void diodeOn();
void diodeOff();
void buttonFunction();
void viewNo1();
void viewNo2();
void moistureSensorFunction();
void waterSensorFunction();
void motorRelayFunction();

/*LCD*/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;					//lcd pinouts to atmega ports
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);									//lcd variable
/*RTC*/
DS3231 clock;																//RTC variable
RTCDateTime dt;																//date and time format variable
/*MS*/
const int atmegaPinoutMoistureSensorAnalogPin = A0;							//moisture sensor analog pin
const int atmegaPinoutMoistureSensorVcc = 8;								//moisture sensor vcc
int moistureSensorValue = 0;												//value measured by moisture sensor
float moistureSensorValuePercentage = 0;									//value measured by moisture sensor but in percentage
/*WS*/
const int atmegaPinoutWaterSensorAnalogPin = A1;							//water sensor analog pin
const int atmegaPinoutWaterSensorVcc = 7;									//water sensor vcc
int waterSensorValue = 0;													//value measured by water sensor
/*RELAY*/
const int atmegaPinoutRelayIn = 6;											//relay input signal pinout (in the water pump circuit)
/*DIODE*/
const int atmegaPinoutDiodeAnode = 9;										//diode
/*BUTTON*/
const int atmegaPinoutButton = 13;											//button
long double buttonUseTime = 0;												//time of button being used [ms]
/*OTHER*/
int presentTime = 0;														//present time [s]
int lastTest = 0;															//time of last check of the sensors [s]
const int sensorsInterval = 14;												//time interval (which is one loop) between measurements [s]
const int diodeCounter = 6;													//diode loop counter
const int thresholdNo1 = 100;												//time threshold of entering into viewNo1 [ms]
const int thresholdNo2 = 2000;												//time threshold of entering into viewNo2 [ms]
RTCDateTime lastWatering;													//date and time of the last watering
RTCDateTime lastRefilling;													//date and time of the last refilling

void setup() {
	clock.begin();
	clock.setDateTime(__DATE__, __TIME__);
	pinMode(atmegaPinoutMoistureSensorVcc, OUTPUT);							//setting digital pin No. 8 as output
	pinMode(atmegaPinoutWaterSensorVcc, OUTPUT);							//setting digital pin No. 7 as output
	pinMode(atmegaPinoutRelayIn, OUTPUT);									//setting digital pin No. 6 as output
	pinMode(atmegaPinoutDiodeAnode, OUTPUT);								//setting digital pin No. 9 as output
	pinMode(atmegaPinoutMoistureSensorAnalogPin, INPUT);					//setting analog pin No. A0 as input
	pinMode(atmegaPinoutWaterSensorAnalogPin, INPUT);						//setting analog pin No. A1 as input
	pinMode(atmegaPinoutButton, INPUT);										//setting digital pin No. 13 as input
	digitalWrite(atmegaPinoutRelayIn, HIGH);								//pin state is low as default, but it triggers relay so we set it as high to prevent that
	lcd.begin(16, 2);														//setting lcd
	lcd.clear();															//clearing (just in case)
	delay(100);
	lcd.setCursor(2, 0);
	lcd.print("Automatyczny");
	lcd.setCursor(4, 1);
	lcd.print("Ogrodnik");
	delay(4000);
	moistureSensorFunction();
	waterSensorFunction();
	considerData();
}

void loop() {
	buttonFunction();
	viewDefault();
	delay(90);
}

void viewDefault() {
	lcd.clear();
	presentTime = millis()/1000;

	if (presentTime - lastTest >= sensorsInterval) {
		moistureSensorFunction();
		waterSensorFunction();
		considerData();
		lcd.clear();
	}

	dt = clock.getDateTime();
	lcd.setCursor(1, 0);
	lcd.print(dt.day);    lcd.print(".");
	lcd.print(dt.month);  lcd.print(".");
	lcd.print(dt.year);   lcd.print("");
	lcd.setCursor(12, 0);
	lcd.print((int)moistureSensorValuePercentage);
	lcd.print("%");
	lcd.setCursor(0, 1);
	lcd.print(dt.hour);   lcd.print(":");
	lcd.print(dt.minute); lcd.print(":");
	lcd.print(dt.second); lcd.print("");
	lcd.setCursor(9, 1);
	lcd.print(clock.readTemperature());
	lcd.print((char)0xDF);
	lcd.print("C");
}

void moistureSensorFunction() {
	digitalWrite(atmegaPinoutMoistureSensorVcc, HIGH);
	delay(10);
	moistureSensorValue = 1023 - analogRead(atmegaPinoutMoistureSensorAnalogPin);
	delay(10);
	digitalWrite(atmegaPinoutMoistureSensorVcc, LOW);

	if (moistureSensorValue <= 60) {
		moistureSensorValue = 0;														//sensor calibration - measurement error of size 60 (value starts at 0)
		} else if (moistureSensorValue >= 620) {
		moistureSensorValue = 720;														//sensor calibration - measurement error of size 60 (value ends at 740)
	}
	moistureSensorValuePercentage = ((float)moistureSensorValue / 720) * 100;
}

void waterSensorFunction() {
	digitalWrite(atmegaPinoutWaterSensorVcc, HIGH);
	delay(10);
	waterSensorValue = analogRead(atmegaPinoutWaterSensorAnalogPin);
	delay(10);
	digitalWrite(atmegaPinoutWaterSensorVcc, LOW);
}

void considerData() {
	if (moistureSensorValuePercentage < 31) {											//if soil is dry
		if (waterSensorValue > 220) {													//there is water in the container
			motorRelayFunction();
			lastWatering = clock.getDateTime();
			moistureSensorFunction();
		} else {																		//there is no water in the container
			do {
				for (int i = 0; i < diodeCounter; ++i) {
					lcd.clear();
					diodeOn();
					diodeOff();
					lcd.setCursor(1, 0);
					lcd.print("ZBIORNIK PUSTY");
					lcd.setCursor(3, 1);
					lcd.print("NALEJ WODE");
					diodeOn();
					diodeOff();
				}
				waterSensorFunction();
			} while(waterSensorValue <=220);
			motorRelayFunction();
			lastWatering = clock.getDateTime();
			lastRefilling = clock.getDateTime();
			moistureSensorFunction();
		}
		} else if (moistureSensorValuePercentage >= 31 && waterSensorValue <= 220) {	//if soil is moist but there is no water in the container
		do {
			for (int i = 0; i < diodeCounter; ++i) {
				lcd.clear();
				diodeOn();
				diodeOff();
				lcd.setCursor(1, 0);
				lcd.print("ZBIORNIK PUSTY");
				lcd.setCursor(3, 1);
				lcd.print("NALEJ WODE");
				diodeOn();
				diodeOff();
			}
			lastRefilling = clock.getDateTime();
			waterSensorFunction();
		} while(waterSensorValue <=220);
	}
	presentTime = millis()/1000;
	lastTest = presentTime;
}

void motorRelayFunction() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Pompa wlaczona!");
	lcd.setCursor(0, 1);
	lcd.print("Pozostalo: ");
	digitalWrite(atmegaPinoutRelayIn, LOW);
	for (int i=0; i<6;) {
		++i;
		lcd.setCursor(11, 1);
		lcd.print(6-i);
		delay(1000);
	}
	digitalWrite(atmegaPinoutRelayIn, HIGH);
	delay(1000);
}

void diodeOn() {
	for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
		analogWrite(atmegaPinoutDiodeAnode, fadeValue);
		delay(10);
	}
}

void diodeOff() {
	for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
		analogWrite(atmegaPinoutDiodeAnode, fadeValue);
		delay(10);
	}
}
void buttonFunction() {
	while(digitalRead(atmegaPinoutButton) == HIGH) {
		delay(100);
		buttonUseTime += 100;
	}
	if(buttonUseTime >= thresholdNo2) {
		viewNo2();
		} else if(buttonUseTime>=thresholdNo1) {
		viewNo1();
	}
	buttonUseTime = 0;
}

void viewNo1() {
	lcd.clear();
	for(int i = 0; i < 6; ++i) {
		lcd.setCursor(0, 0);
		lcd.print("Wilgotnosc: ");
		waterSensorFunction();
		moistureSensorFunction();
		lcd.print((int)moistureSensorValuePercentage);
		lcd.print("%");
		lcd.setCursor(0, 1);
		if (waterSensorValue <=220) {
			lcd.print("Uzupelnij zrodlo");
			} else {
			lcd.print("Zrodlo pelne");
		}
		delay(1000);
	}
}

void viewNo2() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Ost. podlewanie");
	lcd.setCursor(0, 1);
	lcd.print(lastWatering.day);    lcd.print(".");
	lcd.print(lastWatering.month);  lcd.print(".");
	lcd.print(lastWatering.year);   lcd.print("");
	lcd.setCursor(11, 1);
	lcd.print(lastWatering.hour);   lcd.print(":");
	lcd.print(lastWatering.minute);
	delay(3500);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Ost. dolewanie");
	lcd.setCursor(0, 1);
	lcd.print(lastRefilling.day);    lcd.print(".");
	lcd.print(lastRefilling.month);  lcd.print(".");
	lcd.print(lastRefilling.year);   lcd.print("");
	lcd.setCursor(11, 1);
	lcd.print(lastRefilling.hour);   lcd.print(":");
	lcd.print(lastRefilling.minute);
	delay(3500);
	lcd.clear();
}