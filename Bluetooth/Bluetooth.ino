/*
 * Programa de codigo abierto creado para una puerta de bloqueo por Bluetooth. 
 * Creado por Macnolo Tech y Future Technologies
 * Encriptacion desde la app que es comparada en este programa(MD5).
 * 
 * Copyright 2018 Macnolo Tech
 */
#include <Servo.h>
#include "EEPROM.h"
#include "SoftwareSerial.h"

const int pin = 9;
const int rx = 2;
const int tx = 3;
boolean lock = false;
String cmd;
int state = 0;
String correctPassword1 = "fd272fe04b7d4e68effd01bddcc6bb34";
String correctPassword2 = "451b7ed3a3f81564a51f3b904e345406";

SoftwareSerial bt(rx, tx);
Servo servo;

void setup() {
  pinMode(rx, INPUT);
  pinMode(tx, OUTPUT);
  Serial.begin(9600);
  bt.begin(9600);
  servo.attach(pin);
  pinMode(13, OUTPUT);
  if(EEPROM.read(0) == 1){
    state = 1;
    servo.write(120);
    digitalWrite(13, LOW);
    Serial.println("Reanudando. Cerrando");
    delay(200);
  }else{
    state == 0;
    servo.write(70);
    digitalWrite(13, HIGH);
    Serial.println("Reanudando. Abriendo");
    delay(200);
   }
}

void loop() {
  if (bt.available() > 0){
    cmd = bt.readString();
    if(cmd.equals(correctPassword1)){
      lock = true;
    }else if(cmd.equals(correctPassword2)){
      lock = true;
    }else{
      lock = false;
    }
    
    if(lock){
      if (state == 0){
        EEPROM.write(0, 1);
        state = 1;
        digitalWrite(13, LOW);      
        servo.write(120);
        Serial.println("Cerrando");
      }else if (state == 1){
        EEPROM.write(0, 0);
        state = 0;
        digitalWrite(13, HIGH);
        servo.write(70);
        Serial.println("Abriendo");
      }
    }
  }
}


