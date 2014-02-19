#include <SPI.h>
#include "cube.h"
#include <GD.h>

int logoCount = 0;  
int selfPlayCount = 300;

int movetonecount = 0;
boolean moveToneflag = true;

long cubeMove[18];
boolean RightLeft;
boolean Win = false;
boolean button = false;
int level = 0;

long initPattern = 0x0000001f;

void setup() {
  pinMode(2,INPUT);
  digitalWrite(2,HIGH); 
  GD.begin();
  GD.copy(PALETTE4A, cube_sprpal, sizeof(cube_sprpal));
  GD.copy(RAM_SPRIMG, cube_sprimg, sizeof(cube_sprimg));
  GD.ascii(); 
  StackIt();
}  // end setup

void RowShift() {
  if (cubeMove[16-level] & 0x00000001){
    RightLeft = false;
  }
  if (cubeMove[16-level] & 0x00800000){
    RightLeft = true;
  }
  if (!RightLeft ){
    cubeMove[16-level] = cubeMove[16-level]  << 1;
  }
  if (RightLeft){
    cubeMove[16-level] = cubeMove[16-level]  >> 1;
  }
} // end row shift 

void displaySprites() { 
  int spriteNum = 0; // start sprite count at 0
  for (int y = 0 ; y < 18 ; y ++ ) { // loop though the array for y positon  
    for (int x = 0 ; x < 24 ; x ++) {  //  loop though the variable for x positon
      if ((cubeMove[y] >> x) & 0x00000001) { // check current variable position for a 1 
        GD.sprite(spriteNum, (x* 16)+7, (y*16)+7 ,0, 8 , 0);
        spriteNum++;
      } // end if 
    } // end for loop x
  } // end for loop y 
} // end  displaySprites 
void buttonInterrupt  () {
  selfPlayCount = 0;  
  while ( digitalRead(2)== LOW) {
   WinState();
  }
} //end  buttonInterrupt 

void WinState() {
  button = true;
  if ((cubeMove[16-level] & cubeMove[17-level])) {
    Win = true;
  } 
  else {
    Win = false;
  }
} // end WinState

void checkWin() {
  if (Win) {
    // check prior level and set curent level to any misses and coppy to next level
    cubeMove[15-level] = cubeMove[16-level] = cubeMove[16-level] & cubeMove[17-level];
    IncreaseLevel();
  }
  if (!Win) { 
    LossTone(); 
    resetPlay ();
  }
  button = false;
} // end checkWin

void IncreaseLevel() {
  level ++ ;
  if (level >= 17) {
    //display winning pattern and reset play
    displaySprites(); 
    WinTone();
    delay (200);
    resetPlay();
  }
} // end IncreaseLevel

void resetPlay () {  
  for (int i = 0 ; i < 17 ; i ++) {
  cubeMove [i] = 0x00000000;
  }
  cubeMove[16] = initPattern;
  cubeMove[17] = 0x00ffffff;
  for (int i = 0 ; i < 256 ; i ++) {
  GD.sprite(i,450,450,0,0,0);
  }
  level = 0;
} // end resetPlay

void loop() {
  detachInterrupt(0);
  if (button) {
    checkWin();
  } 
  RowShift(); 
  displaySprites();
  attachInterrupt(0, buttonInterrupt   , LOW);
  selfPlay(); 
  delay (120 - ( level * 6));
  moveTone();
} // end loop 

void moveTone() {
  if (moveToneflag) {
    if (movetonecount >= 2) {
    GD.voice(0, 0, movetonecount*1000,127,127);
    }
    if (movetonecount == 5){
    GD.voice(0, 0, 0,0,0);
    movetonecount = 0 ;
    }
    movetonecount++; 
  } // end if moveToneflag
} // end moveTone

void WinTone() {
  for (int t =0 ; t < 2 ; t ++) {
    for(int i = 3 ; i < 5 ; i++) {
      GD.voice(0, 0, i*1000,     254 ,    254);
      delay (150); 
    }  
    for(int i = 5 ; i > 1 ; i--) {
      GD.voice(0, 0, i*1000,254 ,254);;
      delay (150);  
    }
    GD.voice(0, 0, 0,0,0); 
  } // end for loop that plays the tone twice
} // end WinTone 

void LossTone() {
  for(int i = 5 ; i > 1 ; i--) {
  GD.voice(0, 0, i*500,     254 ,    254);
  delay (150);
  }
  GD.voice(0, 0, 0,    0 ,    0);
} // end loss tone 

void StackIt() {
GD.voice(0, 0, 0,0,0);
  long logo[18];
  logo[0]  = 0x00000000;  // hex is revese pattern  1 = # 0 = . 
  logo[1]  = 0x00498df6;  // .##.#####.##...##..#..#.
  logo[2]  = 0x002a5249;  // #..#..#..#..#.#..#.#.#..
  logo[3]  = 0x00185241;  // #.....#..#..#.#....##...
  logo[4]  = 0x00185e46;  // .##...#..####.#....##...
  logo[5]  = 0x00285248;  // ...#..#..#..#.#....#.#..
  logo[6]  = 0x004a5249;  // #..#..#..#..#.#..#.#..#.
  logo[7]  = 0x00899246;  // .##...#..#..#..##..#...#
  logo[8]  = 0x00000000;
  logo[9]  = 0x0003e7c0;  // ......#####..#####......
  logo[10] = 0x00008100;  // ........#......#........
  logo[11] = 0x00008100;  // ........#......#........
  logo[12] = 0x00008100;  // ........#......#........
  logo[13] = 0x00008100;  // ........#......#........
  logo[14] = 0x00008100;  // ........#......#........
  logo[15] = 0x000087c0;  // ......#####....#........
  logo[16] = 0x00000000;
  logo[17] = 0x00ffffff;  // ########################
  for (int i = 17 ; i >= 0 ; i --) {
    cubeMove[i] = logo[i];
      displaySprites();
      delay (300); 
    }
  WinTone();
  delay (500);
  resetPlay();
} // end Stack it logo 

void selfPlay() {
  if (selfPlayCount >= 300) {
      detachInterrupt(0);
      attachInterrupt(0, exitSelfPlay, LOW);
      GD.putstr(0, 0, "PRESS BUTTON TO PLAY");
      moveToneflag = false;
      if (logoCount >= 51 ){
        StackIt();
        logoCount = 0;
      }  
      randomSeed(analogRead(0));
      if (level == 0 && random(10) == 5){
        selfPlayButton();     
      }
      else if ((cubeMove[16-level] == cubeMove[17-level])) {
        if (random(2) == 1){
        RowShift();
        delay (120 - ( level * 6));
        displaySprites();
      }
      if (random(2) == 1) {
        selfPlayButton();
      }
      } // end else if level check
  } // end if self play count check
  else {
      selfPlayCount++ ;   
  }
} // end self play

void selfPlayButton() {
  GD.voice(0, 0, 5000, 254, 254);
  delay (50);
  WinState();
  logoCount++;
  GD.voice(0,0,0,0,0); 
} // end self play button

void exitSelfPlay(){
  GD.voice(0, 0, 5000,254 ,254);
  GD.putstr(0, 0, "                    ");
  while (digitalRead(2)== LOW){
  resetPlay();
  selfPlayCount = 0;
  logoCount = 51;
  moveToneflag = true;
  }
  GD.voice(0, 0, 0,   0 ,    0);
} // end exit self play


