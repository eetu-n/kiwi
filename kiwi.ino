#include <Arduino.h>

#include <U8g2lib.h>     //Using u8g2 libraryE
#include "OOCSI.h"
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <FastLED.h>

#define NUM_LEDS 8
#define vibPin 12
#define ledPin 13
#define tagAmount 10

const char* ssid = "Narhi";
const char* password = "rusinapallo";
const char* OOCSIName = "kiwi";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

int curMillis = 0;

String oocsiMessage = "scannedUid";

String uid;
String scannedUid;
String uidList[tagAmount];
int uidSize;
String tempUid;
String tempUid2;
bool scannedList[tagAmount];

String messageList[tagAmount];
String message = "Hello World!";
int messageTime = 1000;
int messageTimeList[tagAmount];

int lineList[tagAmount];
int line = 0;

int delayer = 0;
int delayList[tagAmount];
int timer = 0;
int timeList[tagAmount];
bool vibrate = false;
bool vibList[tagAmount];
int repeat = 1;
int repeatList[tagAmount];
int vibOn = false;
int vibDelay;

int hue = 0;
int hueList[tagAmount];
int brightness = 0;
int curBrightness = 0;
int targetBrightness = 0;
int brightnessList[tagAmount];
int tempLedMode = 0;
int ledMode = 0;
int ledModeList[tagAmount];
int divBy = 10;
bool direction = true;

int ledMillis;
int vibMillis;
int nfcMillis;

U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
MFRC522 mfrc522(5, 17);
CRGB leds[NUM_LEDS];

void setup(void) {
 //Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
  pinMode(vibPin, OUTPUT);
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  SPI.begin();
  mfrc522.PCD_Init();
  oocsi.setLogging(false);

  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
  oocsi.subscribe("esp-testchannel");

  u8x8.begin();
  u8x8.setPowerSave(0);

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0, "Welcome");

  ledMillis = millis();
  vibMillis = millis();
  nfcMillis = millis();

}

void loop(void) {
  if (mfrc522.PICC_IsNewCardPresent()) {
    getUID();
    for (int i = 0; i < tagAmount; i++) {
      if (uidList[i] == scannedUid && scannedList[i] == false) {
        updateLeds();
        printText();
        sendOocsi();
        updateScannedList();
      }
    }
  }

  ledPattern();
  vibTimer();
  oocsi.check();
}

void processOOCSI() {
  uid = oocsi.getString("uid", 0);

  line = oocsi.getInt("line", 0);
  message = oocsi.getString("message", "-200");

  delayer = oocsi.getInt("delay", 0);
  timer = oocsi.getInt("time", 0);
  repeat = oocsi.getInt("repeat", 0);

  hue = oocsi.getInt("hue", 0);
  brightness = oocsi.getInt("brightness", 0);
  tempLedMode = oocsi.getInt("ledMode", 0);


  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == uid) {
      messageList[i] = message;
      lineList[i] = line;

      repeatList[i] = repeat;
      delayList[i] = delayer;
      timeList[i] = timer;

      hueList[i] = hue;
      brightnessList[i] = brightness;
      ledModeList[i] = tempLedMode;
      scannedList[i] = false;

      //Serial.print("Message: ");
      //Serial.println(messageList[i]);
      //Serial.print("Line: ");
      //Serial.println(lineList[i]);
      //Serial.print("Hue: ");
      //Serial.println(hueList[i]);
      //Serial.print("Brightness: ");
      //Serial.println(brightnessList[i]);
      //Serial.print("Led Mode: ");
      //Serial.println(ledModeList[i]);
      //Serial.print("Repeat: ");
      //Serial.println(repeatList[i]);
      break;
    } else if (uidList[i] == "") {
      uidList[i] = uid;

      messageList[i] = message;
      lineList[i] = line;

      repeatList[i] = repeat;
      delayList[i] = delayer;
      timeList[i] = timer;

      hueList[i] = hue;
      brightnessList[i] = brightness;
      ledModeList[i] = tempLedMode;

      scannedList[i] = false;

      //Serial.print("UID: ");
      //Serial.println(uidList[i]);
      //Serial.print("Message: ");
      //Serial.println(messageList[i]);
      //Serial.print("Line: ");
      //Serial.println(lineList[i]);
      //Serial.print("Hue: ");
      //Serial.println(hueList[i]);
      //Serial.print("Brightness: ");
      //Serial.println(brightnessList[i]);
      //Serial.print("Led Mode: ");
      //Serial.println(ledModeList[i]);
      //Serial.print("Repeat: ");
      //Serial.println(repeatList[i]);
      break;
    }
  }
  for (int i = 0; i < 4; i++) {
    //Serial.println(messageList[i]);
  }

  if (oocsi.getInt("vibrate", 0) == 0) {
    vibrate = false;
  } else {
    vibrate = true;
  }
}

