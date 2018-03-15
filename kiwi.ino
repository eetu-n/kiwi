#include <Arduino.h>

#include <U8x8lib.h>     //Using u8g2 libraryE
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8 (U8X8_PIN_NONE);

#include "OOCSI.h"
const char* ssid = "Springerstraat 1";
const char* password = "solutio@13579";
const char* OOCSIName = "kiwi";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

#define vibPin 16

#include <SPI.h>
#include <MFRC522.h>
MFRC522 mfrc522(5, 17);

int uidSize;
String uid;
String tempUid;
String tempUid2;

String word1 = "Message ";
String word2 = " not set";

String uidList[10];
const char* messageList[10];
int lineList[10];
int clearLines;

String messageIn = "Hello World!";
const char* messageOut;

int delayer = 0;
int timer = 0;
bool vibrate = false;

int line = 0;

void setup(void) {
  Serial.begin(115200);
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  SPI.begin();
  mfrc522.PCD_Init();
  oocsi.setLogging(false);

  messageOut = messageIn.c_str();
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
  oocsi.subscribe("esp-testchannel");

  u8x8.begin();
  u8x8.setPowerSave(0);

  u8x8.setFont(u8x8_font_pxplusibmcgathin_f);

  pinMode(vibPin, OUTPUT);
  digitalWrite(vibPin, HIGH);
  delay(500);
  digitalWrite(vibPin, LOW);

}

void loop(void) {
  if (mfrc522.PICC_IsNewCardPresent()) {
    mfrc522.PICC_ReadCardSerial();

    uidSize = mfrc522.uid.size;

    for (int i = 0; i < uidSize; i++) {
      if (i != 0) {
        if (mfrc522.uid.uidByte[i] < 0x10) {
          tempUid2 = " 0";
        } else {
          tempUid2 = " ";
        }
      }
      tempUid = tempUid + tempUid2 + String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(tempUid);

    uid = tempUid;
    tempUid = "";

    for (int i = 0; i < 10; i++) {
      const int tempLine = lineList[i];
      const char* tempMessage = messageList[i];
      if (uidList[i] == uid) {
        if (clearLines == 8) {
          u8x8.clearDisplay();
        }
        u8x8.drawString(0, tempLine, tempMessage);
        Serial.println(messageList[i]);
        break;
      }
      if (uidList[i] == "") {
        uidList[i] = uid;
        messageList[i] = "";
        break;
      }
    }
  }
  oocsi.check();
}

void processOOCSI() {
  line = oocsi.getInt("line", 0);
  messageIn = oocsi.getString("message", "-200");
  messageOut = messageIn.c_str();
  delayer = oocsi.getInt("delay", 0);
  timer = oocsi.getInt("time", 0);
  uid = oocsi.getString("uid", 0);
  clearLines = oocsi.getInt("clearLines", 9);

  for (int i = 0; i < 10; i++) {
    if (uidList[i] == uid) {
      messageList[i] = messageOut;
      lineList[i] = line;
      break;
    }
    if (uidList[i] == "") {
      uidList[i] = uid;
      messageList[i] = messageOut;
      lineList[i] = line;
      break;
    }
  }

  Serial.print("Message for ");
  Serial.print(uid);
  Serial.print(" is ");
  Serial.println(messageOut);

  if (oocsi.getInt("vibrate", 0) == 0) {
    vibrate = false;
  } else {
    vibrate = true;
  }
}

