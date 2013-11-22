// Pin to connect to your drawing
const int maxPin = 9;
int minPin=2;
// Pin for the LED
int LEDPin = 13;
int pin;

// This is how high the sensor needs to read in order
//  to trigger a touch.  You'll find this number
//  by trial and error, or you could take readings at 
//  the start of the program to dynamically calculate this.
int incomingByte = 0;	// para el byte leido

void setup(){
    Serial.begin(9600);
    // Set up the LED
    pinMode(LEDPin, OUTPUT);
    digitalWrite(LEDPin, LOW);
    pin = minPin;
}

void loop(){
    if (Serial.available() > 0) {
        // lee el byte de entrada:
  	incomingByte = Serial.read();
      if(incomingByte == 104){
          String aImprimir = "";
          String s = pinToChar(pin);
          String pinStr = s + "--";
          aImprimir = pinStr + "\n" + readCapacitivePin(pin) ;
          int a0 = analogRead(A0);
          int a1 = analogRead(A1);
          int a2 = analogRead(A2);
          aImprimir= aImprimir + "-" + a0 + "-" + a1+ "-" + a2;
          Serial.println(aImprimir);
          if(pin<maxPin)
              pin++;
          else
              pin = minPin;
      }
    }
    Serial.flush();
}


String pinToChar(int n){
  if(n == 0){
    return "0";
  }
  else if(n == 1){
    return "1";
  }
  else if(n == 2){
    return "2";
  }
  else if(n == 3){
    return "3";
  }
  else if(n == 4){
    return "4";
  }
  else if(n == 5){
    return "5";
  }
  else if(n == 6){
    return "6";
  }
  else if(n == 7){
    return "7";
  }
  else if(n == 8){
    return "8";
  }
    else if(n == 9){
    return "9";
  }
    else if(n == 10){
    return "10";
  }  else if(n == 11){
    return "11";
  }  else if(n == 12){
    return "12";
  }  else if(n == 13){
    return "13";
  }
}

// readCapacitivePin
//  Input: Arduino pin number
//  Output: A number, from 0 to 17 expressing
//  how much capacitance is on the pin
//  When you touch the pin, or whatever you have
//  attached to it, the number will get higher
#include "pins_arduino.h" // Arduino pre-1.0 needs this
uint8_t readCapacitivePin(int pinToMeasure) {
  // Variables used to translate from Arduino to AVR pin naming
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  port = portOutputRegister(digitalPinToPort(pinToMeasure));
  ddr = portModeRegister(digitalPinToPort(pinToMeasure));
  bitmask = digitalPinToBitMask(pinToMeasure);
  pin = portInputRegister(digitalPinToPort(pinToMeasure));
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  // Prevent the timer IRQ from disturbing our measurement
  noInterrupts();
  // Make the pin an input with the internal pull-up on
  *ddr &= ~(bitmask);
  *port |= bitmask;

  // Now see how long the pin to get pulled up. This manual unrolling of the loop
  // decreases the number of hardware cycles between each read of the pin,
  // thus increasing sensitivity.
  uint8_t cycles = 17;
       if (*pin & bitmask) { cycles =  0;}
  else if (*pin & bitmask) { cycles =  1;}
  else if (*pin & bitmask) { cycles =  2;}
  else if (*pin & bitmask) { cycles =  3;}
  else if (*pin & bitmask) { cycles =  4;}
  else if (*pin & bitmask) { cycles =  5;}
  else if (*pin & bitmask) { cycles =  6;}
  else if (*pin & bitmask) { cycles =  7;}
  else if (*pin & bitmask) { cycles =  8;}
  else if (*pin & bitmask) { cycles =  9;}
  else if (*pin & bitmask) { cycles = 10;}
  else if (*pin & bitmask) { cycles = 11;}
  else if (*pin & bitmask) { cycles = 12;}
  else if (*pin & bitmask) { cycles = 13;}
  else if (*pin & bitmask) { cycles = 14;}
  else if (*pin & bitmask) { cycles = 15;}
  else if (*pin & bitmask) { cycles = 16;}

  // End of timing-critical section
  interrupts();

  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr  |= bitmask;

  return cycles;
}



