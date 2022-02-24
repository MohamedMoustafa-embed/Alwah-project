#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mp3Ser(11, 10); // RX, TX
DFRobotDFPlayerMini mp3 ;
void printDetail(uint8_t type, int value);
int a1 = 0, a2 = 0, a3 = 0, flag = 0;
void setup() {
  mp3Ser.begin(9600);
  // mp3.setTimeOut(100);
  Serial.begin(115200);
  pinMode(A4, INPUT);
  delay(1);
  if (!mp3.begin(mp3Ser))
  {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  mp3.volume(30);
  delay(5);
}

void loop() {
  if (mp3.available()) {
    printDetail(mp3.readType(), mp3.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  flag = 0; Serial.println("flag");
  while (flag == 0) {
    a1 = analogRead(A4);
    a2 = analogRead(A4);
    a3 = analogRead(A4);
    if ((a1 == a2) && (a3 == a2) && (a3 > 30))flag = (a2);
    delay(100);
  }
  a2 = a1 = a3 = flag;
  Serial.println(flag);
  if ((a1 < 38) && (a1 > 31)) {
    //mp3.playFolder(01, 001);
    Serial.println("h");
    flag = 0;
  }
  else if ((a1 <= 100) && (a1 > 85)) {
    mp3.playFolder(01, 006);
    //Serial.println("g");
    flag = 0;
  }
  else if ((a1 <= 185) && (a1 > 170)) {
    mp3.playFolder(01, 001);//بسم الله اللرحمن الرحيم 
    Serial.println("g");
    flag = 0;
  }
  else if ((a1 <= 255) && (a1 > 235)) {//قل هو
     mp3.playFolder(01 ,002);
    Serial.println("g");
    flag = 0;
  }
  else if ((a1 <= 323) && (a1 > 303)) {//الله الصمد
    mp3.playFolder(01, 003);
    Serial.println("g");
    flag = 0;
}
 else if ((a1 <= 523) && (a1 > 503)) {//لم يلد
    mp3.playFolder(01, 004);
    Serial.println("g");
    flag = 0;
}
else if ((a1 <= 423) && (a1 > 403)) {
    mp3.playFolder(01, 005);
    Serial.println("g");
    flag = 0;
}
 else if ((a1 <= 750) && (a1 > 730)) {
    mp3.playFolder(01, 005);
    Serial.println("g");
    flag = 0;
}
}























void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
