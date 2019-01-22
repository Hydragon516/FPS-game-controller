#include <SPI.h>
#include <Mouse.h>
#include <Keyboard.h>
#include "RF24.h"

int msg[6];
int msg1;
int msg2;
  
byte address[6] = "1Node";
  
RF24 radio(7,8);  // CE, CSN
  
void setup(void) {
  Serial.begin(9600);
  
  radio.begin();
  Mouse.begin();
  
  radio.openReadingPipe(1, address);
  radio.startListening();
}
  
void loop(void) {
  if(radio.available()) {
  
    radio.read(&msg, sizeof(msg)); 
    
    Serial.print("Meassage (RX) = ");
  
    Serial.print(msg[0]);
    Serial.print(msg[1]);
    Serial.print(msg[2]);
    Serial.print(msg[3]);
    Serial.println(msg[4]);
  
    Mouse.move(msg[1], -msg[0]);
/*--------------------------------------------*/
    if(msg[2]==1){
      Mouse.press();
      }
    else{
      Mouse.release();
      }
/*--------------------------------------------*/
    if(msg[5]==1){
      Mouse.press(MOUSE_RIGHT);
      }
    else{
      Mouse.release(MOUSE_RIGHT);
      }
/*--------------------------------------------*/      
    if(msg[3]>540){
      Keyboard.press('w');
      }
       
    else if(msg[3]<500){
      Keyboard.press('s');
      }
       
    else if(msg[4]>540){
      Keyboard.press('d');
      }
       
    else if(msg[4]<500){
      Keyboard.press('a');
      }
    else {
      Keyboard.releaseAll();
      }
    }  
}
