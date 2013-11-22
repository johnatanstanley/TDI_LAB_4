// Pin to connect to your drawing
const int maxPin = 12;
int minPin=7;
// Pin for the LED
int LEDPin = 13;
int ledPrendido = 0;
int pin;

// This is how high the sensor needs to read in order
//  to trigger a touch.  You'll find this number
//  by trial and error, or you could take readings at 
//  the start of the program to dynamically calculate this.
int incomingByte = 0;	// para el byte leido

void setup(){
    Serial.begin(9600);
    // Set up the LED
    for (int i = 2; i<= 6;i++{
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
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
          aImprimir = pinStr + "\n" + readCapacitivePin(pin);
          Serial.println(aImprimir);
          if(pin<maxPin)
              pin++;
          else
              pin = minPin;
      }
      else{
        if (incomingByte >= 72 && incomingByte <= 83 ){
            if(incomingByte - 70 == ledPrendido){
                digitalWrite(ledPrendido,LOW);
            }
            else{
                digitalWrite(incomingByte-70,HIGH);
                ledPrendido = incomingByte-70;
            }  
        }
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
//          how much capacitance is on the pin
//  When you touch the pin, or whatever you have
//  attached to it, the number will get higher
//  In order for this to work now,
// The pin should have a 1+Megaohm resistor pulling
//  it up to +5v.
uint8_t readCapacitivePin(int pinToMeasure){
  // This is how you declare a variable which
  //  will hold the PORT, PIN, and DDR registers
  //  on an AVR
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  if ((pinToMeasure >= 0) && (pinToMeasure <= 7)){
    port = &PORTD;
    ddr = &DDRD;
    bitmask = 1 << pinToMeasure;
    pin = &PIND;
  }
  if ((pinToMeasure > 7) && (pinToMeasure <= 13)){
    port = &PORTB;
    ddr = &DDRB;
    bitmask = 1 << (pinToMeasure - 8);
    pin = &PINB;
  }
  if ((pinToMeasure > 13) && (pinToMeasure <= 19)){
    port = &PORTC;
    ddr = &DDRC;
    bitmask = 1 << (pinToMeasure - 13);
    pin = &PINC;
  }
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  // Make the pin an input WITHOUT the internal pull-up on
  *ddr &= ~(bitmask);
  // Now see how long the pin to get pulled up
  int cycles = 16000;
  for(int i = 0; i < cycles; i++){
    if (*pin & bitmask){
      cycles = i;
      break;
    }
  }
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



