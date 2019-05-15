/*
 * Programed by Macnolo Tech <https://macnolo.tk>
 * Assembly by Future Tech 
 * 
 * 
 * Copyright 2019
 * License: GPLv3
*/

#include "MFRC522.h"
#include "MFRC522Extended.h"
#include "require_cpp11.h"
#include <Servo.h>
#include <EEPROM.h>
#include <SPI.h>

#define SS_PIN 10 
#define RST_PIN 9
#define SERVO_PIN 8

#define OPEN_G 70
#define CLOSE_G 120

//Setting RFID R/W
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte nuidPICC[4];

//Setting state-lock variables
boolean lock = false;
int state = 0;
String cmd;

//Setting Servo
Servo servo;

//Setting user variables
byte master[4] = {0xF5,0xDD,0x00,0xE5};
byte user1[4] = {0x9E,0x62,0x0D,0x21};
byte user2[4] = {0x69,0x3A,0x56,0xD3};

void setup() {
  //Init Serial, Servo, SPI and RFID
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(SERVO_PIN);

  //Setting General PINS
  pinMode(LED_BUILTIN, OUTPUT);

  //Setting KEY Memory
  for (byte i = 0;i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  //Restoring last state
  if(EEPROM.read(0) == 2) {
    state = 1;
    servo.write(CLOSE_G);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Restoring. Closing...\n");
    delay(200);
  }else if(EEPROM.read(0) == 1) {
    state == 0;
    servo.write(OPEN_G);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Restoring. Opening...\n");
    delay(200);
  }else {
    Serial.println("No other data. New data created.\n");
  }

  //Presenting
  Serial.println("Welcome to AutoLocker Program!");
  Serial.println("Programed by Macnolo Tech <https://macnolo.tk>");
  Serial.println("Assembly instructions by Future Tech");
  Serial.println("\n");

}

void loop() {
  //Looking for commands
  cmd = Serial.readString();
  if(cmd != ""){
    if (cmd.compareTo("restore")){
      if(restore()){
        Serial.println("Restore Success. Please press reset button.");
      }else{
        Serial.println("Restore failed.");
      }
    }
  }
  
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  //Checking MIFARE type
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("\nTag format: ");
  Serial.print(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //Reading nuidPICC and writing to array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  Serial.println("\nNUID is:");
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.print("\n");

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  //Checking for access
  if(compareArray(nuidPICC,master)){
    Serial.println("Access Granted: Macnolo0x7D4");
    lock = true;
  }else if(compareArray(nuidPICC,user1)){
    Serial.println("Access Granted: Rubek");
    lock = true;
  }else if(compareArray(nuidPICC,user2)){
    Serial.println("Access Granted: Diego");
    lock = true;
  }else{
    Serial.println("Access Denied");
    lock = false;
  }

  delay(1000);
  
  //Checking lock state
  if(lock){
    if (state == 0){
      EEPROM.write(0, 2);
      state = 1;
      digitalWrite(LED_BUILTIN, LOW);      
      servo.write(CLOSE_G);
      Serial.println("Closing");
    }else if (state == 1){
      EEPROM.write(0, 1);
      state = 0;
      digitalWrite(LED_BUILTIN, HIGH);
      servo.write(OPEN_G);
      Serial.println("Opening");
    }
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

boolean compareArray(byte array1[],byte array2[]) {
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}

boolean restore(){
  for(int i = 0; i < EEPROM.length() ; i++){
    EEPROM.write(i,0);
  }
  return 1;
}
