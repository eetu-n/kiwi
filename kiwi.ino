#include <Arduino.h>

#include <U8g2lib.h>     //Using u8g2 libraryE
#include "OOCSI.h"
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <FastLED.h>

#define NUM_LEDS 8    //Number of leds on the strip
#define vibPin 12     //Pin for the vibration motor
#define ledPin 13     //Pin for the LED strip
#define tagAmount 10  //Number of NFC tags the kiwi should be able to handle
#define DEBUG false

const char* ssid = "Narhi";
const char* password = "rusinapallo";
const char* OOCSIName = "kiwiScanner";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

int curMillis = 0;    // Used to keep track of time

String uid;                   //UID received from oocsi
String scannedUid;            //UID scanned by kiwi
String uidList[tagAmount];    //List of UIDs that have been received / scanned
int uidSize;                  //Internal int used when scanning UID
String tempUid;               // -||-
String tempUid2;              // -||-
bool scannedList[tagAmount];  //Whether a particular UID has already been scanned

String message0List[tagAmount];    //List of messages
String message1List[tagAmount];
String message2List[tagAmount];
String message3List[tagAmount];
String message0 = "Hello World!";  //Temporary variable used for receiving messages from oocsi
String message1 = "Hello World!";
String message2 = "Hello World!";
String message3 = "Hello World!";
int messageTime = 1000;           //Temporary variable used for receiving message times from oocsi
int messageTimeList[tagAmount];   //List of times messages should be displayed

int delayer = 0;                  //Temporary variable used to keep track of delay for vibration
int delayList[tagAmount];         //List of delay times
int timer = 0;                    //Time the motor should vibrate
int timeList[tagAmount];          //List of times motor should vibrate
int repeat = 1;                   //Temporary repeat variable
int repeatList[tagAmount];        //List of how many times motor should vibrate
int vibDelay;                     //Internal variable used for switching between delay and time in the method
bool vibOn = false;
int curDelay;
int curTime;
int curRepeat;

int hue = 0;                      //Temporary hue for receiving from oocsi
int hueList[tagAmount];           //List of Hues
int brightness = 0;               //Temporary brigthness for receiving from oocsi
int curBrightness = 0;            //Variable used to keep track of brightness for pulse effect
int targetBrightness = 0;         //Variable used to keep track of max brightness for pulse effect
int brightnessList[tagAmount];    //List of brightnessses
int tempLedMode = 0;              //Temporary LED mode variable
int ledMode = 0;                  // -||- for receiving from oocsi
int ledModeList[tagAmount];       //List of led modes
int divBy = 10;                   //Variable for determining pulse speed
bool direction = true;            //Whether brightness is decreasing or increasing

int ledMillis;  //Used to keep track of LED pulse timing
int vibMillis;  //Used to keep track of vibration timings
int nfcMillis;  //Used to keep track of scanning timings

U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);    //Initialize display
MFRC522 mfrc522(5, 17);                                             //Initialize NFC scanner
CRGB leds[NUM_LEDS];                                                //Initialize LED strip

void setup(void) {
  if (DEBUG) {
    Serial.begin(115200);
  }
  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);                    //Begin LED strip
  pinMode(vibPin, OUTPUT);                                              //Define vibration motor pin to output
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);                       //Set maximum gain for NFC scanner
  SPI.begin();                                                          //Begin SPI communication with scanner
  mfrc522.PCD_Init();                                                   //Couple scanner with SPI
  oocsi.setLogging(DEBUG);                                              //Disable oocsi messages

  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);   //Connect to oocsi
  oocsi.subscribe("kiwiReceiver");                                   //Subscribe to channel

  u8x8.begin();                                                         //Begin the display
  u8x8.setPowerSave(0);                                                 //Turn off power saving on display

  u8x8.setFont(u8x8_font_chroma48medium8_r);                            //Set display font
  u8x8.drawString(0, 0, "Welcome");                                     //Display welcome message

  digitalWrite(vibPin, LOW);

  ledMillis = millis();                                                 //Initialize timing variables
  vibMillis = millis();
  nfcMillis = millis();

}

