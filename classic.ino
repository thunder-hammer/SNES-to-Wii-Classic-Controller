/*
 * Sample sketch which makes the Arduino to impersonate a Classic Controller.
 *
 * Use a PushButton connected to the digital pin 2 and GND. It will trigger
 * a D-Pad RIGHT button press of the impersonated Classic Controller.
 *
 * Copyright (c) 2011 Peter Brinkmann (peter.brinkmann@gmail.com)
 *
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

//#define DEBUG

#include "Wire.h" // This seems redundant, but we need to declare this
                  // dependency in the pde file or else it won't be included
                  // in the build.

#include "wiimote.h"

int DATA_CLOCK    = 6;
int DATA_LATCH    = 7;
int DATA_SERIAL  = 12;

uint8_t buttons[12]; //from SNES controller
//order is as follows
//        Clock Cycle     Button Reported
//        ===========     ===============
//        1               B
//        2               Y
//        3               Select
//        4               Start
//        5               Up on joypad
//        6               Down on joypad
//        7               Left on joypad
//        8               Right on joypad
//        9               A
//        10              X
//        11              L
//        12              R
//        13              none (always high)
//        14              none (always high)
//        15              none (always high)
//        16              none (always high)
//
//from https://gamefaqs.gamespot.com/snes/916396-super-nintendo/faqs/5395


void getControllerData();
void setupPins();
void printData();
void transferButtons();

// Classic Controller Buttons
int bdl = 0; // D-Pad Left state
int bdr = 0; // D-Pad Right state
int bdu = 0; // D-Pad Up state
int bdd = 0; // D-Pad Down state
int ba = 0; // A button state
int bb = 0; // B button state
int bx = 0; // X button state
int by = 0; // Y button state
int bl = 0; // L button state
int br = 0; // R button state
int bm = 0; // MINUS button state
int bp = 0; // PLUS button state
int bhome = 0; // HOME button state
int bzl = 0; // ZL button state
int bzr = 0; // ZR button state

/*
 * Analog Buttons.
 * They are initialized with center values from the calibration buffer.
 */
byte lx = 32;//calbuf[2]>>2;
byte ly = 32;//calbuf[5]>>2;
byte rx = 16;//calbuf[8]>>3;
byte ry = 16;//calbuf[11]>>3;


// Wiimote button data stream
byte *stream_callback(byte *buffer) {
	wiimote_write_buffer(buffer, bdl, bdr, bdu, bdd, ba, bb, bx, by, bl, br,
			bm, bp, bhome, lx, ly, rx, ry, bzl, bzr);

	return buffer;
}

void setup() {
	// Set PushButton pins as input, turning pull-up on
  //pinMode(13, OUTPUT);

	// Prepare wiimote communications
	wiimote_stream = stream_callback;
	wiimote_init();
  setupPins();
  //setupUnoJoy();
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  //Serial.println("system setup");
}

//uint8_t blink_count = 0;
//uint8_t pin13 = 0;
void loop() {
  //blink_count++;
	//if(blink_count == 100){
  //  pin13 = !pin13;
  //  if(pin13){
  //    digitalWrite(13, HIGH);
  //  }
  //  else{
  //    digitalWrite(13, LOW);
  //  }
  //  blink_count = 0;
	//}
  
  
	getControllerData();
  #ifdef DEBUG
  printData();
  #endif
  transferButtons();
  delay(5);
}


void setupPins(void){
  // Set DATA_CLOCK normally HIGH
  pinMode(DATA_CLOCK, OUTPUT);
  digitalWrite(DATA_CLOCK, HIGH);
  
  // Set DATA_LATCH normally LOW
  pinMode(DATA_LATCH, OUTPUT);
  digitalWrite(DATA_LATCH, LOW);

  // Set DATA_SERIAL normally HIGH
  pinMode(DATA_SERIAL, OUTPUT);
  digitalWrite(DATA_SERIAL, HIGH);
  pinMode(DATA_SERIAL, INPUT);  
}

void printData(){
  for(int i = 0; i < 12; i++){
    Serial.print(buttons[i]);
  }
  Serial.println();
}

void transferButtons(){
  //buttons are mapped to the button of corresponding name with the exeption of + and -
  bdl = buttons[6]; // D-Pad Left state
  bdr = buttons[7]; // D-Pad Right state
  bdu = buttons[4]; // D-Pad Up state
  bdd = buttons[5]; // D-Pad Down state
  ba = buttons[8]; // A button state
  bb = buttons[0]; // B button state
  bx = buttons[9]; // X button state
  by = buttons[1]; // Y button state
  bl = buttons[10]; // L button state
  br = buttons[11]; // R button state
  bm = buttons[2]; // MINUS button state snes Select
  bp = buttons[3]; // PLUS button state snes Start

  ry = 15;
  rx = 15;

  ly = buttons[4] ? 63: buttons[5]? 0:31;
  lx = buttons[7] ? 63: buttons[6]? 0:31;
  
}

void getControllerData(void){
    // Latch for 12us
    noInterrupts();
    digitalWrite(DATA_LATCH, HIGH);
    delayMicroseconds(12);
    digitalWrite(DATA_LATCH, LOW);
    delayMicroseconds(6);
    
    for(int i = 0; i < 16; i++){
        digitalWrite(DATA_CLOCK, LOW);
        delayMicroseconds(6);
        if(i <= 11){
            buttons[i] = !digitalRead(DATA_SERIAL);
        }
        digitalWrite(DATA_CLOCK, HIGH);
        delayMicroseconds(6);
    }    
    interrupts();
}
