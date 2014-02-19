int LEDshift = 0x0001;     // holds the led pattern
boolean RightLeft = false; // true for right
boolean Win = false;       // when true a win state has be achived 
boolean button = false;    // flag if the button has been pressed
byte level = 0;            // curent level holder 

void setup() {
  DDRD = DDRD | B11111000; // pins 3-7 set data direction 
  DDRB = DDRB | B00111111; //  pins 8 - 13
  digitalWrite(2,HIGH);    // pull up so the intupt can be signaled on a low transition 
}
void moveLED() {
  if (LEDshift & 0x0002) {
    RightLeft = false;
  }
  if (LEDshift & 0x0800) {
    RightLeft = true;
  }
  if (!RightLeft ) {
    LEDshift = LEDshift  << 1;
  }
  if (RightLeft) {
    LEDshift = LEDshift   >> 1;
  }
  displayLED();    
}  // end  moveLED
void displayLED() {
  PORTD =  ~((LEDshift & 0x003E) << 2); // format and place the proper bits into the registers
  PORTB =  ~((LEDshift & 0x0FC0) >> 6); // portd = low portb = hi 
}
void Button(){
  while (digitalRead(2)== LOW) {
    button = true; 
    if ((LEDshift & 0x0040)) {
      Win = true;
    } 
    else {
      Win = false;
    }
  } // end while
} // end button
void checkWin() {
  if (Win) {
    if (level < 10) {
      flashWin();
    }
    if (level >= 10) {
      BigWin();
    }
    IncreaseLevel();
  }
  if (!Win) {
    notWin(); 
  }
  resetPlay ();
} // end checkwin
void flashWin() {
  delay (100);
  LEDshift = 0xFAAA;
  for ( int i = 0 ; i < 10; i++) {
    LEDshift = ~LEDshift;
    displayLED();
    delay (100);
} 
} // end flashWin
void BigWin  () {
  flashWin();
  for (int i = 0 ; i < 4 ; i++) {
    LEDshift = 0x0040; //turn on the center LED
    displayLED();
    delay (100);
    for (int i = 0 ; i < 6 ; i++) {
      LEDshift = LEDshift  | (1<< 5 - i); // radiate from the center by a logical OR of the 1s in to the 
      LEDshift = LEDshift  | (1<< 7 + i); // LEDshift variable
      displayLED();
      delay (25);
    }
  }
  flashWin();  
} // end BigWin
void notWin() {
  level = 0;
  delay (100);
  LEDshift = 0x0001;
  for ( int i = 0 ; i < 11; i++) { 
  LEDshift = LEDshift << 1;
    LEDshift++;
    displayLED();
    delay (100);
  } 
} // end notWin
void DspLevel() {
  LEDshift = 0x0000;
  for (int i = 0 ; i <= level ; i++) {
    LEDshift = LEDshift | 0x1000; //add one to the high bits of LEDshift
    LEDshift = LEDshift >> 1 ; 
    displayLED();
    delay (50); 
  } 
  delay (500);
} // end DspLevel
void resetPlay () {
  DspLevel();
  Win = false;
  button = false;
  LEDshift = 0x0001;
  RightLeft = false;
}

void IncreaseLevel() {
  level++;
  level = level & 0x0F;// reset level when greater than 15
} 
void loop() {
detachInterrupt(0);
if (button == true) {
 checkWin();
}
moveLED();
attachInterrupt(0, Button, LOW);
delay  ( 100 - (level*6)); 
}