void loop(void) {
  if (mfrc522.PICC_IsNewCardPresent()) {                                //If a card is detected
    getUID();                                                           //Get it's UID
    for (int i = 0; i < tagAmount; i++) {                               //Iterate through list of known UIDs
      if (uidList[i] == scannedUid && scannedList[i] == false) {        //If this card has not been scanned yet, procede
        updateLeds();                                                   //Update the LEDs
        printText();                                                    //Update display
        setVib();
        sendOocsi();                                                    //Send oocsi message that card was scanned
        updateScannedList();                                            //Update scannedList to reflect card has been scanned
      } else if (uidList[i] == ""){
        sendOocsi();
        uidList[i] = scannedUid;
      }
    }
    
  }

  ledPattern();                                                         //Pulse controller
  vibTimer();                                                           //Vibration controller
  oocsi.check();                                                        //Check for new oocsi input
}

void processOOCSI() {
  uid = oocsi.getString("uid", "0");

  message0 = oocsi.getString("message0", "-200");
  message1 = oocsi.getString("message1", "-200");
  message2 = oocsi.getString("message2", "-200");
  message3 = oocsi.getString("message3", "-200");

  delayer = oocsi.getInt("delay", 0);
  timer = oocsi.getInt("time", 0);
  repeat = oocsi.getInt("repeat", 0);

  hue = oocsi.getInt("hue", 0);
  brightness = oocsi.getInt("brightness", 0);
  tempLedMode = oocsi.getInt("ledMode", 0);

  if (uid == "0") {
    execute();
  }

  for (int i = 0; i < tagAmount; i++) {  //Records all received values in the same index in all arrays
    if (uid == "0") {
      break;
    }
    if (uidList[i] == uid) {
      message0List[i] = message0;
      message1List[i] = message1;
      message2List[i] = message2;
      message3List[i] = message3;

      repeatList[i] = repeat;
      delayList[i] = delayer;
      timeList[i] = timer;

      hueList[i] = hue;
      brightnessList[i] = brightness;
      ledModeList[i] = tempLedMode;
      scannedList[i] = false;

      if (DEBUG) {
        Serial.print("Message0: ");
        Serial.println(message0List[i]);
        Serial.print("Message1: ");
        Serial.println(message1List[i]);
        Serial.print("Message2: ");
        Serial.println(message2List[i]);
        Serial.print("Message3: ");
        Serial.println(message3List[i]);
        Serial.print("Hue: ");
        Serial.println(hueList[i]);
        Serial.print("Brightness: ");
        Serial.println(brightnessList[i]);
        Serial.print("Led Mode: ");
        Serial.println(ledModeList[i]);
        Serial.print("Repeat: ");
        Serial.println(repeatList[i]);
      }
      break;
    } else if (uidList[i] == "") {        //If the received UID is not yet scanned. Same as above, also records the UID in an empty slot
      uidList[i] = uid;

      message0List[i] = message0;
      message1List[i] = message1;
      message2List[i] = message2;
      message3List[i] = message3;

      repeatList[i] = repeat;
      delayList[i] = delayer;
      timeList[i] = timer;

      hueList[i] = hue;
      brightnessList[i] = brightness;
      ledModeList[i] = tempLedMode;

      scannedList[i] = false;

      if (DEBUG) {
        Serial.print("UID: ");
        Serial.println(uidList[i]);
        Serial.print("Message0: ");
        Serial.println(message0List[i]);
        Serial.print("Message1: ");
        Serial.println(message1List[i]);
        Serial.print("Message2: ");
        Serial.println(message2List[i]);
        Serial.print("Message3: ");
        Serial.println(message3List[i]);
        Serial.print("Hue: ");
        Serial.println(hueList[i]);
        Serial.print("Brightness: ");
        Serial.println(brightnessList[i]);
        Serial.print("Led Mode: ");
        Serial.println(ledModeList[i]);
        Serial.print("Repeat: ");
        Serial.println(repeatList[i]);
      }
      break;
    }
  }
}