void updateLeds() {
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      const int tempHue = hueList[i];
      const int tempBrightness = brightnessList[i];
      targetBrightness = brightnessList[i];
      curBrightness = brightnessList[i];
      ledMode = ledModeList[i];
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].setHue(tempHue);
        leds[i].maximizeBrightness();
        leds[i].nscale8(tempBrightness);
      }
    }
  }
  ledMillis = millis();
  direction = true;
  FastLED.show();
}

void getUID() {
  mfrc522.PICC_ReadCardSerial();

  uidSize = mfrc522.uid.size;

  for (int i = 0; i < uidSize; i++) {
    if (i != 0) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
        tempUid2 = " 0";
      } else {
        tempUid2 = " ";
      }
    } else {
      tempUid2 = "";
    }
    tempUid = tempUid + tempUid2 + String(mfrc522.uid.uidByte[i], HEX);
  }
  scannedUid = tempUid;
  tempUid = "";
}

void printText() {
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      u8x8.clearDisplay();
      u8x8.drawString(0, lineList[i], messageList[i].c_str());
      break;
    } else if (uidList[i] == "") {
      uidList[i] = scannedUid;
      messageList[i] = "";
      break;
    }
  }
}

void ledPattern() {
  if (ledMode == 0) {
    return;
  } else if (ledMode == 1) {
    divBy = 10;
  } else if (ledMode == 2) {
    divBy = 20;
  }

  curMillis = millis();

  if (curBrightness <= 1 || curBrightness >= targetBrightness) {
    direction = !direction;
  }

  if (curMillis - ledMillis >= 10) {
    if (direction) {
      curBrightness = int(curBrightness - (targetBrightness / divBy));
    } else {
      curBrightness = int(curBrightness + (targetBrightness / divBy));
    }

    if (curBrightness < 1) {
      curBrightness = 1;
    } else if (curBrightness > targetBrightness) {
      curBrightness = targetBrightness;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].maximizeBrightness();
      leds[i].nscale8(curBrightness);
    }

    FastLED.show();
    ledMillis = millis();
  }
}

void vibTimer() {
  curMillis = millis();
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      if (repeatList[i] > 0) {
        if (vibOn) {
          vibDelay = timeList[i];
        } else {
          vibDelay = delayList[i];
        }
        if (curMillis - vibMillis >= vibDelay) {
          repeatList[i]--;
          if (vibOn) {
            //Serial.println("Low");
            digitalWrite(vibPin, LOW);
            vibOn = false;
          } else {
            //Serial.println("High");
            digitalWrite(vibPin, HIGH);
            vibOn = true;
          }
          vibMillis = millis();
        }
      }
    }
  }
}

void updateScannedList() {
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
     //Serial.print("UID: ");
     //Serial.println(uidList[i]);
     //Serial.print("ScannedList: ");
     //Serial.println(scannedList[i]);
     //Serial.print("i: ");
     //Serial.println(i);
      scannedList[i] = true;
    }
  }
}

void sendOocsi(){
  oocsi.newMessage("esp-testchannel");
  oocsi.addString(oocsiMessage.c_str(), scannedUid.c_str());
  oocsi.sendMessage();
}