void updateLeds() {                                     //Sets the hue and brigthness of the LEDs
  for (int i = 0; i < tagAmount; i++) {                 //Iterate through UID list
    if (uidList[i] == scannedUid) {                     //Selects the UID that was just scanned
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

void getUID() {                                                             //Reads UID
  mfrc522.PICC_ReadCardSerial();

  uidSize = mfrc522.uid.size;                                               //Sets size of UID. Usually 4 bytes.

  for (int i = 0; i < uidSize; i++) {                                       //Iterate through each byte in the UID
    if (i != 0) {                                                           //If the current byte is not 0
      if (mfrc522.uid.uidByte[i] < 0x10) {                                  //And if it is less than 10
        tempUid2 = " 0";                                                    //Add a space and 0 prefix (to make sure all bytes are displayed separately and with two digits)
      } else {
        tempUid2 = " ";                                                     //If byte is above 10, just add a space prefix
      }
    } else {
      tempUid2 = "";                                                        //Makes sure first byte does not have a prefix
    }
    tempUid = tempUid + tempUid2 + String(mfrc522.uid.uidByte[i], HEX);     //Adds the read byte to the UID
  }
  scannedUid = tempUid;
  tempUid = "";
}

void printText() {                                                //Updates the text on the display
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      u8x8.clearDisplay();
      u8x8.drawString(0, 0, message0List[i].c_str());
      u8x8.drawString(0, 1, message1List[i].c_str());
      u8x8.drawString(0, 2, message2List[i].c_str());
      u8x8.drawString(0, 3, message3List[i].c_str());
      break;
    } else if (uidList[i] == "") {                                //TODO: Move this to appropriate method
      uidList[i] = scannedUid;
      message0List[i] = "";
      message1List[i] = "";
      message2List[i] = "";
      message3List[i] = "";
      break;
    }
  }
}

void ledPattern() {                                                       //Controls LED pulse effect
  if (ledMode == 0) {                                                     //Returns if no pulse, otherwise sets speed
    return;
  } else if (ledMode == 1) {
    divBy = 10;
  } else if (ledMode == 2) {
    divBy = 20;
  }

  curMillis = millis();

  if (curBrightness <= 1 || curBrightness >= targetBrightness) {         //If the brigthness is at either extreme (min or max), reverse the direction
    direction = !direction;
  }

  if (curMillis - ledMillis >= 10) {                                      //Makes sure 10ms has passed since last update
    if (direction) {                                                      //Determines whether to reduce or increase brightness
      curBrightness = int(curBrightness - (targetBrightness / divBy));
    } else {
      curBrightness = int(curBrightness + (targetBrightness / divBy));
    }

    if (curBrightness < 1) {                                              //If brightness is below one or above target, set it to one or target respectively. Error prevention
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

void vibTimer() {                                 //Controls vibration timings
  curMillis = millis();
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      if (curRepeat > 0) {
        if (vibOn) {
          vibDelay = curTime;
        } else {
          vibDelay = curDelay;
        }
        if (curMillis - vibMillis >= vibDelay) {
          if (vibOn) {
            if (DEBUG) {
              Serial.println("Vib off");
            }
            digitalWrite(vibPin, LOW);
            vibOn = false;
            curRepeat--;
          } else {
            if (DEBUG) {
              Serial.println("Vib on");
            }
            digitalWrite(vibPin, HIGH);
            vibOn = true;
          }
          vibMillis = millis();
        }
      } else {
        vibOn = false;
        digitalWrite(vibPin, LOW);
      }
    }
  }
}

void setVib() {
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      curDelay = delayList[i];
      curTime = timeList[i];
      curRepeat = repeatList[i];
    }
  }
}

void updateScannedList() {
  for (int i = 0; i < tagAmount; i++) {
    if (uidList[i] == scannedUid) {
      if (DEBUG) {
        Serial.print("UID: ");
        Serial.println(uidList[i]);
        Serial.print("ScannedList: ");
        Serial.println(scannedList[i]);
        Serial.print("i: ");
        Serial.println(i);
      }
      scannedList[i] = true;
    }
  }
}

void sendOocsi() {
  oocsi.newMessage("kiwiSender").addString("scannedUid", scannedUid.c_str()).sendMessage();
  if (DEBUG){
    Serial.println(scannedUid);
  }
}

void execute() {
  curDelay = delayer;
  curTime = timer;
  curRepeat = repeat;

  const int tempHue = hue;
  const int tempBrightness = brightness;
  targetBrightness = brightness;
  curBrightness = brightness;
  ledMode = tempLedMode;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHue(tempHue);
    leds[i].maximizeBrightness();
    leds[i].nscale8(tempBrightness);
  }

  u8x8.clearDisplay();
  u8x8.drawString(0, 0, message0.c_str());
  u8x8.drawString(0, 1, message1.c_str());
  u8x8.drawString(0, 2, message2.c_str());
  u8x8.drawString(0, 3, message3.c_str());
}

